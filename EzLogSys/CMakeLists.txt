cmake_minimum_required(VERSION 3.24)
project(EzLogSys)

set(CMAKE_CXX_STANDARD 14)

add_executable(EzLogSys main.cpp utils/logger.cpp utils/logger.h)
