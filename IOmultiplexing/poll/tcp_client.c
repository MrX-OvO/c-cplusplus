#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {

  // 创建通信套接字文件描述符
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket");
    return -1;
  }

  // 绑定服务器IP和端口号
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(9998);
  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

  // 连接服务器
  int ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret == -1) {
    perror("connect");
    return -1;
  }

  int num = 0;

  // 通信
  while (1) {
    // 发数据
    char buff[1024];
    // fgets(buff, sizeof(buff), stdin);
    sprintf(buff, "hello world, number = %d\n", num++);
    write(fd, buff, strlen(buff) + 1);

    // 如果客户端没有发送数据，默认阻塞
    // 读数据
    memset(buff, 0, sizeof(buff));
    int len = read(fd, buff, sizeof(buff));
    if (len == -1) {
      perror("read");
      return -1;
    } else if (len == 0) {
      printf("server has disconnected to client!!!\n");
      break;
    }
    printf("server send: %s\n", buff);
    sleep(1);
  }

  // 关闭通信套接字文件描述符
  close(fd);

  return 0;
}
