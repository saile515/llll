cmake_minimum_required(VERSION 3.20)

project("llll"
        VERSION "0.1.0"
        LANGUAGES CXX)

add_executable(llll src/main.cpp src/lexer.cpp src/lexer.hpp)
