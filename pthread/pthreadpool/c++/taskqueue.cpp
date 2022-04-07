/*
 * @Autor        : MrX-OvO
 * @Date         : 2021-07-10 15:27:54
 * @LastEditTime : 2021-07-13 10:39:23
 * @LastEditors  : MrX-OvO
 * @Description  : description
 * @FilePath     : /c++/taskqueue.cpp
 * Copyright 2021 MrX-OvO, All Rithts Rerserved
 * 
 To: One is never too old to learn...
 */
#include "taskqueue.hpp"

template <typename T>
TaskQueue<T>::TaskQueue()
{
    pthread_mutex_init(&m_mutex, NULL);
}

template <typename T>
TaskQueue<T>::~TaskQueue()
{
    pthread_mutex_destroy(&m_mutex);
}

// 添加任务
template <typename T>
void TaskQueue<T>::addTask(Task<T> task)
{
    pthread_mutex_lock(&m_mutex);
    m_taskQ.push(task);
    pthread_mutex_unlock(&m_mutex);
}

// 添加任务
template <typename T>
void TaskQueue<T>::addTask(callback f, void *arg)
{
    pthread_mutex_lock(&m_mutex);
    m_taskQ.push(Task(f, arg));
    pthread_mutex_unlock(&m_mutex);
}

// 取出任务
template <typename T>
Task<T> TaskQueue<T>::takeTask()
{
    Task<T> task;
    pthread_mutex_lock(&m_mutex);
    if (!m_taskQ.empty())
    {
        task = m_taskQ.front();
        m_taskQ.pop();
    }
    pthread_mutex_unlock(&m_mutex);
    return task;
}
