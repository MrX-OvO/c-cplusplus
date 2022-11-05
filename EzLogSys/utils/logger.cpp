//
// Created by mrxovo on 11/4/22.
//

#include "logger.h"

#include <cstdarg>
#include <cstring>
#include <ctime>

#include <stdexcept>

namespace utils {

    const char *Logger::m_clevel[LEVEL_COUNT] = {
            "DEBUG",
            "INFO",
            "WARN",
            "ERROR",
            "FATAL"};

    Logger *Logger::m_instance = nullptr;

    Logger::Logger() : m_level(Level::DEBUG), m_max(0), m_len(0) {}

    Logger::~Logger() { close(); }

    Logger *Logger::instance() {
        if (m_instance == nullptr) { m_instance = new Logger(); }
        return m_instance;
    }

    void Logger::open(const std::string &filename) {
        m_filename = filename;
        m_ofs.open(filename, std::ios::app);
        if (m_ofs.fail()) { throw std::runtime_error("open file " + filename + " failed"); }

        m_ofs.seekp(0, std::ios::end);
        m_len = m_ofs.tellp();
    }

    void Logger::close() { m_ofs.close(); }

    void Logger::log(Level level, const char *file, int line, const char *format, ...) {
        if (m_level > level) { return; }

        if (m_ofs.fail()) { throw std::runtime_error("open file " + m_filename + " failed"); }

        // 获取当前时间戳
        time_t now = time(NULL);
        struct tm *info = localtime(&now);
        char timestamp[32];
        memset(timestamp, 0, sizeof(timestamp));
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", info);

        // 将时间戳等信息以指定格式写入缓冲区
        const char *fmt = "[%s] [%s] %s:%d ";
        int size = snprintf(NULL, 0, fmt, timestamp, m_clevel[level], file, line);
        if (size > 0) {
            char *buffer = new char[size + 1];
            snprintf(buffer, size + 1, fmt, timestamp, m_clevel[level], file, line);
            m_ofs << buffer;
            delete[] buffer;
            m_len += size;
        }

        // 将log中的可变参数按相应格式写入缓冲区
        va_list vl;
        va_start(vl, format);
        size = vsnprintf(NULL, 0, format, vl);
        va_end(vl);
        if (size > 0) {
            char *buffer = new char[size + 1];
            va_start(vl, format);
            vsnprintf(buffer, size + 1, format, vl);
            va_end(vl);
            m_ofs << buffer;
            delete[] buffer;
            m_len += size;
        }

        // 从缓冲区写入文件
        m_ofs << "\n";
        m_ofs.flush();

        if (m_len >= m_max && m_max > 0) { rotate(); }
    }

    void Logger::rotate() {
        close();

        time_t ticks = time(NULL);
        struct tm *info = localtime(&ticks);
        char timestamp[32];
        memset(timestamp, 0, sizeof(timestamp));
        strftime(timestamp, sizeof(timestamp), ".%Y-%m-%d_%H-%M-%S", info);

        std::string filename = m_filename + timestamp;
        if (rename(m_filename.c_str(), filename.c_str()) != 0) { throw std::logic_error("rename log file failed: " + std::string(strerror(errno))); }

        open(m_filename);
    }
}// namespace utils
