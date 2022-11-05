//
// Created by mrxovo on 11/4/22.
//

#ifndef EZLOGSYS_LOGGER_H
#define EZLOGSYS_LOGGER_H

#include <fstream>
#include <string>

namespace utils {

#define debug(format, ...) \
    Logger::instance()->log(Logger::DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define info(format, ...) \
    Logger::instance()->log(Logger::INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define warn(format, ...) \
    Logger::instance()->log(Logger::WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define error(format, ...) \
    Logger::instance()->log(Logger::ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define fatal(format, ...) \
    Logger::instance()->log(Logger::FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)

    class Logger {
    public:
        enum Level {// 日志级别
            DEBUG = 0,
            INFO,
            WARN,
            ERROR,
            FATAL,
            LEVEL_COUNT
        };
        static Logger *instance();
        void open(const std::string &filename);
        void close();
        void log(Level level, const char *file, int line, const char *format, ...);// 写日志
        inline void level(Level level) { m_level = level; }
        inline void max(int bytes) { m_max = bytes; }

    private:
        Logger();
        ~Logger();
        void rotate();

    private:
        std::string m_filename;
        std::ofstream m_ofs;
        Level m_level;
        int m_max;
        int m_len;
        static const char *m_clevel[LEVEL_COUNT];// 日志级别相对应的字符串
        static Logger *m_instance;
    };
}// namespace utils
#endif//EZLOGSYS_LOGGER_H
