#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 5

// 链表节点
struct Node
{
    int number;
    struct Node* next;
};

// 定义全局变量统计生产者线程和消费者线程分别执行了多少次producer()和consumer()
int p_counts = 0, c_counts = 0;

// 定义条件变量，控制消费者进程
pthread_cond_t cond;

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

        // 生产了任务，通知消费者消费
        pthread_cond_broadcast(&cond);
        // 也可调用另一个唤醒阻塞线程函数
        // pthread_cond_signal(&cond);
        // pthread_cond_signal 至少有一个阻塞线程被解除阻塞
        // pthread_cond_broadcast 全部阻塞线程被解除阻塞
        
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
        // 加互斥锁
        pthread_mutex_lock(&mutex);
        
        // 任务列表，即链表中已经没有节点可以消费
        // 消费者线程程需要阻塞
        // 若互斥锁锁住，消费者线程又被阻塞，当生产者线程申请共享资源（任务列表）时，发生死锁
        // 使用while循环判断head是否为空指针
        // 例如：
        // 刚开始列表为空，消费者线程先抢到cpu时间，然后被阻塞;
        // 当生产者线程生产了一个产品时，唤醒所有（一个或多个）被阻塞的消费者线程;
        // 此时，消费者线程加锁成功，从被阻塞处继续向下执行，并成功删除一个节点，然后解锁;
        // 之后，若消费者再次抢到cpu时间;
        // 若使用if判断则，则消费者线程加锁成功，从被阻塞处向下执行，但此时head已为空，访问节点失败，程序出现错误
        // 故，将if改为while，循环判断链表是否为空
        while(head == NULL)
        {
            // 1.阻塞消费者线程
            // 2.阻塞线程之前会自动将互斥锁打开
            // 3.当阻塞线程被唤醒后，自动将互斥锁锁上，从而能继续执行临界区的代码
            pthread_cond_wait(&cond, &mutex);
        }

        // 取出链表的头节点，将其删除
        struct Node* pnode = (struct Node*)malloc(sizeof(struct Node));
        pnode = head;
        // consumer()++
        ++c_counts;
        printf("-=-=-=- In consumer for loop i=%d, producer, id=%ld, number=%2d, c_counts=%2d -=-=-=-\n", i, pthread_self(), pnode->number, c_counts);
        // head指向之前生产的产品
        head = pnode->next;
        free(pnode);

        // 解互斥锁
        pthread_mutex_unlock(&mutex);

        // 消费慢一点（控制台输出慢一点）
        sleep(rand() % 5);
    }

    return NULL;
}

int main()
{
    // 初始化条件变量和互斥锁
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);
    
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

    // 销毁条件变量和互斥锁
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    // 打印p_counts和c_counts
    printf("p_counts=%d, c_counts=%d\n", p_counts, c_counts);

    return 0;
}

