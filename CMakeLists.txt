cmake_minimum_required(VERSION 3.17)
project(956213)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(bin)
include_directories(datasets)
include_directories(tests)
add_executable(956213
        area.cpp
        areas.cpp
        bethyw.cpp
        input.cpp
        measure.cpp
        tests/test11.cpp
        bin/catch.o)