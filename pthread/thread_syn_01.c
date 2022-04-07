#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 定义数数次数 50次
#define MAX 50

// 定义全局变量 mutex
pthread_mutex_t mutex;

// 定义全局显示的number
int number;

// 线程A处理函数
void* countA(void* arg)
{
    for(int i=0; i<MAX; ++i)
    {
        // 给A加互斥锁
        pthread_mutex_lock(&mutex);
        int cur = number;
        ++cur;
        number = cur;
        printf("Thread A, id=%ld, number=%d\n", pthread_self(), number);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// 线程B处理函数
void* countB(void* arg)
{
    for(int i=0; i<MAX; ++i)
    {
        // 给B加互斥锁
        pthread_mutex_lock(&mutex);
        int cur = number;
        ++cur;
        number = cur;
        printf("Thread B, id=%ld, number=%d\n", pthread_self(), number);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}


int main()
{
    pthread_t A, B;
    
    // 初始化互斥锁
    pthread_mutex_init(&mutex, NULL);
    
    // 创建两个子线程
    pthread_create(&A, NULL, countA, NULL);
    pthread_create(&B, NULL, countB, NULL);
    
    // 阻塞，资源回收
    pthread_join(A,NULL);
    pthread_join(B,NULL);

    // 销毁互斥锁
    // 在线程销毁后再释放互斥锁
    pthread_mutex_destroy(&mutex);
    
    return 0;
}

