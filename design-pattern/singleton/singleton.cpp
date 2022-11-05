//
// Created by mrxovo on 11/5/22.
//

#include "singleton.h"

std::shared_ptr<Singleton> Singleton::m_instance = nullptr;

std::shared_ptr<Singleton> Singleton::instance() {
    if (m_instance == nullptr) { m_instance = std::shared_ptr<Singleton>(); }
    return m_instance;
}
