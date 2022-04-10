#include "../include/MySQLConn.h"

#include <mysql/mysql.h>

MySQLConn::MySQLConn() {
  m_conn = mysql_init(nullptr);
  mysql_set_character_set(m_conn, "utf-8");
}

MySQLConn::~MySQLConn() {
  if (m_conn != nullptr) {
    mysql_close(m_conn);
  }
  FreeResultPtr();
}

bool MySQLConn::connect(std::string host, std::string user, std::string passwd,
                        std::string db, unsigned int port) {
  mysql_real_connect(m_conn, host.c_str(), user.c_str(), passwd.c_str(),
                     db.c_str(), port, nullptr, 0);
  return m_conn != nullptr;
}

bool MySQLConn::insert(std::string sql) {
  if (mysql_query(m_conn, sql.c_str())) {
    return false;
  }
  return true;
}

bool MySQLConn::update(std::string sql) {
  if (mysql_query(m_conn, sql.c_str())) {
    return false;
  }
  return true;
}

bool MySQLConn::remove(std::string sql) {
  if (mysql_query(m_conn, sql.c_str())) {
    return false;
  }
  return true;
}

bool MySQLConn::query(std::string sql) {
  FreeResultPtr();
  if (mysql_query(m_conn, sql.c_str())) {
    return false;
  }
  m_result = mysql_store_result(m_conn);
  return true;
}

bool MySQLConn::next() {
  if (m_result != nullptr) {
    m_row = mysql_fetch_row(m_result);
    if (m_row != nullptr) {
      return true;
    }
  }
  return false;
}

std::string MySQLConn::value(unsigned int index) {
  auto num_fields = mysql_num_fields(m_result);
  if (index < 0 || index >= num_fields) {
    return std::string();
  }
  auto val = m_row[index];
  auto length = mysql_fetch_lengths(m_result)[index];
  return std::string(val, length);
}

bool MySQLConn::transaction() { return mysql_autocommit(m_conn, false); }

bool MySQLConn::commit() { return mysql_commit(m_conn); }

bool MySQLConn::rollback() { return mysql_rollback(m_conn); }

void MySQLConn::refreshAliveTime() {
  m_alive_time = std::chrono::steady_clock::now();
}

long long MySQLConn::aliveTime() {
  auto nanosecs = std::chrono::steady_clock::now() - m_alive_time;
  auto millisecs =
      std::chrono::duration_cast<std::chrono::milliseconds>(nanosecs);
  return millisecs.count();
}

unsigned int MySQLConn::num_fields() {
  if (m_result != nullptr) {
    return mysql_num_fields(m_result);
  }
  return 0;
}

void MySQLConn::FreeResultPtr() {
  if (m_result != nullptr) {
    mysql_free_result(m_result);
    m_result = nullptr;
  }
}
