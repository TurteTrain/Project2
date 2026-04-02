cmake_minimum_required(VERSION 3.20)
project(Project2)

set(CMAKE_CXX_STANDARD 17)

add_executable(main
        cpp/main.cpp
        cpp/parser.cpp
)