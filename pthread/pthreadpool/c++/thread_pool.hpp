/*
 * @Autor        : MrX-OvO
 * @Date         : 2021-07-12 16:01:21
 * @LastEditTime : 2021-07-13 12:19:54
 * @LastEditors  : MrX-OvO
 * @Description  : description
 * @FilePath     : /c++/thread_pool.hpp
 * Copyright 2021 MrX-OvO, All Rithts Rerserved
 * 
 To: One is never too old to learn...
 */
#pragma once

#include <pthread.h>

//#include "taskqueue.hpp"
// 类模板头文件和源文件分开则需要引入源文件
#include "taskqueue.cpp"

template <typename T>
class Threadpool
{
public:
    Threadpool(int min, int max);
    ~Threadpool();

    // 添加任务
    void addTask(Task<T> task);

    // 获取当前忙的工作者线程数
    int getBusyNums();

    // 获取当前存活的工作者线程数
    int getAliveNums();

private:
    // 管理者线程任务函数
    static void *manager(void *arg);

    // 工作者线程任务函数
    static void *worker(void *arg);

    // 工作者线程退出处理函数
    void exitThread();

private:
    TaskQueue<T> *taskQ;

    pthread_t managerID;
    pthread_t *workerIDs;

    int minNum;
    int maxNum;
    int busyNum;
    int aliveNum;
    int killNum;

    static const int NUMBER = 2;

    pthread_mutex_t mutexPool;
    pthread_cond_t notEmpty;

    bool isShutdown;
};
