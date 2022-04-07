/*
 * @Autor        : MrX-OvO
 * @Date         : 2021-07-10 15:27:35
 * @LastEditTime : 2021-07-13 10:46:21
 * @LastEditors  : MrX-OvO
 * @Description  : description
 * @FilePath     : /c++/taskqueue.hpp
 * Copyright 2021 MrX-OvO, All Rithts Rerserved
 * 
 To: One is never too old to learn...
 */
#include <queue>
#include <pthread.h>

using callback = void (*)(void *arg);
template <typename T>
struct Task
{
    Task()
    {
        function = nullptr;
        arg = nullptr;
    }

    Task(callback f, void *arg)
    {
        this->function = f;
        this->arg = (T *)arg;
    }

    callback function;
    T *arg;
};

template <typename T>
class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();

    // 添加任务
    void addTask(Task<T> task);
    void addTask(callback f, void *arg);
    // 取出任务
    Task<T> takeTask();
    // 获取当前任务个数
    inline size_t getTaskNums() const
    {
        return m_taskQ.size();
    }

private:
    std::queue<Task<T>> m_taskQ;
    pthread_mutex_t m_mutex;
};
