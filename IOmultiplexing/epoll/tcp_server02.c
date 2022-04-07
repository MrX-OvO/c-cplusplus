#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  // 创建监听文件描述符
  int lfd = socket(AF_INET, SOCK_STREAM, 0);
  if (lfd == -1) {
    perror("socker");
    return -1;
  }

  // 绑定
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

  // 设置端口复用
  int optval = 1;
  setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  // 设置监听
  ret = listen(lfd, 128);
  if (ret == -1) {
    perror("listen");
    return -1;
  }

  // 创建epoll实例
  int epfd = epoll_create(1);
  if (epfd == -1) {
    perror("epoll_create");
    return -1;
  }

  // 监听的文件描述符的事件
  struct epoll_event ev;
  ev.events = EPOLLIN | EPOLLET; // 设置边缘沿触发（ET）模式
  ev.data.fd = lfd;

  // 将监听的文件描述符放在epoll实例上
  int epctl = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
  if (epctl == -1) {
    perror("epoll_ctl");
    return -1;
  }

  struct epoll_event evs[1024];
  int size = sizeof(evs) / sizeof(evs[0]);
  // 进行检测
  while (1) {
    // -1，一直阻塞，直到epfd对应depoll树有文件描述符就绪了
    // 当返回了就绪文件描述符个数时，对应的事件会写入evs中
    int num = epoll_wait(epfd, evs, size, -1);
    printf("就绪文件描述符个数：%d\n", num);
    // 遍历evs
    for (int i = 0; i < num; ++i) {
      int curfd = evs[i].data.fd;
      // 建立连接
      if (curfd == lfd) {
        struct sockaddr_in caddr;
        socklen_t cLen = sizeof(caddr);
        int cfd = accept(curfd, (struct sockaddr *)&caddr, &cLen);
        if (cfd == -1) {
          perror("accept");
          return -1;
        }

        // epoll在边沿模式下，必须要将套接字设置为非阻塞模式
        // 将文件描述符修改为非阻塞
        int flag = fcntl(cfd, F_GETFL);
        flag |= O_NONBLOCK;
        fcntl(cfd, F_SETFL, flag);

        // 将通信的文件描述符cfd放入epoll树
        ev.events = EPOLLIN | EPOLLET; // 设置边缘沿触发（ET）模式
        ev.data.fd = cfd;
        int epctl = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
        if (epctl == -1) {
          perror("epoll_ctl");
          return -1;
        }
        char ip[16];
        printf("client IP:%s, port:%d\n",
               inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip)),
               ntohs(caddr.sin_port));
      }
      // 通信文件描述符
      else {
        // 接收数据
        // 1.使用大内存存储数据
        // 不推荐=>1.内存大小没办法界定 2.系统能够分配的最大内存（堆、栈内存）有限
        // 2.使用小内存循环读取读缓冲区数据
        char buff[5] = {0};
        char tmp[1024];
        bzero(tmp, sizeof(tmp));
        while (1) {
          int len = recv(curfd, buff, sizeof(buff), 0);
          if (len == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
              printf("recieved data completely...\n");
              // 发送数据
              ret = send(curfd, tmp, strlen(tmp) + 1, 0);
              if (ret == -1) {
                perror("send");
                return -1;
              }
              break;
            }
            perror("recv");
            return -1;
          } else if (len == 0) {
            // 从epoll树中删除fd，删除fd不需要指定事件
            int epctl = epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
            if (epctl == -1) {
              perror("epoll_ctl");
              return -1;
            }

            // 先执行epctl再执行close，否则epctl返回-1
            // 关闭文件描述符
            close(curfd);
            printf("client has disconnected to server!!!\n");
            break;
          }
          // printf("clent send: %s\n", buff);
          write(STDOUT_FILENO, buff, len);

          // 将客户端发送来的数据小写转大写，然后再发送回去
          for (int i = 0; i < len; ++i) {
            buff[i] = toupper(buff[i]);
          }
          strncat(tmp + strlen(tmp), buff, len);
          printf("\nafter toupper: %s\n", tmp);
        }
      }
    }
  }

  // 关闭监听文件描述符
  close(lfd);

  return 0;
}
