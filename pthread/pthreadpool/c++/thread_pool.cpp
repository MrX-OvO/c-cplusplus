/*
 * @Autor        : MrX-OvO
 * @Date         : 2021-07-12 16:23:02
 * @LastEditTime : 2021-07-13 12:24:17
 * @LastEditors  : MrX-OvO
 * @Description  : description
 * @FilePath     : /c++/thread_pool.cpp
 * Copyright 2021 MrX-OvO, All Rithts Rerserved
 *
 To: One is never too old to learn...
 */
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>

#include "thread_pool.hpp"

template <typename T> Threadpool<T>::Threadpool(int min, int max) {
  do {
    // 实例化任务队列
    this->taskQ = new TaskQueue<T>;
    if (nullptr == this->taskQ) {
      std::cout << "new taskQ failed...\n";
      break;
    }

    // 实例化存放工作者线程ID的指针
    this->workerIDs = new pthread_t[max];
    if (nullptr == this->workerIDs) {
      std::cout << "new workerIDs failed...\n";
      break;
    }
    memset(workerIDs, 0, sizeof(pthread_t) * max);

    // 初始化有关工作者线程数量的变量
    this->minNum = min;
    this->maxNum = max;
    this->busyNum = 0;
    this->aliveNum = min;
    this->killNum = 0;

    if (pthread_mutex_init(&this->mutexPool, NULL) ||
        pthread_cond_init(&this->notEmpty, NULL)) {
      std::cout << "mutex or cond init failed...\n";
      break;
    }

    // 初始化关闭线程池为false
    this->isShutdown = false;

    // pthread_create()回调函数如果是类的非静态成员函数，则编译报错
    // void* (Threadpool<T>::*)(void* arg)类型的实参与void* (void*
    // arg)类型的形参不兼容 对象实例化后manager()和worker()这两个函数才有地址
    // 解决方法
    // 1.声明为static
    // 2.类的友元函数（破坏封装性，不推荐）
    // 静态函数不能访问非静态变量，故传入this指针
    pthread_create(&this->managerID, NULL, manager, this);
    for (decltype(min) i = 0; i < min; ++i) {
      pthread_create(&this->workerIDs[i], NULL, worker, this);
    }
    return;

  } while (0);

  // 创建失败 释放资源
  if (this->taskQ) {
    delete this->taskQ;
    this->taskQ = nullptr;
  }
  if (this->workerIDs) {
    delete[] this->workerIDs;
    this->workerIDs = nullptr;
  }
}

template <typename T> Threadpool<T>::~Threadpool() {
  // 关闭线程池
  this->isShutdown = true;

  // 阻塞回收管理者线程
  pthread_join(this->managerID, NULL);

  // 唤醒被条件变量notEmpty阻塞的工作者线程
  for (decltype(this->aliveNum) i = 0; i < this->aliveNum; ++i) {
    // 唤醒后会执行到Threadpool<T>::worker中判断线程池是否关闭，然后自动退出
    pthread_cond_broadcast(&this->notEmpty);
  }

  // 释放资源
  if (this->taskQ) {
    delete this->taskQ;
    this->taskQ = nullptr;
  }
  if (this->workerIDs) {
    delete[] this->workerIDs;
    this->workerIDs = nullptr;
  }

  pthread_mutex_destroy(&this->mutexPool);
  pthread_cond_destroy(&this->notEmpty);

  std::cout << "+=+=+ thread pool destroyed...\n";
}

// 添加任务
template <typename T> void Threadpool<T>::addTask(Task<T> task) {
  // 如果线程池关闭了，则先解锁，防止出现死锁
  if (this->isShutdown) {
    pthread_mutex_unlock(&this->mutexPool);
    return;
  }

  // 不加互斥锁是因为TaskQueue::addtask中已经添加了互斥锁
  this->taskQ->addTask(task);

  std::cout << "+=+=+ added one task...\n";
}

// 获取当前忙的工作者线程数
template <typename T> int Threadpool<T>::getBusyNums() {
  pthread_mutex_lock(&this->mutexPool);
  auto busyNum = this->busyNum;
  pthread_mutex_unlock(&this->mutexPool);
  return busyNum;
}

// 获取当前存活的工作者线程数
template <typename T> int Threadpool<T>::getAliveNums() {
  pthread_mutex_lock(&this->mutexPool);
  auto aliveNum = this->aliveNum;
  pthread_mutex_unlock(&this->mutexPool);
  return aliveNum;
}

// 管理者线程任务函数
template <typename T> void *Threadpool<T>::manager(void *arg) {
  // 获取pthread_create()传入的pool
  // 需要强制类型转换
  auto pool = static_cast<Threadpool *>(arg);
  while (!pool->isShutdown) {
    // 每3s检测一次
    sleep(3);

    pthread_mutex_lock(&pool->mutexPool);

    // 获取任务队列中的任务个数和当前工作者线程相关数量
    auto queueSize = pool->taskQ->getTaskNums();
    auto minNum = pool->minNum;
    auto maxNum = pool->maxNum;
    auto busyNum = pool->busyNum;
    auto aliveNum = pool->aliveNum;
    auto kiilNum = pool->killNum;

    // 添加工作者线程
    // rule:
    // 当前任务个数 > 存活的工作者线程个数 && 存活的工作者线程个数 <
    // 最大工作者线程个数
    if (queueSize > aliveNum && aliveNum < maxNum) {
      // counter用于控制添加工作者线程上限（NUMBER = 2）
      int counter = 0;
      for (decltype(maxNum) i = 0;
           i < maxNum && counter < NUMBER && aliveNum < maxNum; ++i) {
        if (pool->workerIDs[i] == 0) {
          std::cout << "+=+=+ manager thread \"added\" one worker thread, i = "
                    << i << "...\n";
          pthread_create(&pool->workerIDs[i], NULL, worker, pool);
          ++counter;
          ++pool->aliveNum;
        }
      }
    }

    // 杀死工作者线程
    // rule:
    // 忙的工作者线程数量 × 2 < 存活的工作者线程数量 && 存活的工作者线程数量 >
    // 最小工作者线程数量
    if (busyNum * 2 < aliveNum && aliveNum > minNum) {
      pool->killNum = NUMBER;
      for (size_t i = 0; i < NUMBER; ++i) {
        // 让工作者线程自杀
        // 唤醒后，会在被条件变量notEmpty阻塞处（pthread_cond_wait）继续向下执行，然后执行相关退出操作
        std::cout << "+=+=+ manager thread \"killed\" one worker thread i = "
                  << i << "...\n";
        pthread_cond_broadcast(&pool->notEmpty);
      }
    }
    pthread_mutex_unlock(&pool->mutexPool);
  }

  return NULL;
}

// 工作者线程任务函数
template <typename T> void *Threadpool<T>::worker(void *arg) {
  // 获取pthread_create()传入的pool
  // 需要强制类型转换
  auto pool = static_cast<Threadpool *>(arg);
  while (true) {
    pthread_mutex_lock(&pool->mutexPool);
    // 循环判断任务队列是否为空且线程池是否关闭
    while (pool->taskQ->getTaskNums() == 0 && !pool->isShutdown) {
      // 阻塞工作者线程
      std::cout << "+=+=+ worker thread " << pthread_self()
                << " blocked by empty...\n";
      pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);
      std::cout << "+=+=+ worker thread " << pthread_self()
                << " waked up by not empty...\n";

      // 是否需要杀死工作者线程
      if (pool->killNum > 0) {
        --pool->killNum;
        if (pool->aliveNum > pool->minNum) {
          --pool->aliveNum;
          // 退出先解锁，防止出现死锁
          pthread_mutex_unlock(&pool->mutexPool);
          pool->exitThread();
        }
      }
    }

    // 线程池关闭了则要先解锁，防止出现死锁
    if (pool->isShutdown) {
      pthread_mutex_unlock(&pool->mutexPool);
      pool->exitThread();
    }

    // 获取任务
    auto task = pool->taskQ->takeTask();

    // 忙工作者线程数加1
    ++pool->busyNum;
    std::cout << "+=+=+ worker thread " << pthread_self()
              << " start working...\n";

    // 执行任务
    task.function(task.arg);
    // 编译出错，警告：删除‘void*’未定义
    // 解决方法：使用模板
    delete task.arg;
    task.arg = nullptr;

    // 忙工作者线程数减1
    --pool->busyNum;
    std::cout << "+=+=+ worker thread " << pthread_self()
              << " end working...\n";

    pthread_mutex_unlock(&pool->mutexPool);
  }

  return NULL;
}

// 工作者线程退出处理函数
template <typename T> void Threadpool<T>::exitThread() {
  // 获取退出线程ID
  auto tid = pthread_self();
  for (decltype(this->maxNum) i = 0; i < this->maxNum; ++i) {
    // 如果获取的退出线程ID与存储在workerIDs[]中的线程ID之一相同，
    // 则将workerIDs[]中对应的置为0
    if (this->workerIDs[i] == tid) {
      this->workerIDs[i] = 0;
      std::cout << "+=+=+ exitThread() called, worker thread " << pthread_self()
                << " exited...\n";
    }
  }
}
