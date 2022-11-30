cmake_minimum_required(VERSION 3.24)
project(design_pattern)

set(CMAKE_CXX_STANDARD 14)

add_executable(design_pattern main.cpp singleton/singleton.cpp singleton/singleton.h singleton/A.h)
