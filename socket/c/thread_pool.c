/*
 * @Autor        : MrX-OvO
 * @Date         : 2021-07-13 21:46:08
 * @LastEditTime : 2021-07-13 22:56:26
 * @LastEditors  : MrX-OvO
 * @Description  : c语言版 线程池
 * @FilePath     : /socket/thread_pool.c
 * Copyright 2021 MrX-OvO, All Rithts Rerserved
 * 
 To: One is never too old to learn...
 */
#include "thread_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

// 创建并初始化线程池
ThreadPool *createThreadPool(int qCanpacity, int min, int max)
{
    // 创建线程池
    // malloc返回void *，故要将其转换为ThreadPool *
    ThreadPool *pool = (ThreadPool *)malloc(sizeof(ThreadPool));

    // do-while(0) + break代替break，可以减少free操作
    do
    {
        if (NULL == pool)
        {
            printf("malloc pool failed...\n");
            break;
        }
        printf("malloc pool successed...\n");

        // 创建并初始化任务队列
        pool->taskQ = (Task *)malloc(sizeof(Task) * qCanpacity);
        if (NULL == pool->taskQ)
        {
            printf("malloc task queue failed...\n");
            break;
        }
        printf("malloc task queue successed...\n");
        pool->queueCapacity = qCanpacity;
        pool->queueSize = 0;
        pool->queueFront = 0;
        pool->queueRear = 0;

        // 初始化有关工作者线程数量的变量
        pool->minNum = min;
        pool->maxNum = max;
        pool->busyNum = 0;
        pool->aliveNum = min;
        pool->killNum = 0;

        // 创建并初始化工作者线程ID
        pool->workerIDs = (pthread_t *)malloc(sizeof(pthread_t) * max);
        if (NULL == pool->workerIDs)
        {
            printf("malloc workerIDs failed...\n");
            break;
        }
        printf("malloc workerIDs successed...\n");
        // 先将所有的（max 个）工作者线程ID初始化为0
        memset(pool->workerIDs, 0, sizeof(pthread_t) * max);

        // 创建管理者线程（1个）
        pthread_create(&pool->managerID, NULL, manager, pool);

        // 创建工作者线程（min 个）
        for (int i = 0; i < min; ++i)
        {
            pthread_create(&pool->workerIDs[i], NULL, worker, pool);
        }

        // 判断初始化互斥锁和条件变量是否成功（初始值为0
        if (pthread_mutex_init(&pool->mutexPool, NULL) != 0 || pthread_mutex_init(&pool->mutexBusy, NULL) != 0 || pthread_cond_init(&pool->notFull, NULL) != 0 || pthread_cond_init(&pool->notEmpty, NULL) != 0)
        {
            printf("mutex or condition init failed...\n");
            break;
        }

        // 初始化线程池关闭标志
        pool->isShutdown = 0;

        return pool;

    } while (0);

    // 释放资源
    if (pool && pool->taskQ)
    {
        free(pool->taskQ);
        pool->taskQ = NULL;
    }
    if (pool && pool->workerIDs)
    {
        free(pool->workerIDs);
        pool->workerIDs = NULL;
    }
    if (pool)
    {
        free(pool);
        pool = NULL;
    }

    return NULL;
}

// 销毁线程池
int destroyThreadPool(ThreadPool *pool)
{
    // pool为空，不用销毁线程池，直接返回-1
    if (NULL == pool)
        return -1;

    // 关闭线程池
    pool->isShutdown = 1;

    // 阻塞回收管理者线程
    pthread_join(pool->managerID, NULL);

    // 唤醒被条件变量notEmpty阻塞的工作者线程
    for (int i = 0; i < pool->aliveNum; ++i)
    {
        pthread_cond_broadcast(&pool->notEmpty);
    }

    // 释放互斥锁和条件变量
    pthread_mutex_destroy(&pool->mutexPool);
    pthread_mutex_destroy(&pool->mutexBusy);
    pthread_cond_destroy(&pool->notFull);
    pthread_cond_destroy(&pool->notEmpty);

    // 释放堆内存
    if (pool->taskQ)
    {
        free(pool->taskQ);
        pool->taskQ = NULL;
    }
    if (pool->workerIDs)
    {
        free(pool->workerIDs);
        pool->workerIDs = NULL;
    }
    free(pool);
    pool = NULL;

    return 0;
}

// 往线程池添加任务
void addTask(ThreadPool *pool, void (*func)(void *), void *arg)
{
    // 访问共享资源前，先加锁
    pthread_mutex_lock(&pool->mutexPool);

    // 当前任务队列是否为满，且线程池是否关闭
    while (pool->queueSize == pool->queueCapacity && !pool->isShutdown)
    {
        // 阻塞工作者线程
        printf("worker thread %ld blocked by full...\n", pthread_self());
        pthread_cond_wait(&pool->notFull, &pool->mutexPool);
        printf("worker thread %ld waked up by notFull, can add task, queueSize:%d <= qCanpacity:%d\n", pthread_self(), pool->queueSize, pool->queueCapacity);
    }

    // 判断线程池是否关闭
    if (pool->isShutdown)
    {
        // 先解锁，防止出现死锁
        pthread_mutex_unlock(&pool->mutexPool);
        return;
    }

    //添加任务
    pool->taskQ[pool->queueRear].function = func;
    pool->taskQ[pool->queueRear].arg = arg;

    // 移动尾结点，形成循环队列
    pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;

    // 当前任务数加1
    ++pool->queueSize;
    printf("added one task...\n");

    // 因为往任务队列中添加了一个任务，故任务队列不空，唤醒阻塞在条件变量notEmpty的工作者线程
    pthread_cond_broadcast(&pool->notEmpty);

    // 退出临界区前先解锁
    pthread_mutex_unlock(&pool->mutexPool);
}

// 获取当前线程池正在工作的线程数
int getBusyNum(ThreadPool *pool)
{
    // 访问共享资源busyNum，加锁
    pthread_mutex_lock(&pool->mutexBusy);
    int busyNum = pool->busyNum;
    // 退出临界区前先解锁
    pthread_mutex_unlock(&pool->mutexBusy);

    return busyNum;
}

// 获取当前线程池存活的线程数
int getAliveNum(ThreadPool *pool)
{
    // 访问共享资源前，先加锁
    pthread_mutex_lock(&pool->mutexPool);
    int aliveNum = pool->aliveNum;
    // 退出临界区，先解锁
    pthread_mutex_unlock(&pool->mutexPool);

    return aliveNum;
}

// 管理者线程任务函数
void *manager(void *arg)
{
    printf("create manager thread %ld successed...\n", pthread_self());

    // 将出入参数强制转换为ThreadPool * 类型
    ThreadPool *pool = (ThreadPool *)arg;

    // 当线程池未销毁时管理者线程才运行
    while (!pool->isShutdown)
    {
        // 每隔1s检测一次
        sleep(1);

        // 访问共享资源前，先加锁
        pthread_mutex_lock(&pool->mutexPool);

        // 获取当前线程池中任务的数量和当前线程的数量
        int queueSize = pool->queueSize;
        int aliveNum = pool->aliveNum;

        // 退出临界区前，先解锁
        pthread_mutex_unlock(&pool->mutexPool);

        // 访问共享资源busyNum，加锁
        pthread_mutex_lock(&pool->mutexBusy);

        // 获取当前线程池中正在工作的工作者线程
        int busyNum = pool->busyNum;

        // 退出临界区前，先解锁
        pthread_mutex_unlock(&pool->mutexBusy);

        // 添加工作者线程
        // 规则：
        // 任务的个数>存活的线程个数-忙的线程个数 && 存活的线程数<最大线程数
        if (queueSize > aliveNum - busyNum && aliveNum < pool->maxNum)
        {
            // 访问共享资源前，先加锁
            pthread_mutex_lock(&pool->mutexPool);

            // 打印输出信息
            printf("add task(rule: queueSize > aliveNum && aliveNum < pool->maxNum), pool => queueSize:%d, aliveNum:%d, maxNum:%d\n", pool->queueSize, pool->aliveNum, pool->maxNum);

            // 控制添加工作者线程个数，最多NUMBER（2）个
            int counter = 0;
            for (int i = 0; i < pool->maxNum && counter < NUMBER && pool->aliveNum < pool->maxNum; ++i)
            {
                // 工作者线程ID为0，则当前位置没有存储线程ID
                if (pool->workerIDs[i] == 0)
                {
                    pthread_create(&pool->workerIDs[i], NULL, worker, pool);
                    printf("create worker thread %ld successed... queueSize:%d, busyNum:%d, aliveNum:%d, maxNum:%d\n", pthread_self(), pool->queueSize, pool->busyNum, pool->aliveNum, pool->maxNum);
                    ++counter;
                    ++pool->aliveNum;
                }
            }

            // 退出临界区前，先解锁
            pthread_mutex_unlock(&pool->mutexPool);
        }

        // 销毁工作者线程
        // 规则：
        // 忙的线程*2 < 存活的线程数 && 存活的线程>最小线程数
        if (busyNum * 2 < aliveNum && aliveNum > pool->minNum)
        {
            // 访问共享资源前，先加锁
            pthread_mutex_lock(&pool->mutexPool);

            // 打印输出信息
            printf("kill thread(rule: busyNum * 2 < aliveNum && aliveNum > pool->minNum), pool => busyNum:%d, aliveNum:%d, maxNum:%d\n", pool->busyNum, pool->aliveNum, pool->maxNum);

            pool->killNum = NUMBER;

            // 退出临界区前，先解锁
            pthread_mutex_unlock(&pool->mutexPool);

            // 让工作者线程线程自杀
            for (int i = 0; i < NUMBER; ++i)
            {
                // 唤醒被条件变量notEmpty阻塞的工作者线程
                pthread_cond_broadcast(&pool->notEmpty);
            }
        }
    }

    return NULL;
}

// 工作的线程(消费者线程)任务函数
void *worker(void *arg)
{
    // 将出入参数强制转换为ThreadPool * 类型
    ThreadPool *pool = (ThreadPool *)arg;

    // 一直检测是否有任务可以执行
    while (1)
    {
        // 访问共享资源前，先加锁
        pthread_mutex_lock(&pool->mutexPool);

        // 当前任务队列是否为空，且线程池是否关闭
        while (pool->queueSize == 0 && !pool->isShutdown)
        {
            // 阻塞工作者线程
            printf("worker thread %ld blocked by empty...\n", pthread_self());
            pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);
            printf("worker thread %ld waked up by notEmpty, can work, queueSize:%d > 0\n", pthread_self(), pool->queueSize);

            // 因为管理者线程中的销毁线程功能唤醒了被条件变量notEmpty阻塞的工作者线程
            // 所以被唤醒线程从之前被阻塞处继续向下执行
            // 判断是不是要销毁线程
            if (pool->killNum > 0)
            {
                --pool->killNum;

                // 当存活的工作者线程数>工作者线程输的下限时，才能退出
                if (pool->aliveNum > pool->minNum)
                {
                    --pool->aliveNum;

                    // 解锁，防止出现死锁
                    pthread_mutex_unlock(&pool->mutexPool);

                    // 销毁工作者线程
                    exitThread(pool);
                }
            }
        }

        // 判断线程池是否被关闭了
        if (pool->isShutdown)
        {
            // 先解锁，防止出现死锁
            pthread_mutex_unlock(&pool->mutexPool);

            // 销毁工作者线程
            exitThread(pool);
        }

        // 从任务队列中取出一个任务
        Task task;
        task.function = pool->taskQ[pool->queueFront].function;
        task.arg = pool->taskQ[pool->queueFront].arg;

        // 移动头结点，形成循环队列
        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;

        // 当前任务数减1
        --pool->queueSize;

        // 因为从任务队列中取出了一个任务，故任务队列不满，从而唤醒阻塞在条件变量notFull的工作者线程
        pthread_cond_broadcast(&pool->notFull);

        // 退出临界区前，先解锁
        pthread_mutex_unlock(&pool->mutexPool);

        printf("worker thread %ld start working...\n", pthread_self());
        // 执行一个任务，当前忙工作者线程加1
        // 访问共享资源busyNum，加锁
        pthread_mutex_lock(&pool->mutexBusy);
        ++pool->busyNum;
        // 退出临界区前先解锁
        pthread_mutex_unlock(&pool->mutexBusy);

        // 执行任务
        task.function(task.arg);

        // 释放资源
        free(task.arg);
        task.arg = NULL;

        printf("worker thread %ld end working...\n", pthread_self());
        // 执行完一个任务，当前忙工作者线程减1
        // 访问共享资源busyNum，加锁
        pthread_mutex_lock(&pool->mutexBusy);
        --pool->busyNum;
        // 退出临界区前，先解锁
        pthread_mutex_unlock(&pool->mutexBusy);
    }

    return NULL;
}

// 单个线程退出
void exitThread(ThreadPool *pool)
{
    // 获取退出线程的ID
    pthread_t tid = pthread_self();

    // 判断每个任务ID是否与退出的线程ID相等
    for (int i = 0; i < pool->maxNum; ++i)
    {
        if (pool->workerIDs[i] == tid)
        {
            // 令退出的工作者线程ID重新置为0
            pool->workerIDs[i] = 0;
            printf("void exitTread(ThreadPool *pool) called, worker thread %ld exiting...\n", tid);
            break;
        }
    }

    // 线程退出
    pthread_exit(NULL);
}
