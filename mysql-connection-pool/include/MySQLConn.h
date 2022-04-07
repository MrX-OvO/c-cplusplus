#ifndef MYSQLCONN_H_
#define MYSQLCONN_H_

#include <mysql/mysql.h>

#include <chrono>
#include <iostream>

class MySQLConn {
 public:
  // 初始化数据库连接
  MySQLConn();
  // 释放数据库连接
  ~MySQLConn();
  // 连接数据库
  bool connect(std::string host, std::string user, std::string passwd,
               std::string db, unsigned int = 3306);

  // 更新数据库：insert, update, delete
  bool insert(std::string sql);
  bool update(std::string sql);
  bool remove(std::string sql);
  // 查询数据库：select
  bool query(std::string sql);
  // 遍历查询到的结果集
  bool next();
  // 得到结果集中的字段值
  std::string value(unsigned int index);
  // 事务操作
  bool transaction();
  // 提交事务
  bool commit();
  // 事务回滚
  bool rollback();
  // 刷新起始的空闲时间点
  void refreshAlive();
  // 计算连接存活的总时长
  long long aliveTime();
  // 计算表中字段数
  unsigned int num_fields();

 private:
  // 释放指向结果集的指针
  void FreeResultPtr();

  MYSQL* m_conn = nullptr;        // 数据库连接句柄
  MYSQL_RES* m_result = nullptr;  // 行的查询结果，结果集
  MYSQL_ROW m_row = nullptr;      // 1行数据
  using time_point = std::chrono::steady_clock::time_point;
  time_point m_alive_time;  // 记录时间点
};

#endif  // MYSQLCONN_H_
