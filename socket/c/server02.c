/*
 * @Autor        : MrX-OvO
 * @Date         : 2021-07-13 15:40:50
 * @LastEditTime : 2021-07-13 21:52:30
 * @LastEditors  : MrX-OvO
 * @Description  : socket通信 服务器端 多线程
 * @FilePath     : /socket/server02.c
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

// 设置最大通信量
#define CLIENT_NUMS 512

// 作为woring线程处理函数的传入参数，信息结构体
struct SockInfo
{
    struct sockaddr_in addr;
    int fd;
};

struct SockInfo infos[CLIENT_NUMS];

// 线程通信处理函数
void *working(void *arg);

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

    // 初始化结构体数组
    int max = sizeof(infos) / sizeof(infos[0]);
    for (size_t i = 0; i < max; ++i)
    {
        bzero(&infos[i], sizeof(infos[i]));
        infos[i].fd = -1;
    }

    // 4.阻塞并等待客户端的连接
    int addrlen = sizeof(struct sockaddr_in);
    while (1)
    {
        struct SockInfo *pinfo;
        for (size_t i = 0; i < max; ++i)
        {
            if (infos[i].fd == -1)
            {
                pinfo = &infos[i];
                break;
            }
        }

        int cfd = accept(fd, (struct sockaddr *)&pinfo->addr, &addrlen); // 如果客户端不发送新的连接，服务器端阻塞在accept
        pinfo->fd = cfd;
        if (cfd == -1)
        {
            perror("accept");
            break;
        }

        // 创建子线程进行与客户端通信
        pthread_t tid;
        pthread_create(&tid, NULL, working, pinfo);
        // 不能调用pthread_join，因为该函数会阻塞主线程，直到调用该函数的子线程退出
        // 如果阻塞了主线程，则不能进行多个连接
        pthread_detach(tid);
    }

    // 6.关闭文件描述符
    close(fd);

    return 0;
}

void *working(void *arg)
{
    // 将传入参数强制类型转换
    struct SockInfo *pinfo = (struct SockInfo *)arg;

    // 连接建立成功，打印客户端的IP和端口信息
    char ip[32];
    printf("client IP: %s, port: %d\n", inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip)), ntohs(pinfo->addr.sin_port));

    // 5.与客户端进行通信
    while (1)
    {
        // 接收数据
        char buff[1024];
        int len = recv(pinfo->fd, buff, sizeof(buff), 0); // 如果客户端不给服务器发送数据，服务器端阻塞在recv
        if (len > 0)
        {
            printf("client send: %s\n", buff);
            send(pinfo->fd, buff, len, 0); // 如果写缓冲区buff满了，服务器端阻塞在send
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
    close(pinfo->fd);
    pinfo->fd = -1;

    return NULL;
}