#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

struct Test{
    int num;
    int age;
};

// 将t声明为全局变量
// struct Test t;

void* callback(void* arg)
{
    for(int i=0; i<5; ++i)
    {
        printf("子线程:\ti=%d\n", i);
    }

    // t是临时变量，调用pthread_exit()后就消失了，再去访问只能得到错误的数据
    // 解决方案：
    // 1.将t声明为全局变量
    // 2.将t改为static
    // 3.使用主线程的栈空间
    
    // 将t改为static
    // static struct Test t; 
    
    // t.num=12345;
    // t.age=666;
    
    // pthread_exit(&t);
    
    // 使用主线程栈空间
    struct Test* t=(struct Test*) arg;
    t->num=12345;
    t->age=666;
    
    printf("子线程:\t%ld\n", pthread_self());
    pthread_exit(t);
}

int main()
{
    // 使用主线程栈空间
    struct Test t;

    pthread_t tid;
    // pthread_create(&tid, NULL, callback, NULL);
    
    // 使用主线程栈空间
    pthread_create(&tid, NULL, callback, &t);
    for(int i=0; i<5; ++i)
    {
        printf("主线程:\ti=%d\n", i);
    }
    printf("主线程:\t%ld\n", pthread_self());

    void* ptr;
    pthread_join(tid, &ptr);

    struct Test* t_ptr=(struct Test*)ptr;
    printf("子线程中返回的数据：\n");
    printf("num：%d, age:%d\n", t_ptr->num, t_ptr->age);

    return 0;
}
