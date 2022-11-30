cmake_minimum_required(VERSION 3.25)
project(mystl)

set(CMAKE_CXX_STANDARD 14)

add_executable(mystl main.cpp Vector.h test/vector_test.h)
