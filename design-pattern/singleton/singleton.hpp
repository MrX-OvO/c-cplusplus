#include <atomic>
#include <iostream>
#include <mutex>

namespace singleton {
// 饿汉模式
template <typename T> class SingletonEager {
public:
  SingletonEager(const SingletonEager &) = delete;
  SingletonEager(SingletonEager &&) = delete;
  SingletonEager &operator=(const SingletonEager &) = delete;

  T &getVal() { return value; }
  void setVal(const T &val) { value = val; }

protected:
  SingletonEager() = default;
  ~SingletonEager() = default;
  static SingletonEager *getInstance() { return m_instance; }

private:
  static SingletonEager *m_instance;
  T value;
};

// 直接先 new 出来
template <typename T>
SingletonEager<T> *SingletonEager<T>::m_instance = new SingletonEager<T>();

// 懒汉模式
template <typename T> class SingletonLazy {
public:
  SingletonLazy(const SingletonLazy &) = delete;
  SingletonLazy(SingletonLazy &&) = delete;
  SingletonLazy &operator=(const SingletonLazy &) = delete;

  T &getVal() { return value; }
  void setVal(const T &val) { value = val; }

protected:
  SingletonLazy() = default;
  ~SingletonLazy() = default;
  static SingletonLazy *getInstance() {
    if (m_instance == nullptr) {
      m_instance = new SingletonLazy();
    }
    return m_instance;
  }

private:
  static SingletonLazy *m_instance;
  T value;
};

template <typename T> SingletonLazy<T> *SingletonLazy<T>::m_instance = nullptr;

// 懒汉模式线程安全问题
// 使用 mutex ，双重检查锁定
template <typename T> class SingletonLazyMutex {
public:
  SingletonLazyMutex(const SingletonLazyMutex &) = delete;
  SingletonLazyMutex(SingletonLazyMutex &&) = delete;
  SingletonLazyMutex &operator=(const SingletonLazyMutex &) = delete;

  T &getVal() { return value; }
  void setVal(const T &val) { value = val; }

protected:
  SingletonLazyMutex() = default;
  ~SingletonLazyMutex() = default;
  static SingletonLazyMutex *getInstance() {
    if (m_instance == nullptr) {
      m_mutex.lock();
      if (m_instance == nullptr) {
        m_instance = new SingletonLazyMutex();
      }
    }
    m_mutex.unlock();
    return m_instance;
  }

private:
  static SingletonLazyMutex *m_instance;
  static std::mutex m_mutex;
  T value;
};

template <typename T>
SingletonLazyMutex<T> *SingletonLazyMutex<T>::m_instance = nullptr;

template <typename T> std::mutex SingletonLazyMutex<T>::m_mutex;

// 使用原子变量 atomic
template <typename T> class SingletonLazyAtomic {
public:
  SingletonLazyAtomic(const SingletonLazyAtomic &) = delete;
  SingletonLazyAtomic(SingletonLazyAtomic &&) = delete;
  SingletonLazyAtomic &operator=(const SingletonLazyAtomic &) = delete;

  T &getVal() { return value; }
  void setVal(const T &val) { value = val; }

protected:
  SingletonLazyAtomic() = default;
  ~SingletonLazyAtomic() = default;
  static SingletonLazyAtomic *getInstance() {
    if (m_instance == nullptr) {
      m_mutex.lock();
      if (m_instance == nullptr) {
        m_instance = new SingletonLazyAtomic();
      }
    }
    m_mutex.unlock();
    return m_instance;
  }

private:
  static std::atomic<SingletonLazyAtomic *> m_instance;
  static std::mutex m_mutex;
  T value;
};

template <typename T> std::atomic<SingletonLazyAtomic<T> *> m_instance;

template <typename T> std::mutex SingletonLazyAtomic<T>::m_mutex;

// 局部对象
template <typename T> class SingletonLocalStatic {
public:
  SingletonLocalStatic(const SingletonLocalStatic &) = delete;
  SingletonLocalStatic(SingletonLocalStatic &&) = delete;
  SingletonLocalStatic &operator=(const SingletonLocalStatic &) = delete;

  T &getVal() { return value; }
  void setVal(const T &val) { value = val; }

protected:
  SingletonLocalStatic() = default;
  ~SingletonLocalStatic() = default;
  static SingletonLocalStatic *getInstance() {
    static SingletonLocalStatic m_instance;
    return &m_instance;
  }

private:
  T value;
};
} // namespace singleton