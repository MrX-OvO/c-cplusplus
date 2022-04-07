#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <arpa/inet.h>

// 服务端
// 绑定+监听
int setBindAndListen(int lfd, const char *ip, unsigned short port);

// 阻塞并等待客户端连接
int acceptConn(int lfd, struct sockaddr_in *addr);

// 客户端
// 连接服务器
int connToHost(int fd, const char *ip, unsigned short port);

// 共同函数
// 创建通信套接字
int createSocket();

// 发送指定长度的数据
int writeN(int fd, const char *msg, int size);

// 发送数据
int sendMsg(int fd, const char *msg, int size);

// 接收指定长度的数据
int readN(int fd, char *msg, int size);

// 接收数据
int recieceMsg(int fd, char **ms);

// 关闭通信套接字
int closeSocket(int fd);

#endif
