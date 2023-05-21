cmake_minimum_required(VERSION 3.25)
project(Baby)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(Baby main.cpp SystemInfo.cpp)
