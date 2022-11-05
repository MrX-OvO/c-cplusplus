//
// Created by mrxovo on 11/5/22.
//

#ifndef DESIGN_PATTERN_SINGLETON_H
#define DESIGN_PATTERN_SINGLETON_H

#include <iostream>
#include <memory>

class Singleton {
public:
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    static std::shared_ptr<Singleton> instance();
    inline void show() { std::cout << "Singleton::show()" << std::endl; }

private:
    Singleton() = default;
    ~Singleton() = default;

private:
    static std::shared_ptr<Singleton> m_instance;
};

#endif//DESIGN_PATTERN_SINGLETON_H
