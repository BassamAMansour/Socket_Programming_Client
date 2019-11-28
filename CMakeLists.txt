cmake_minimum_required(VERSION 3.15)
project(Socket_Programming_Client)

set(CMAKE_CXX_STANDARD 17)

add_executable(Socket_Programming_Client main.cpp RequestProcessor.cpp RequestProcessor.h StringUtils.cpp StringUtils.h)