//
// Created by mrxovo on 9/19/22.
//

#ifndef INIPARSER_VALUE_H
#define INIPARSER_VALUE_H

#include <ostream>
#include <string>

namespace utils {

    class Value {
    public:
        Value() = default;
        Value(bool v);
        Value(int v);
        Value(double v);
        Value(const char *v);
        Value(std::string v);
        ~Value() = default;

        Value &operator=(bool v);
        Value &operator=(int v);
        Value &operator=(double v);
        Value &operator=(const char *v);
        Value &operator=(const std::string &v);

        operator bool() const;
        operator int() const;
        operator double() const;
        operator std::string() const;

    private:
        std::string m_value;
    };

    std::ostream &operator<<(std::ostream &out, const Value &value);
}// namespace utils


#endif//INIPARSER_VALUE_H
