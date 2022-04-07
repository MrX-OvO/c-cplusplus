/*
 * @Autor        : MrX-OvO
 * @Date         : 2021-07-10 13:27:26
 * @LastEditTime : 2021-07-13 12:18:35
 * @LastEditors  : MrX-OvO
 * @Description  : description
 * @FilePath     : /c++/main.cpp
 * Copyright 2021 MrX-OvO, All Rithts Rerserved
 * 
 To: One is never too old to learn...
 */
#include <iostream>
#include <unistd.h>

//#include "thread_pool.hpp"
// 类模板头文件和源文件分开则需要引入源文件
#include "thread_pool.cpp"

#define TASK_NUM 20

void taskFunc(void *arg)
{
    auto number = *(static_cast<int *>(arg));
    std::cout << "+=+=+ worker thread " << pthread_self() << " is working, number = " << number << "...\n";
}

int main()
{
    int min = 3, max = 10;

    std::cout << "+=+=+ thread pool created...\n";

    Threadpool<int> pool(min, max);

    sleep(2);
    std::cout << "+=+=+ before add task, sleep 2s...\n";

    for (decltype(TASK_NUM) i = 0; i < TASK_NUM; ++i)
    {
        auto num = new int(i * i);
        pool.addTask(Task<int>(taskFunc, num));
    }

    sleep(25);

    return 0;
}
