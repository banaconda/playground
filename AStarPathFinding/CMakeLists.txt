cmake_minimum_required(VERSION 3.0.0)
project(AStartPathFinding VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -g")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
set(CMAKE_SHARED_LINKER_FLAGS "")

include_directories(src)

file(GLOB SRC "*.cpp")

add_executable(AStartPathFinding main.cpp ${SRC})