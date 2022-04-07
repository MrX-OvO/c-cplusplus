#ifndef CONNECTION_POOL_
#define CONNECTION_POOL_

#include <json/json.h>

#include <chrono>
#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "../include/MySQLConn.h"

class ConnectionPool {  // 单例模式
 public:
  static ConnectionPool* getConnectionPool();
  ConnectionPool(const ConnectionPool&) = delete;
  ConnectionPool& operator=(const ConnectionPool&) = delete;
  ~ConnectionPool();
  std::shared_ptr<MySQLConn> getConn();

 private:
  ConnectionPool();
  // 解析json配置文件
  bool parseJsonFile(std::string jsonFile);
  // 添加数据库连接
  void addConn();
  // 创建和销毁数据库连接
  void produceConn();
  void recycleConn();

  // 数据库连接池队列
  std::queue<MySQLConn*> m_conn_Q;
  // 数据库连接所需参数
  std::string m_host;
  std::string m_user;
  std::string m_passwd;
  std::string m_db;
  unsigned int m_port;
  //最小连接数和最大连接数
  unsigned int m_min;
  unsigned int m_max;
  // 超时时间和空闲时间
  unsigned int m_timeout;
  unsigned int m_max_idle_time;
  // 互斥锁
  std::mutex m_mutex_Q;
  // 条件变量
  std::condition_variable m_cond;
};

#endif  // CONNECTION_POOL_