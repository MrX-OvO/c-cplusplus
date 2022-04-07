#pragma once

#include <pthread.h>

// 管理者线程一次性添加工作者线程个数
#define NUMBER 2

// 任务结构体
typedef struct Task
{
    void (*function)(void *arg);
    void *arg;
} Task;

// 线程池结构体
typedef struct ThreadPool
{
    // 任务队列
    Task *taskQ;
    // 任务队列容量
    int queueCapacity;
    // 当前任务数
    int queueSize;
    // 队头 取数据
    int queueFront;
    // 队尾 放数据
    int queueRear;

    // 管理者线程ID
    pthread_t managerID;
    // 工作者线程ID
    pthread_t *workerIDs;
    // 工作者线程数量范围
    int minNum;
    int maxNum;
    // 记录正在工作的工作者线程数
    int busyNum;
    // 记录当前存活的工作者线程数
    int aliveNum;
    // 记录要杀死的工作者线程数
    int killNum;
    // 互斥锁，访问线程池共享资源时，对线程池上锁
    pthread_mutex_t mutexPool;
    // 互斥锁，对忙工作者线程数量（busyNum）上锁
    pthread_mutex_t mutexBusy;
    // 条件变量，任务队列是否为满、为空
    pthread_cond_t notFull;
    pthread_cond_t notEmpty;

    // 线程池关闭标志，是否关闭线程池，1：是，0：否
    int isShutdown;
} ThreadPool;

// 创建线程池并初始化
ThreadPool *createThreadPool(int qCanpacity, int min, int max);

// 销毁线程池
int destroyThreadPool(ThreadPool *pool);

// 往线程池添加任务
void addTask(ThreadPool *pool, void (*func)(void *), void *arg);

// 获取当前线程池正在工作的线程数
int getBusyNum(ThreadPool *pool);

// 获取当前线程池存活的线程数
int getAliveNum(ThreadPool *pool);

// 管理者线程任务函数
void *manager(void *arg);

// 工作的线程(消费者线程)任务函数
void *worker(void *arg);

// 单个线程退出
void exitThread(ThreadPool *pool);
