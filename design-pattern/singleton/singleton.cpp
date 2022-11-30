//
// Created by mrxovo on 11/5/22.
//

#include "singleton.h"

template<typename T>
T *Singleton<T>::m_instance = nullptr;

template<typename T>
T *Singleton<T>::instance() {
    if (m_instance == nullptr) { m_instance = new T(); }
    return m_instance;
}
