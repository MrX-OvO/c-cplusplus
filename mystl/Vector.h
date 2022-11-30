//
// Created by mrxovo on 11/29/22.
//

#ifndef MYSTL_VECTOR_H
#define MYSTL_VECTOR_H

#include <cstring>
#include <iostream>
#include <typeinfo>

namespace mystl {
    template<typename T>
    class Vector {
    public:
        using value_type = T;
        using size_type = size_t;

        Vector();
        Vector &operator=(const Vector &other);
        ~Vector();

        inline bool empty() const { return m_size == 0; }
        inline size_type size() const { return m_size; }
        inline size_type capacity() const { return m_capacity; }
        inline void clear() { m_size = 0; }
        inline T *data() { return m_data; }
        T &at(int pos);
        T &operator[](int pos);
        T &front();
        T &back();
        void push_back(const T &value);
        void pop_back();
        void reserve(size_type new_cap);
        void resize(size_type count, value_type value = value_type());
        void swap(Vector &other);
        void show() const;

        class Iterator {
        public:
            Iterator() : m_pointer(nullptr) {}
            explicit Iterator(T *pointer) : m_pointer(pointer) {}
            Iterator &operator=(const Iterator &other) {
                if (this != &other) { m_pointer = other.m_pointer; }
                return *this;
            }
            ~Iterator() = default;

            bool operator==(const Iterator &other) const { return m_pointer == other.m_pointer; }
            bool operator!=(const Iterator &other) const { return m_pointer != other.m_pointer; }
            Iterator &operator++() {
                m_pointer += 1;
                return *this;
            }
            Iterator operator++(int) & {
                auto it = *this;
                ++(*this);
                return it;
            }
            Iterator operator+(int n) {
                auto it = *this;
                it.m_pointer += n;
                return it;
            }
            Iterator &operator+=(int n) {
                m_pointer += n;
                return *this;
            }
            Iterator &operator--() {
                m_pointer -= 1;
                return *this;
            }
            Iterator operator--(int) & {
                auto it = *this;
                --(*this);
                return it;
            }
            Iterator operator-(int n) {
                auto it = *this;
                it.m_pointer -= n;
                return it;
            }
            Iterator &operator-=(int n) {
                m_pointer -= n;
                return *this;
            }
            int operator-(const Iterator &other) const { return m_pointer - other.m_pointer; }
            T &operator*() const { return *m_pointer; }
            T *operator->() const { return m_pointer; }

        private:
            value_type *m_pointer;
        };

        Iterator begin();
        Iterator end();
        Iterator find(const T &value);
        Iterator insert(Iterator pos, const T &value);
        Iterator insert(Iterator pos, const T &value, size_type count);
        Iterator erase(Iterator pos);
        Iterator erase(Iterator first, Iterator last);

        class ReverseIterator {
        public:
            ReverseIterator() : m_pointer(nullptr) {}
            explicit ReverseIterator(T *pointer) : m_pointer(pointer) {}
            ReverseIterator &operator=(const ReverseIterator &other) {
                if (this != &other) { m_pointer = other.m_pointer; }
                return *this;
            }
            ~ReverseIterator() = default;

            bool operator==(const ReverseIterator &other) const { return m_pointer == other.m_pointer; }
            bool operator!=(const ReverseIterator &other) const { return m_pointer != other.m_pointer; }
            ReverseIterator &operator++() {
                m_pointer -= 1;
                return *this;
            }
            ReverseIterator operator++(int) & {
                auto it = *this;
                ++(*this);
                return it;
            }
            ReverseIterator operator+(int n) {
                auto it = *this;
                it.m_pointer -= n;
                return it;
            }
            ReverseIterator &operator+=(int n) {
                m_pointer -= n;
                return *this;
            }
            ReverseIterator &operator--() {
                m_pointer += 1;
                return *this;
            }
            ReverseIterator operator--(int) & {
                auto it = *this;
                --(*this);
                return it;
            }
            ReverseIterator operator-(int n) {
                auto it = *this;
                it.m_pointer += n;
                return it;
            }
            ReverseIterator &operator-=(int n) {
                m_pointer += n;
                return *this;
            }
            int operator-(const ReverseIterator &other) const { return m_pointer - other.m_pointer; }
            T &operator*() const { return *m_pointer; }
            T *operator->() const { return m_pointer; }

        private:
            value_type *m_pointer;
        };

        ReverseIterator rbegin();
        ReverseIterator rend();
        ReverseIterator rfind(const T &value);

    private:
        bool is_basic_type() const;

    private:
        value_type *m_data;
        size_type m_size;
        size_type m_capacity;
    };

    template<typename T>
    Vector<T>::Vector() : m_data(nullptr), m_size(0), m_capacity(0) {}

    template<typename T>
    Vector<T> &Vector<T>::operator=(const Vector<T> &other) {
        if (this != &other) {
            if (m_capacity < other.m_size) {// 当前vector的容量小于目标vector的大小，进行扩容
                if (m_data != nullptr) {
                    delete[] m_data;
                    m_data = nullptr;
                }
                while (m_capacity < other.m_capacity) {
                    if (m_capacity == 0) {
                        m_capacity = 1;
                    } else {
                        m_capacity *= 2;
                    }
                }
                m_data = new value_type[m_capacity];// 开辟新空间
            }
            // 进行相应赋值
            if (is_basic_type()) {
                memmove(m_data, other.m_data, other.m_size * sizeof(T));
            } else {
                for (int i = 0; i < other.m_size; ++i) { m_data[i] = other.m_data[i]; }
            }
            m_size = other.m_size;
        }
        return *this;
    }

    template<typename T>
    Vector<T>::~Vector() {
        if (m_data != nullptr) {
            delete[] m_data;
            m_data = nullptr;
        }
        m_size = 0;
        m_capacity = 0;
    }

    template<typename T>
    T &Vector<T>::at(int pos) {
        if (pos < 0 || pos >= m_size) { throw std::logic_error("out of range"); }// 越界检查
        return m_data[pos];
    }

    template<typename T>
    T &Vector<T>::front() {
        if (m_size == 0) { throw std::logic_error("vector is empty"); }// 越界检查
        return m_data[0];
    }

    template<typename T>
    T &Vector<T>::back() {
        if (m_size == 0) { throw std::logic_error("vector is empty"); }// 越界检查
        return m_data[m_size - 1];
    }

    template<typename T>
    T &Vector<T>::operator[](int pos) { return at(pos); }

    template<typename T>
    void Vector<T>::push_back(const T &value) {
        if (m_size < m_capacity) {// 空间还能容纳数据
            m_data[m_size] = value;
            m_size++;
            return;
        }

        if (m_capacity == 0) { m_capacity = 1; }// 初始容量置1
        else {
            m_capacity *= 2;// 扩容
        }
        // 开辟新空间，进行相应赋值
        auto *data = new T[m_capacity];
        if (is_basic_type()) {
            memcpy(data, m_data, m_size * sizeof(T));
        } else {
            for (int i = 0; i < m_size; ++i) { data[i] = m_data[i]; }
        }
        if (m_data != nullptr) {
            delete[] m_data;
            m_data = nullptr;
        }
        m_data = data;
        m_data[m_size] = value;
        m_size++;
    }

    template<typename T>
    void Vector<T>::pop_back() {
        if (!empty()) { m_size--; }
    }

    template<typename T>
    void Vector<T>::reserve(size_type new_cap) {
        if (m_capacity >= new_cap) { return; }// 可以容纳new_cap，直接返回

        while (m_capacity < new_cap) {// 改变m_capacity，至少容纳new_cap
            if (m_capacity == 0) {
                m_capacity = 1;
            } else {
                m_capacity *= 2;
            }
        }
        // 开辟新空间，进行相应赋值
        auto *data = new T[m_capacity];
        if (is_basic_type()) {
            memcpy(data, m_data, m_size * sizeof(T));
        } else {
            for (int i = 0; i < m_size; ++i) { data[i] = m_data[i]; }
        }
        if (m_data != nullptr) {
            delete[] m_data;
            m_data = nullptr;
        }
        m_data = data;
    }

    template<typename T>
    void Vector<T>::resize(size_type count, value_type value) {
        if (m_size >= count) {// 缩小至count
            m_size = count;
            return;
        }

        if (m_capacity >= count) {// 扩容到count,不改变m_capacity
            for (int i = m_size; i < count; ++i) { m_data[i] = value; }
            m_size = count;
            return;
        }

        while (m_capacity < count) {// 扩容到count，改变m_capacity
            if (m_capacity == 0) {
                m_capacity = 1;
            } else {
                m_capacity *= 2;
            }
        }
        // 开辟新空间，进行相应赋值
        auto *data = new T[m_capacity];
        if (is_basic_type()) {
            memcpy(data, m_data, m_size * sizeof(T));
        } else {
            for (int i = 0; i < m_size; ++i) { data[i] = m_data[i]; }
        }
        for (int i = m_size; i < count; ++i) { m_data[i] = value; }
        if (m_data != nullptr) {
            delete[] m_data;
            m_data = nullptr;
        }
        m_data = data;
        m_size = count;
    }

    template<typename T>
    void Vector<T>::swap(Vector<T> &other) {
        // 保存目标相关数据
        value_type *data = other.m_data;
        size_type size = other.m_size;
        size_type capacity = other.m_capacity;

        // 直接交换
        other.m_data = m_data;
        other.m_size = m_size;
        other.m_capacity = m_capacity;

        m_data = data;
        m_size = size;
        m_capacity = capacity;
    }

    template<typename T>
    void Vector<T>::show() const {
        std::cout << "size = " << size() << ", capacity = " << capacity() << std::endl;
        for (int i = 0; i < m_size; ++i) {
            std::cout << m_data[i] << (i + 1 == m_size ? "" : ", ");
        }
        std::cout << std::endl;
    }

    template<typename T>
    typename Vector<T>::Iterator Vector<T>::begin() { return Vector<T>::Iterator{m_data}; }

    template<typename T>
    typename Vector<T>::Iterator Vector<T>::end() { return Vector<T>::Iterator{m_data + m_size}; }

    template<typename T>
    typename Vector<T>::Iterator Vector<T>::find(const T &value) {
        for (auto it = begin(); it != end(); it++) {
            if (*it == value) { return it; }
        }
        return end();
    }

    template<typename T>
    typename Vector<T>::Iterator Vector<T>::insert(Vector::Iterator pos, const T &value) { return insert(pos, value, 1); }

    template<typename T>
    typename Vector<T>::Iterator Vector<T>::insert(Vector::Iterator pos, const T &value, Vector::size_type count) {
        auto size = pos - begin();// 找到插入位置
        if (m_capacity >= m_size + count) {
            if (is_basic_type()) {
                memmove(m_data + size + count, m_data + size, (m_size - size) * sizeof(T));
            } else {
                for (int i = m_size; i > size; --i) { m_data[i + count - 1] = m_data[i - 1]; }// 从末尾开始遍历
            }
            for (int i = 0; i < count; ++i) { m_data[i + size] = value; }// 插入新数据
            m_size += count;
            return Vector<T>::Iterator{m_data + size};
        }
        while (m_capacity < m_size + count) {// 扩容
            if (m_capacity == 0) {
                m_capacity = 1;
            } else {
                m_capacity *= 2;
            }
        }
        //开辟新空间，进行相应赋值
        auto data = new T[m_capacity];
        if (is_basic_type()) {
            memcpy(data, m_data, m_size * sizeof(T));
        } else {
            for (int i = 0; i < size; ++i) { data[i] = m_data[i]; }// 复制插入位置前面的数据
        }
        for (int i = 0; i < count; ++i) { data[i + size] = value; }// 插入新数据
        if (is_basic_type()) {
            memcpy(data + size + count, m_data + size, (m_size - size) * sizeof(T));
        } else {
            for (int i = size; i < m_size; ++i) { data[i + count] = m_data[i]; }//赋值包括插入位置后面的数据
        }
        if (m_data != nullptr) {
            delete[] m_data;
            m_data = nullptr;
        }
        m_data = data;
        m_size += count;
        return Vector<T>::Iterator{m_data + size};
    }

    template<typename T>
    typename Vector<T>::Iterator Vector<T>::erase(Iterator pos) {
        if (end() - pos == 1) {// pos 指代末元素，那么返回 end() 迭代器
            m_size -= 1;
            return end();
        }
        // 返回最后移除元素之后的迭代器
        auto size = pos - begin();
        if (is_basic_type()) {
            memmove(m_data + size, m_data + size + 1, (m_size - size) * sizeof(T));
        } else {
            for (int i = size; i < m_size; ++i) { m_data[i] = m_data[i + 1]; }
        }
        m_size -= 1;
        return pos;
    }

    template<typename T>
    typename Vector<T>::Iterator Vector<T>::erase(Iterator first, Iterator last) {
        if (first == last) { return last; }// 范围 [first, last) 为空，那么返回 last
        auto f = first - begin();
        auto l = last - begin();
        auto size = last - first;
        if (last == end()) {// 在移除前 last == end()，那么返回更新的 end() 迭代器
            m_size -= size;
            return end();
        }
        if (is_basic_type()) {
            memmove(m_data + f, m_data + l, (m_size - l + 1) * sizeof(T));
        } else {
            for (int i = 0; i < m_size - l + 1; ++i) { m_data[i + f] = m_data[i + l]; }
        }
        m_size -= size;
        return first;
    }

    template<typename T>
    typename Vector<T>::ReverseIterator Vector<T>::rbegin() { return Vector<T>::ReverseIterator{m_data + m_size - 1}; }

    template<typename T>
    typename Vector<T>::ReverseIterator Vector<T>::rend() { return Vector<T>::ReverseIterator{m_data - 1}; }

    template<typename T>
    typename Vector<T>::ReverseIterator Vector<T>::rfind(const T &value) {
        for (auto it = rbegin(); it != rend(); it++) {
            if (*it == value) { return it; }
        }
        return rend();
    }

    template<typename T>
    bool Vector<T>::is_basic_type() const {
        if (std::is_pointer<T>::value) { return true; }
        return (typeid(T) == typeid(bool)) ||
               (typeid(T) == typeid(char)) ||
               (typeid(T) == typeid(unsigned char)) ||
               (typeid(T) == typeid(short)) ||
               (typeid(T) == typeid(unsigned short)) ||
               (typeid(T) == typeid(int)) ||
               (typeid(T) == typeid(unsigned int)) ||
               (typeid(T) == typeid(long)) ||
               (typeid(T) == typeid(unsigned long)) ||
               (typeid(T) == typeid(float)) ||
               (typeid(T) == typeid(double));
    }
}// namespace mystl

#endif//MYSTL_VECTOR_H
