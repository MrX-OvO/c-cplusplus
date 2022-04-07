#include "socket.h"
#include "myheader.h"
// 服务端
// 绑定+监听
int setBindAndListen(int lfd, const char *ip, unsigned short port) {
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY; // 0 => 0.0.0.0
  saddr.sin_port = htons(port);

  // 绑定
  int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
  if (ret == -1) {
    perror("bind");
    return -1;
  }
  printf("bind successed, lfd = %d\n", lfd);

  // 监听
  ret = listen(lfd, 128);
  if (ret == -1) {
    perror("listen");
    return -1;
  }
  printf("listen successed, lfd = %d\n", lfd);

  return ret;
}

// 阻塞并等待客户端连接
int acceptConn(int lfd, struct sockaddr_in *addr) {
  int cfd = -1;
  if (addr == NULL)
    cfd = accept(lfd, NULL, NULL);
  else {
    socklen_t len = sizeof(struct sockaddr_in);
    cfd = accept(lfd, (struct sockaddr *)addr, &len);
  }
  if (cfd == -1) {
    perror("accept");
    return -1;
  }
  printf("connect to client successed, lfd = %d cfd = %d\n", lfd, cfd);

  return cfd;
}

// 客户端
// 连接服务器
int connToHost(int fd, const char *ip, unsigned short port) {
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &saddr.sin_addr.s_addr);
  saddr.sin_port = htons(port);
  int ret = connect(fd, (struct sockaddr *)&saddr, sizeof(saddr));
  if (ret == -1) {
    perror("connect");
    return -1;
  }
  printf("connect to server successed, cfd = %d\n", fd);

  return ret;
}

// 共同函数
// 创建通信套接字
int createSocket() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket");
    return -1;
  }
  printf("create socket successed, fd = %d\n", fd);

  return fd;
}

// 发送指定长度的数据
int writeN(int fd, const char *msg, int size) {
  const char *buf = msg;
  int count = size;
  while (count > 0) {
    int len = send(fd, buf, count, 0);
    if (len == -1) {
      perror("send");
      return -1;
    } else if (len == 0)
      continue;
    buf += len;
    count -= len;
  }

  return size;
}

// 发送数据
int sendMsg(int fd, const char *msg, int size) {
  if (fd < 0 || NULL == msg || size <= 0)
    return -1;
  // 动态申请一块内存，用于存储打包了的数据（大小+数据）
  char *data = (char *)malloc(size + 4);
  int bigLen = htonl(size);
  memcpy(data, &bigLen, 4);
  memcpy(data + 4, msg, size);
  int ret = writeN(fd, data, size + 4);
  if (ret == -1)
    closeSocket(fd);
  printf("\nsize of the data block: %dB", ret - 4);
  free(data);
  data = NULL;

  return ret;
}

// 接收指定长度的数据
int readN(int fd, char *msg, int size) {
  char *buf = msg;
  int count = size;
  while (count > 0) {
    int len = recv(fd, buf, count, 0);
    if (len == -1) {
      perror("recv");
      return -1;
    } else if (len == 0)
      return size - count;
    buf += len;
    count -= len;
  }

  return size;
}

// 接收数据
int recieceMsg(int fd, char **msg) {
  // 先读取包头，获得数据的长度
  int len = 0;
  readN(fd, (char *)&len, 4);
  len = ntohl(len);
  printf("\nsize of the data block: %dB", len);

  // 动态申请一块内存，用于存储要读取的数据块
  char *data = (char *)malloc(len + 1);
  int ret = readN(fd, data, len);
  if (ret != len) {
    printf("\nrecieve data failed...\n");
    closeSocket(fd);
    free(data);
    data = NULL;
    return -1;
  }
  data[len] = '\0';
  *msg = data;

  return ret;
}

// 关闭通信套接字
int closeSocket(int fd) {
  int ret = close(fd);
  if (ret == -1)
    perror("close");

  return ret;
}
