#include "thread_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// 任务函数
void taskFunc(void *arg)
{
    // 将传入参数强制类型转换
    int num = *(int *)arg;
    printf("thread %ld is working, number=%d\n", pthread_self(), num);
}

int main()
{
    // 任务队列容量
    int qCanpacity = 10;

    // 线程池工作者线程数的上下限
    int min = 3, max = 10;
    
    // 创建线程池
    printf("+=+=+ main thread, create threadpool...\n");
    ThreadPool *pool = createThreadPool(qCanpacity, min, max);
    
    sleep(1);
    printf("+=+=+ main thread, before add task, sleep 1s...\n");
    
    // 添加任务
    printf("+=+=+ main thread, add task...\n");
    for (int i = 0; i < 50; ++i)
    {
        // 任务中需要用到的数据
        // 这里不用释放num，因为在worker()中会释放
        int *num = (int *)malloc(sizeof(int));
        if(NULL == num)
        {
            printf("malloc num failed...\n");
            
            // 销毁线程池
            printf("+=+=+ main thread, destroy threadpool...\n");
            destroyThreadPool(pool);
            
            return -1;
        }
        *num = i * i;
        addTask(pool, taskFunc, num);
        //usleep(1000);
    }
    
    // 让主线程睡眠一段时间，保证子线程先执行完，然后在执行主线程（即，先让线程池中的线程执行完，再继续向下执行，销毁线程池）
    sleep(30);
    printf("+=+=+ main thread, after add task, sleep 30s...\n");

    // 销毁线程池
    printf("+=+=+ main thread, destroy threadpool...\n");
    destroyThreadPool(pool);

    return 0;
}

