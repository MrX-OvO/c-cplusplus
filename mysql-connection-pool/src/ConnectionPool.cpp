#include "../include/ConnectionPool.h"

ConnectionPool* ConnectionPool::getConnectionPool() {
  static ConnectionPool pool;
  return &pool;
}

ConnectionPool::~ConnectionPool() {
  if (!m_conn_Q.empty()) {
    auto conn = m_conn_Q.front();
    m_conn_Q.pop();
    delete conn;
  }
}

std::shared_ptr<MySQLConn> ConnectionPool::getConn() {
  std::unique_lock<std::mutex> locker(m_mutex_Q);
  while (m_conn_Q.empty()) {
    if (std::cv_status::timeout ==
        m_cond.wait_for(locker, std::chrono::milliseconds(m_timeout))) {
      if (m_conn_Q.empty()) {
        // return nullptr;
        continue;
      }
    }
  }
  auto connPtr =
      std::shared_ptr<MySQLConn>(m_conn_Q.front(), [this](MySQLConn* conn) {
        std::unique_lock<std::mutex> locker(m_mutex_Q);
        conn->refreshAliveTime();
        m_conn_Q.push(conn);
      });
  m_conn_Q.pop();
  m_cond.notify_all();

  return connPtr;
}

ConnectionPool::ConnectionPool() {
  // 加载配置文件
  std::string jsonFile = "config.json";
  auto flag = parseJsonFile(jsonFile);
  if (!flag) {
    return;
  }

  // 初始化线程池中的数据库连接
  for (size_t i = 0; i < m_min; ++i) {
    addConn();
  }

  // 创建两个线程，分别用于创建和销毁数据库连接
  std::thread produce(&ConnectionPool::produceConn, this);
  std::thread recycle(&ConnectionPool::recycleConn, this);
  produce.detach();
  recycle.detach();
}

bool ConnectionPool::parseJsonFile(std::string jsonFile) {
  std::ifstream ifs(jsonFile.c_str());
  if (ifs.is_open()) {
    Json::Reader rd;
    Json::Value root;
    if (rd.parse(ifs, root)) {
      if (root.isObject()) {
        m_host = root["host"].asString();
        m_user = root["user"].asString();
        m_passwd = root["passwd"].asString();
        m_db = root["db"].asString();
        m_port = root["port"].asInt();
        m_min = root["min"].asInt();
        m_max = root["max"].asInt();
        m_timeout = root["timeout"].asInt();
        m_max_idle_time = root["max_idle_time"].asInt();
        return true;
      }
    } else {
      std::cout << "解析json文件失败！\n";
    }
  } else {
    std::cout << "打开json文件失败！\n";
  }
  return false;
}

void ConnectionPool::addConn() {
  MySQLConn* conn = new MySQLConn();
  auto flag = conn->connect(m_host, m_user, m_passwd, m_db, m_port);
  if (flag) {
    conn->refreshAliveTime();
    m_conn_Q.push(conn);
  } else {
    std::cout << "连接数据库失败！添加数据库连接池连接失败！\n";
  }
}

void ConnectionPool::produceConn() {
  while (true) {
    std::unique_lock<std::mutex> locker(m_mutex_Q);
    while (m_conn_Q.size() >= m_min) {
      m_cond.wait(locker);
    }
    addConn();
    m_cond.notify_all();
  }
}

void ConnectionPool::recycleConn() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::unique_lock<std::mutex> locker(m_mutex_Q);
    while (m_conn_Q.size() > m_min) {
      auto conn = m_conn_Q.front();
      if (conn->aliveTime() >= m_max_idle_time) {
        m_conn_Q.pop();
        delete conn;
      } else {
        break;
      }
    }
  }
}