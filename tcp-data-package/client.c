#include "myheader.h"
#include "socket.h"

int main() {
  // 1.创建通信套接字
  int fd = createSocket();
  if (fd == -1)
    return -1;

  // 2.连接服务器
  const char *ip = "172.17.36.82";
  unsigned short port = 9999;
  int ret = connToHost(fd, ip, port);
  if (ret == -1)
    return -1;

  // 3.通信
  int fdOfTxt = open("The Old Man and the Sea.txt", O_RDONLY);
  int length = 0;
  char tmp[1024];
  while ((length = read(fdOfTxt, tmp, rand() % 1024)) > 0) {
    int ret = sendMsg(fd, tmp, length);
    if (ret == -1)
      return -1;
    printf("\n =+=+= send data to server +=+=+\n");
    memset(tmp, 0, sizeof(tmp));
    usleep(300);
  }

  sleep(10);
  // 4.关闭文件描述符
  closeSocket(fd);

  return 0;
}
