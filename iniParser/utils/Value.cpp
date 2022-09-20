//
// Created by mrxovo on 9/19/22.
//

#include "Value.h"

namespace utils {

    Value::Value(bool v) { *this = v; }

    Value::Value(int v) { *this = v; }

    Value::Value(double v) { *this = v; }

    Value::Value(const char *v) { *this = v; }

    Value::Value(std::string v) { *this = v; }

    Value &Value::operator=(bool v) {
        m_value = v ? "true" : "false";
        return *this;
    }

    Value &Value::operator=(int v) {
        m_value = std::to_string(v);
        return *this;
    }

    Value &Value::operator=(double v) {
        m_value = std::to_string(v);
        return *this;
    }

    Value &Value::operator=(const char *v) {
        m_value = v;
        return *this;
    }

    Value &Value::operator=(const std::string &v) {
        m_value = v;
        return *this;
    }

    Value::operator bool() const { return m_value == "true"; }

    Value::operator int() const { return std::stoi(m_value); }

    Value::operator double() const { return std::stod(m_value); }

    Value::operator std::string() const { return m_value; }

    std::ostream &operator<<(std::ostream &out, const Value &value) {
        out << std::string(value);
        return out;
    }
}// namespace utils