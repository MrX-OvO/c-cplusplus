//
// Created by mrxovo on 11/5/22.
//

#ifndef DESIGN_PATTERN_SINGLETON_H
#define DESIGN_PATTERN_SINGLETON_H

#include <iostream>

template<typename T>
class Singleton {
public:
    Singleton(const Singleton<T> &) = delete;
    Singleton(Singleton<T> &&) = delete;
    Singleton<T> &operator=(const Singleton<T> &) = delete;
    Singleton<T> &operator=(Singleton<T> &&) = delete;

    static T *instance();
    inline void show() { std::cout << "Singleton::show()" << std::endl; }

protected:
    Singleton() = default;
    virtual ~Singleton() = default;

private:
    static T *m_instance;
};

#endif//DESIGN_PATTERN_SINGLETON_H
