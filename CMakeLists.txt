cmake_minimum_required(VERSION 3.24)
project(Baby)

set(CMAKE_CXX_STANDARD 17)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(Baby main.cpp SystemInfo.cpp UiTable.cpp UiTable.h TableInfo.h)
target_link_libraries(Baby ${CURSES_LIBRARIES})
