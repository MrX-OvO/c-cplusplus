/*
 * @Autor        : MrX-OvO
 * @Date         : 2021-07-13 16:25:29
 * @LastEditTime : 2021-07-13 21:47:26
 * @LastEditors  : MrX-OvO
 * @Description  : socket通信 客户端
 * @FilePath     : /socket/client01.c
 * Copyright 2021 MrX-OvO, All Rithts Rerserved
 * 
 To: One is never too old to learn...
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    // 1.创建通信套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket");
        return -1;
    }

    // 2.连接服务器的IP port
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    inet_pton(AF_INET, "172.17.36.82", &saddr.sin_addr.s_addr);
    int ret = connect(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1)
    {
        perror("connect");
        return -1;
    }

    int number = 0;
    // 3.与服务器端进行通信
    while (1)
    {
        // 发送数据
        char buff[1024];
        sprintf(buff, "hello, world, %d...\n", number++);
        send(fd, buff, strlen(buff) + 1, 0);

        // 接收数据
        memset(buff, 0, sizeof(buff));
        int len = recv(fd, buff, sizeof(buff), 0);
        if (len > 0)
        {
            printf("server send: %s\n", buff);
        }
        else if (len == 0)
        {
            printf("server has disconnected...\n");
            break;
        }
        else
        {
            perror("recv");
            break;
        }
        sleep(1);
    }

    // 4.关闭文件描述符
    close(fd);

    return 0;
}