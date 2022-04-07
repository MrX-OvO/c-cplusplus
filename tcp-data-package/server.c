#include "myheader.h"
#include "socket.h"

// 设置最大通信量
#define CLIEN_NUMS 512

// 作为working线程处理函数的传入参数，信息结构体
typedef struct SocketInfo {
  int fd;
  struct sockaddr_in addr;
} SocketInfo;

SocketInfo infos[CLIEN_NUMS];

// 线程通信处理函数
void *working(void *arg);

int main() {
  // 1.创建监听套接字
  int lfd = createSocket();
  if (lfd == -1)
    return -1;

  // 2.绑定并监听
  const char *ip = "172.17.36.82";
  unsigned short port = 9999;
  int ret = setBindAndListen(lfd, ip, port);
  if (ret == -1)
    return -1;

  // 初始化结构体数组
  int max = sizeof(infos) / sizeof(infos[0]);
  printf("max connections: %d\n", max);
  for (int i = 0; i < max; ++i) {
    bzero(&infos[i], sizeof(infos[i]));
    infos[i].fd = -1;
  }

  // 3.阻塞等待客户端连接
  while (1) {
    SocketInfo *pinfo;
    for (int i = 0; i < max; ++i) {
      if (infos[i].fd == -1) {
        pinfo = &infos[i];
        break;
      }
    }

    int cfd = acceptConn(lfd, &pinfo->addr);
    if (cfd == -1)
      continue;
    pinfo->fd = cfd;

    // 创建子线程与客户端通信
    pthread_t tid;
    pthread_create(&tid, NULL, working, pinfo);
    pthread_detach(tid);
  }

  // 4.关闭文件描述符
  closeSocket(lfd);

  return 0;
}

void *working(void *arg) {
  SocketInfo *pinfo = (SocketInfo *)arg;

  // 打印客户端IP和端口
  char ip[15];
  inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip));
  unsigned short port = ntohs(pinfo->addr.sin_port);
  printf("client IP: %s, port: %d\n", ip, port);

  while (1) {
    // 接收数据
    char *buf;
    int len = recieceMsg(pinfo->fd, &buf);
    if (len > 0) {
      printf("\n=+=+= revieve data from client +=+=+\n");
      printf("%s\n", buf);
      free(buf);
      buf = NULL;
    } else
      break;
    sleep(1);
  }
  pinfo->fd = -1;

  return NULL;
}
