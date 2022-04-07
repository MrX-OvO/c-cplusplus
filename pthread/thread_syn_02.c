#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// 定义数数次数 50次
#define MAX 50

// 定义读写锁rwlock
pthread_rwlock_t rwlock;

// 定义全局显示的number
int number=0;

// 写线程处理函数
void* writeNum(void* arg)
{
    for(int i=0; i<MAX; ++i)
    {
        pthread_rwlock_wrlock(&rwlock);
        int cur=number;
        ++cur;
        number=cur;
        printf("写操作完毕，number=%d, wreite_tid=%ld\n", number, pthread_self());
        pthread_rwlock_unlock(&rwlock);
        usleep(rand()%100);
    }

    return NULL;
}

// 读线程处理函数
void* readNum(void* arg)
{
    for(int i=0; i<MAX; ++i)
    {
        pthread_rwlock_rdlock(&rwlock);
        printf("读操作完毕，number=%d, read_tid=%ld\n", number, pthread_self());
        pthread_rwlock_unlock(&rwlock);
        usleep(rand()%100);
    }

    return NULL;
}

int main()
{
    // 初始化读写锁
    pthread_rwlock_init(&rwlock, NULL);
    
    // 创建3个写线程，5个读线程
    pthread_t write[3], read[5];
    for(int i=0; i<3; ++i)
    {
        pthread_create(&write[i], NULL, writeNum, NULL);
    }
    for(int i=0; i<5; ++i)
    {
        pthread_create(&read[i], NULL, readNum, NULL);
    }
    
    // 阻塞，资源回收
    for(int i=0; i<3; ++i)
    {
        pthread_join(write[i],NULL);
    }
    for(int i=0; i<5; ++i)
    {
        pthread_join(read[i],NULL);
    }

    // 销毁读写锁
    // 在线程销毁后再释放读写锁
    pthread_rwlock_destroy(&rwlock);
    
    return 0;
}

