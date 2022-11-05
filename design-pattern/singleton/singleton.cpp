//
// Created by mrxovo on 11/5/22.
//

#include "singleton.h"

Singleton *Singleton::m_instance = nullptr;

Singleton *Singleton::instance() {
    if (m_instance == nullptr) { m_instance = new Singleton(); }
    return m_instance;
}
