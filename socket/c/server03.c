/*
 * @Autor        : MrX-OvO
 * @Date         : 2021-07-13 15:40:50
 * @LastEditTime : 2021-07-13 22:57:28
 * @LastEditors  : MrX-OvO
 * @Description  : socket通信 服务器端 线程池
 * @FilePath     : /socket/server03.c
 * Copyright 2021 MrX-OvO, All Rithts Rerserved
 * 
 To: One is never too old to learn...
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#include "thread_pool.h"

// 作为woring线程处理函数的传入参数，信息结构体，套接字socket和文件描述符fd复合结构
typedef struct SockInfo
{
    struct sockaddr_in addr;
    int fd;
} SockInfo;

// 作为acceptCoon线程处理函数的传入参数，信息结构体，线程池pool和文件描述符fd复合结构
typedef struct PoolInfo
{
    ThreadPool *pool;
    int fd;
} PoolInfo;

// 线程建立连接函数
void acceptConn(void *arg);

// 线程通信处理函数
void working(void *arg);

int main()
{
    // 1.创建监听套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket");
        return -1;
    }

    // 2.绑定本地的IP port
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY; // 0 => 0.0.0.0 会绑定本机IP
    int ret = bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1)
    {
        perror("bind");
        return -1;
    }

    // 3.设置监听
    ret = listen(fd, 128);
    if (ret == -1)
    {
        perror("listen");
        return -1;
    }

    // 创建线程池
    int qCanpacity = 100, min = 3, max = 10;
    ThreadPool *pool = createThreadPool(qCanpacity, min, max);
    PoolInfo *pp_info = (PoolInfo *)malloc(sizeof(PoolInfo));
    pp_info->pool = pool;
    pp_info->fd = fd;
    sleep(1);
    addTask(pool, acceptConn, pp_info);

    // 主线程退出，剩下任务由子线程完成
    pthread_exit(NULL);

    return 0;
}

void acceptConn(void *arg)
{
    // 将传入参数强制类型转换
    PoolInfo *pp_info = (PoolInfo *)arg;

    // 4.阻塞并等待客户端的连接
    int addrlen = sizeof(struct sockaddr_in);
    while (1)
    {
        SockInfo *ps_info;
        ps_info = (SockInfo *)malloc(sizeof(SockInfo));
        ps_info->fd = accept(pp_info->fd, (struct sockaddr *)&ps_info->addr, &addrlen); // 如果客户端不发送新的连接，服务器端阻塞在accept
        if (ps_info->fd == -1)
        {
            perror("accept");
            break;
        }

        // 添加通信任务
        addTask(pp_info->pool, working, ps_info);
    }

    // 6.关闭文件描述符
    close(pp_info->fd);
}

void working(void *arg)
{
    // 将传入参数强制类型转换
    SockInfo *ps_info = (SockInfo *)arg;

    // 连接建立成功，打印客户端的IP和端口信息
    char ip[32];
    printf("client IP: %s, port: %d\n", inet_ntop(AF_INET, &ps_info->addr.sin_addr.s_addr, ip, sizeof(ip)), ntohs(ps_info->addr.sin_port));

    // 5.与客户端进行通信
    while (1)
    {
        // 接收数据
        char buff[1024];
        int len = recv(ps_info->fd, buff, sizeof(buff), 0); // 如果客户端不给服务器发送数据，服务器端阻塞在recv
        if (len > 0)
        {
            printf("client send: %s\n", buff);
            send(ps_info->fd, buff, len, 0); // 如果写缓冲区buff满了，服务器端阻塞在send
        }
        else if (len == 0)
        {
            printf("client has disconnected...\n");
            break;
        }
        else
        {
            perror("recv");
            break;
        }
    }

    // 6.关闭文件描述符
    close(ps_info->fd);
}