#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket");
    return -1;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(9998);
  addr.sin_addr.s_addr = INADDR_ANY;
  int ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret == -1) {
    perror("bind");
    return -1;
  }

  ret = listen(fd, 128);
  if (ret == -1) {
    perror("listen");
    return -1;
  }

  struct pollfd fds[1024];
  for (int i = 0; i < 1024; ++i) {
    fds[i].fd = -1;
    fds[i].events = POLLIN | POLLOUT;
  }
  // 把监听套接字文件描述符放在第一个pollfd中
  fds[0].fd = fd;

  int maxfd = 0;
  while (1) {
    // 委托内核检测
    ret = poll(fds, maxfd + 1, -1);
    if (ret == -1) {
      perror("poll");
      return -1;
    }

    // 有新连接
    if (fds[0].revents & POLLIN) {
      struct sockaddr_in caddr;
      socklen_t len = sizeof(caddr);
      // 下面的accept不会阻塞
      int connfd = accept(fd, (struct sockaddr *)&caddr, &len);
      // 委托内核检测connfd的读缓冲区
      int i;
      for (i = 0; i < 1024; ++i) {
        if (fds[i].fd == -1) {
          fds[i].fd = connfd;
          break;
        }
      }
      maxfd = i > maxfd ? i : maxfd;
    }

    int num = 0;

    // 通信
    for (int i = 1; i <= maxfd; ++i) {
      char buff[1024];
      if (fds[i].revents & POLLOUT) {
        sprintf(buff, "hello client, number = %d\n", num++);
        for (int i = 0; i < strlen(buff); ++i) {
          buff[i] = toupper(buff[i]);
        }
        send(fds[i].fd, buff, strlen(buff) + 1, 0);
      }
      if (fds[i].revents & POLLIN) {
        int len = recv(fds[i].fd, buff, sizeof(buff), 0);
        if (len == -1) {
          perror("recv");
          break;
        } else if (len == 0) {
          printf("client has disconnected to server!!!\n");
          close(fds[i].fd);
          fds[i].fd = -1;
          break;
        }
        printf("client send: %s\n", buff);
      }
    }
  }

  return 0;
}
