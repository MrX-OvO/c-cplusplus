//
// Created by mrxovo on 11/5/22.
//

#ifndef DESIGN_PATTERN_SINGLETON_H
#define DESIGN_PATTERN_SINGLETON_H

#include <iostream>

class Singleton {
public:
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    static Singleton *instance();
    inline void show() { std::cout << "Singleton::show()" << std::endl; }

private:
    Singleton() = default;
    ~Singleton() = default;

    class Deleter {
    public:
        Deleter() = default;
        ~Deleter() {
            if (Singleton::m_instance != nullptr) { delete Singleton::m_instance; }
        }
    };
    static Deleter deleter;

private:
    static Singleton *m_instance;
};

#endif//DESIGN_PATTERN_SINGLETON_H
