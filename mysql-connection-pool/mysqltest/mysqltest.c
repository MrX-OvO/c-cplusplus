#include <mysql/mysql.h>
#include <stdio.h>

int main() {
  // 1.初始化连接环境
  MYSQL *mysql = mysql_init(NULL);
  if (mysql == NULL) {
    printf("mysql_init() error\n");
    return -1;
  }

  // 2.连接数据库服务器
  const char *host = "localhost";
  const char *user = "mrxovo";
  const char *passwd = "123@";
  const char *db = "mytest";
  unsigned int port = 3306;
  mysql = mysql_real_connect(mysql, host, user, passwd, db, port, NULL, 0);
  if (mysql == NULL) {
    printf("mysql_real_connect() error\n");
    return -1;
  }

  printf("mysql api使用默认编码:%s\n", mysql_character_set_name(mysql));

  // 设置编码为utf-8
  const char *csname = "utf-8";
  mysql_set_character_set(mysql, csname);
  printf("mysql api使用修改后的编码：%s\n", mysql_character_set_name(mysql));

  printf("恭喜，连接数据库服务器成功了...\n");

  // 3.执行一个sql语句
  const char *q = "select * from mytest.people";
  int ret = mysql_query(mysql, q);
  if (ret != 0) {
    printf("mysql_query() error\n");
    printf("失败原因：%s\n", mysql_error(mysql));
    return -1;
  }

  // 4.显示执行sql语句结果
  MYSQL_RES *result = mysql_store_result(mysql);
  MYSQL_ROW row;
  MYSQL_FIELD *fields = mysql_fetch_fields(result);
  unsigned int num_fields = mysql_num_fields(result);
  int field_lengths[] = {10, 8, 6, 3};
  int flag = 1;
  while ((row = mysql_fetch_row(result))) {
    if (flag) {
      for (int i = 0; i < num_fields; ++i) {
        printf("%-*s|", field_lengths[i], fields[i].name);
      }
      printf("\n");
      flag = 0;
    }
    for (int i = 0; i < num_fields; ++i) {
      printf("%-*s|", field_lengths[i], row[i] ? row[i] : "NULL");
    }
    printf("\n");
  }

  // 5.资源回收
  mysql_free_result(result);
  mysql_close(mysql);

  return 0;
}
