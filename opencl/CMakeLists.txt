cmake_minimum_required(VERSION 3.0.0)
project(opencl VERSION 0.1.0)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCL_TARGET_OPENCL_VERSION=200")

include(CTest)
enable_testing()

add_executable(opencl main.cpp)

TARGET_LINK_LIBRARIES(opencl OpenCL)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
