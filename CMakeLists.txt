cmake_minimum_required(VERSION 3.20)

project("llll"
        VERSION "0.1.0"
        LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

add_executable(llll src/main.cpp src/lexer.hpp src/parser.hpp)
