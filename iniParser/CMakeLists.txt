cmake_minimum_required(VERSION 3.0)
project(iniParser)

set(CMAKE_CXX_STANDARD 11)

add_executable(iniParser main.cpp utils/IniFile.cpp utils/IniFile.h utils/Value.cpp utils/Value.h)
