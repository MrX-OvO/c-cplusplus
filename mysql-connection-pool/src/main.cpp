#include <iostream>
#include <vector>

#include "../include/ConnectionPool.h"
#include "../include/MySQLConn.h"

void query() {
  MySQLConn conn;
  auto flag = conn.connect("localhost", "mrxovo", "123@", "mytest");
  if (flag) {
    std::string sql = "INSERT INTO people VALUES('1234','Lili','female',24)";
    flag = conn.insert(sql);
    std::cout << (flag ? "插入数据成功1\n" : "插入数据失败！\n");

    sql = "select * from people";
    flag = conn.query(sql);
    if (flag) {
      auto num_fields = conn.num_fields();
      while (conn.next()) {
        for (unsigned int i = 0; i < num_fields; ++i) {
          std::cout << conn.value(i) << (i < num_fields - 1 ? ", " : "\n");
        }
      }
    } else {
      std::cout << "查询失败！\n";
    }
  } else {
    std::cout << "query 连接数据库失败！\n";
  }
}

void op1(int begin, int end) {
  for (int i = begin; i < end; ++i) {
    MySQLConn conn;
    auto flag = conn.connect("localhost", "mrxovo", "123@", "mytest");
    if (flag) {
      char sql[1024] = {0};
      std::sprintf(sql, "INSERT INTO people VALUES(%d,'Lili','female',24)", i);
      flag = conn.insert(sql);
      if (!flag) {
        std::cout << "op1 插入数据失败！\n";
      };
    } else {
      std::cout << "op1 连接数据库失败！\n";
    }
  }
}

void op2(ConnectionPool* pool, int begin, int end) {
  for (int i = begin; i < end; ++i) {
    std::shared_ptr<MySQLConn> conn = pool->getConn();
    auto flag = conn->connect("localhost", "mrxovo", "123@", "mytest");
    if (flag) {
      char sql[1024] = {0};
      std::sprintf(sql, "INSERT INTO people VALUES(%d,'Lili','female',24)", i);
      flag = conn->insert(sql);
      if (!flag) {
        std::cout << "op2 插入数据失败！\n";
      };
    } else {
      std::cout << "op2 连接数据库失败！\n";
    }
  }
}

// 1.单线程：使用/不使用线程池
void test1() {
#if 0
  // 单线程，不使用线程池，用时：5892342936纳秒, 5892毫秒
  auto begin = std::chrono::steady_clock::now();
  op1(0, 10000);
  auto end = std::chrono::steady_clock::now();
  auto ns = end - begin;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ns);
  std::cout << "单线程，不使用线程池，用时：" << ns.count() << "纳秒, "
            << ms.count() << "毫秒\n";
#else
  // 单线程，使用线程池，用时：5807077602纳秒, 5807毫秒
  ConnectionPool* pool = ConnectionPool::getConnectionPool();
  auto begin = std::chrono::steady_clock::now();
  op2(pool, 0, 10000);
  auto end = std::chrono::steady_clock::now();
  auto ns = end - begin;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ns);
  std::cout << "单线程，使用线程池，用时：" << ns.count() << "纳秒, "
            << ms.count() << "毫秒\n";
#endif
}

// 2.多线程：使用/不使用线程池
void test2() {
#if 1
  // 多线程，不使用线程池，用时：2242853362纳秒, 2242毫秒
  std::vector<std::thread> thread_vec;
  auto begin = std::chrono::steady_clock::now();
  for (int i = 0; i < 10; ++i) {
    thread_vec.emplace_back(std::thread(op1, i * 1000, (i + 1) * 1000));
  }
  for (auto& t : thread_vec) {
    t.join();
  }
  auto end = std::chrono::steady_clock::now();
  auto ns = end - begin;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ns);
  std::cout << "多线程，不使用线程池，用时：" << ns.count() << "纳秒, "
            << ms.count() << "毫秒\n";
#else
  // 多线程，使用线程池，用时：2388380607纳秒, 2388毫秒
  ConnectionPool* pool = ConnectionPool::getConnectionPool();
  std::vector<std::thread> thread_vec;
  auto begin = std::chrono::steady_clock::now();
  for (int i = 0; i < 10; ++i) {
    thread_vec.emplace_back(std::thread(op2, pool, i * 1000, (i + 1) * 1000));
  }
  for (auto& t : thread_vec) {
    t.join();
  }
  auto end = std::chrono::steady_clock::now();
  auto ns = end - begin;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ns);
  std::cout << "多线程，使用线程池，用时：" << ns.count() << "纳秒, "
            << ms.count() << "毫秒\n";
#endif
}

int main() {
#if 0
  test1();
#else
  test2();
#endif
  return 0;
}