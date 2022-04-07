/*
 * @Autor        : MrX-OvO
 * @Date         : 2021-07-13 15:40:50
 * @LastEditTime : 2021-07-13 21:22:24
 * @LastEditors  : MrX-OvO
 * @Description  : socket通信 服务器端 单线程
 * @FilePath     : /socket/server01.c
 * Copyright 2021 MrX-OvO, All Rithts Rerserved
 * 
 To: One is never too old to learn...
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

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

    // 4.阻塞并等待客户端的连接
    struct sockaddr_in caddr;
    int addrlen = sizeof(caddr);
    int cfd = accept(fd, (struct sockaddr *)&caddr, &addrlen); // 如果客户端不发送新的连接，服务器端阻塞在accept
    if (cfd == -1)
    {
        perror("accept");
        return -1;
    }

    // 连接建立成功，打印客户端的IP和端口信息
    char ip[32];
    printf("client IP: %s, port: %d\n", inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip)), ntohs(caddr.sin_port));

    // 5.与客户端进行通信
    while (1)
    {
        // 接收数据
        char buff[1024];
        int len = recv(cfd, buff, sizeof(buff), 0); // 如果客户端不给服务器发送数据，服务器端阻塞在recv
        if (len > 0)
        {
            printf("client send: %s\n", buff);
            send(cfd, buff, len, 0); // 如果写缓冲区buff满了，服务器端阻塞在send
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
    close(fd);
    close(cfd);

    return 0;
}