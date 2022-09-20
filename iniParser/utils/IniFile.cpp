//
// Created by mrxovo on 9/19/22.
//

#include "IniFile.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

namespace utils {

    IniFile::IniFile(const char *filename) { load(filename); }

    IniFile::IniFile(const std::string &filename) { load(filename); }

    bool IniFile::load(const std::string &filename) {
        m_filename = filename;
        m_sections.clear();

        std::ifstream ifs(filename);
        if (ifs.fail()) { return false; }

        std::string line;
        std::string section;
        while (std::getline(ifs, line)) {
            if (line.empty() || '#' == line[0]) { continue; }

            if ('[' == line[0]) {
                auto pos = line.find_first_of(']');
                section = line.substr(1, pos - 1);
                boost::trim(section);
                m_sections[section] = Parameter();
            } else {
                auto pos = line.find_first_of('=');
                auto key = line.substr(0, pos);
                boost::trim(key);
                auto value = line.substr(pos + 1);
                boost::trim(value);
                m_sections[section][key] = value;
            }
        }

        ifs.close();
        return true;
    }

    bool IniFile::save(const std::string &filename) {
        std::ofstream ofs(filename.c_str());
        if (ofs.fail()) { return false; }
        ofs << str();
        ofs.close();
        return true;
    }

    Value &IniFile::get(const std::string &section, const std::string &key) { return m_sections[section][key]; }

    void IniFile::set(const std::string &section, const std::string &key, const Value &value) { m_sections[section][key] = value; }

    bool IniFile::has(const std::string &section) { return (m_sections.find(section) != m_sections.end()); }

    bool IniFile::has(const std::string &section, const std::string &key) {
        auto it = m_sections.find(section);
        if (it != m_sections.end()) {
            return (it->second.find(key) != it->second.end());
        }
        return false;
    }

    void IniFile::remove(const std::string &section, const std::string &key) {
        if (has(section, key)) {
            m_sections[section].erase(key);
        }
    }

    void IniFile::remove(const std::string &section) {
        if (has(section)) {
            m_sections.erase(section);
        }
    }

    void IniFile::clear() { m_sections.clear(); }

    void IniFile::show() {
        std::cout << str();
    }

    Parameter &IniFile::operator[](const std::string &section) { return m_sections[section]; }

    std::string IniFile::str() {
        std::stringstream ss;
        for (auto &&section: m_sections) {
            ss << "[" << section.first << "]\n";
            for (auto &&parameters: m_sections[section.first]) {
                ss << parameters.first << ":" << parameters.second << std::endl;
            }
            ss << std::endl;
        }
        return ss.str();
    }
}// namespace utils
