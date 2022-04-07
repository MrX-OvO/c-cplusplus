#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* callback(void* arg)
{
    for(int i=0; i<5; ++i)
    {
        printf("子线程:\ti=%d\n", i);
    }

    printf("子线程:\t%ld\n", pthread_self());
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, callback, NULL);
    for(int i=0; i<5; ++i)
    {
        printf("主线程:\ti=%d\n", i);
    }
    printf("主线程:\t%ld\n", pthread_self());
    // pthread_join会阻塞主线程，直到它join的子线程执行完
    pthread_join(tid,NULL);
    
    return 0;
}
