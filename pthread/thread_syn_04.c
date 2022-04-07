#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX 5

// 定义链表节点
struct Node
{
    int number;
    struct Node* next;
};

// 定义生产着和消费者信号量
sem_t psem, csem;

// 定义全局变量统计生产者线程和消费者线程分别执行了多少次producer()和consumer()
int p_counts = 0, c_counts = 0;

// 定义互斥锁mutex
pthread_mutex_t mutex;

// 指向头节点的指针
// c语言 (struct 类型名) 才能作为一种类型
struct Node* head = NULL;

// 生产者处理函数
void* producer(void* arg)
{
    for(int i=0; i<MAX; ++i)
    {
        // 生产者信号量拥有资源数-1
        sem_wait(&psem);

        // 若先获取共享资源，即先对共享资源加锁，当sem_wait不满足条件时将线程阻塞，当其他线程访问共享资源时就会发生死锁
        // 故要先调用sem_wait，在对共享资源上锁
        
        // 加互斥锁
        pthread_mutex_lock(&mutex);

        // 创建新链表节点
        struct Node* pnew = (struct Node*)malloc(sizeof(struct Node));
        // 节点初始化
        pnew->number = rand() % 100;
        // 使pnew成为新的头节点
        pnew->next = head;
        // head指针前移
        head = pnew;
        // producer()++
        ++p_counts;
        printf("+=+=+=+ In producer for loop i=%d, producer, id=%ld, number=%2d, p_counts=%2d +=+=+=+\n", i, pthread_self(), pnew->number, p_counts);

        // 解互斥锁
        pthread_mutex_unlock(&mutex);

        // 生产了一个产品，通知消费者消费
        sem_post(&csem);

        // 生产慢一点（控制台输出慢一点）
        sleep(rand() % 5);
    }

    return NULL;
}

// 消费者处理函数
void* consumer(void* arg)
{
    for(int i=0; i<MAX; ++i)
    {
        
        // 消费者信号量拥有资源数-1
        sem_wait(&csem);

        // 加互斥锁
        pthread_mutex_lock(&mutex);

        // 取出链表的头节点，将其删除
        struct Node* pnode = (struct Node*)malloc(sizeof(struct Node));
        pnode = head;
        // consumer()++
        ++c_counts;
        printf("-=-=-=- In consumer for loop i=%d, producer, id=%ld, number=%2d, c_counts=%2d -=-=-=-\n", i, pthread_self(), pnode->number, c_counts);
        // head指向之前生产的产品
        head = pnode->next;
        free(pnode);

        // 消费了一个产品，通知生产者生产
        sem_post(&psem);

        // 解互斥锁
        pthread_mutex_unlock(&mutex);

        // 消费慢一点（控制台输出慢一点）
        sleep(rand() % 5);
    }

    return NULL;
}

int main()
{
    // 初始化信号量
    // 若生产者信号量拥有的资源量初始化为1，则涉及不到线程同步，也就不需要将共享资源上锁；
    // 若生产者信号量拥有的资源量初始化>1，则涉及到线程同步，需要将共享资源上锁；
    sem_init(&psem, 0, 5);
    // 消费者信号量初始化为0,消费者线程就阻塞了
    sem_init(&csem, 0, 0);

    // 初始化互斥锁
    //pthread_mutex_init(&mutex, NULL);
    
    // 创建5个生产者线程和5个消费者进程
    pthread_t p_tid[5], c_tid[5];
    for(int i=0; i<5; ++i)
    {
        pthread_create(&p_tid[i], NULL, producer, NULL);
        pthread_create(&c_tid[i], NULL, consumer, NULL);
    }

    // 阻塞，资源回收
    for(int i=0; i<5; ++i)
    {
        pthread_join(p_tid[i],NULL);
        pthread_join(c_tid[i],NULL);
    }

    // 销毁互斥锁
    //pthread_mutex_destroy(&mutex);

    // 打印p_counts和c_counts
    printf("p_counts=%d, c_counts=%d\n", p_counts, c_counts);

    return 0;
}

