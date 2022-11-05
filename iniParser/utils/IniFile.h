//
// Created by mrxovo on 9/19/22.
//

#ifndef INIPARSER_INIFILE_H
#define INIPARSER_INIFILE_H

#include "./Value.h"

#include <map>
#include <string>

using namespace utils;

using Parameter = std::map<std::string, Value>;

namespace utils {

    class IniFile {
    public:
        IniFile() = default;
        IniFile(const char *filename);
        IniFile(const std::string &filename);
        ~IniFile() = default;

        bool load(const std::string &filename);
        bool save(const std::string &filename);
        Value &get(const std::string &section, const std::string &key);
        void set(const std::string &section, const std::string &key, const Value &value);
        bool has(const std::string &section);
        bool has(const std::string &section, const std::string &key);
        void remove(const std::string &section, const std::string &key);
        void remove(const std::string &section);
        void clear();
        void show();

        Parameter &operator[](const std::string &section);

    private:
        std::string m_filename;
        std::map<std::string, Parameter> m_sections;

    private:
        std::string str();
    };
}// namespace utils

#endif//INIPARSER_INIFILE_H
