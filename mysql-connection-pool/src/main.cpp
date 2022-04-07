#include <iostream>

#include "../include/ConnectionPool.h"
#include "../include/MySQLConn.h"

#define TEST_QUERY_

void query() {
  MySQLConn conn;
  auto flag = conn.connect("localhost", "mrxovo", "123@", "mytest");
  if (flag) {
    std::string sql = "INSERT INTO people VALUES('1234','Lili','female',24)";
    flag = conn.insert(sql);
    std::cout << (flag ? "Insert succeeded!\n" : "Insert failed!\n");

    sql = "select * from people";
    conn.query(sql);
    auto num_fields = conn.num_fields();
    while (conn.next()) {
      for (unsigned int i = 0; i < num_fields; ++i) {
        std::cout << conn.value(i) << (i < num_fields - 1 ? ", " : "\n");
      }
    }
  }
}

int main() {
#if defined TEST_QUERY_
  query();
#endif
  return 0;
}