cmake_minimum_required(VERSION 3.4)
project(untitled)

#set(_extra_warnings -Wpointer-arith)
#string (REPLACE ";" " " _extra_warnings "${_extra_warnings}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Wpointer-arith")

set(SOURCE_FILES main.cpp)
add_executable(main ${SOURCE_FILES})