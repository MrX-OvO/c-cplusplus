#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
  // 1.创建监听文件描述符
  int lfd = socket(AF_INET, SOCK_STREAM, 0);
  if (lfd == -1) {
    perror("socker");
    return -1;
  }

  // 2.绑定
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(9999);
  addr.sin_addr.s_addr = INADDR_ANY;
  int ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret == -1) {
    perror("bind");
    return -1;
  }

  // 设置监听
  ret = listen(lfd, 128);
  if (ret == -1) {
    perror("bind");
    return -1;
  }

  // 将监听的lfd的状态检测委托给内核检测
  int maxfd = lfd;

  // 初始化检测的读集合
  fd_set rdset;
  fd_set rdtmp;

  // 清零
  FD_ZERO(&rdset);

  // 将监听的lfd设置到检测的读集合中
  FD_SET(lfd, &rdset);

  // 通过select委托内核检测读集合中的文件描述符状态，检测read缓冲区有没有数据
  // 如果有数据，select解除阻塞返回
  // 应该让内核持续检测
  while (1) {
    // 默认阻塞
    // rdset中是委托内核检测的所以文件描述符
    rdtmp = rdset;
    int num = select(maxfd + 1, &rdtmp, NULL, NULL, NULL);

    // rdset中的数据被内核改写了，只保留了发生变化的文件描述符的标志位上的1，没变化的为0
    // 只要rdset中fd对应的标志位为1=>缓冲区有数据了
    // 判断有没有新连接
    if (FD_ISSET(lfd, &rdtmp)) {
      // 接受连接请求，这个调用accept不阻塞
      struct sockaddr_in caddr;
      socklen_t cLen = sizeof(caddr);
      int cfd = accept(lfd, (struct sockaddr *)&caddr, &cLen);
      if (cfd == -1) {
        perror("accept");
        return -1;
      }
      // 打印客户端IP和端口号
      char ip[32];
      printf("client IP: %s, port: %d\n",
             inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip)),
             ntohs(caddr.sin_port));

      // 得到有效文件描述符
      // 通信的文件描述符添加到读集合
      // 在下一轮select检测的时候就能得到缓冲区的状态
      FD_SET(cfd, &rdset);

      // 重置最大的文件描述符
      maxfd = cfd > maxfd ? cfd : maxfd;
    }

    // 没有新连接，通信
    for (int i = 0; i <= maxfd; ++i) {
      // 判断非监听文件描述符是否读缓冲区有数据
      if (i != lfd && FD_ISSET(i, &rdtmp)) {
        // 接收数据
        char buff[1024] = {0};
        int len = recv(i, buff, sizeof(buff), 0);
        if (len == -1) {
          perror("recv");
          return -1;
        } else if (len == 0) {
          // 文件描述符对应标志位置为0
          FD_CLR(i, &rdset);

          // 关闭文件描述符
          close(i);

          printf("client has disconnected to server!!!\n");
          break;
        }
        printf("clent send: %s\n", buff);

        // 将客户端发送来的数据小写转大写，然后再发送回去
        for (int i = 0; i < len; ++i) {
          buff[i] = toupper(buff[i]);
        }
        printf("after toupper, buff: %s\n", buff);
        ret = send(i, buff, len + 1, 0);
        if (ret == -1) {
          perror("send");
          return -1;
        }
      }
    }
  }

  // 关闭监听文件描述符
  close(lfd);

  return 0;
}
