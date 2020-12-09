cmake_minimum_required(VERSION 3.0.0)
project(opencl VERSION 0.1.0)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCL_TARGET_OPENCL_VERSION=200")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCL_TARGET_OPENCL_VERSION=200")

include(CTest)
enable_testing()

include_directories(${PROJECT_SOURCE_DIR}/kernels)
include_directories(${PROJECT_SOURCE_DIR}/utils)

file(GLOB UTILS_SOURCES "utils/*.c")

add_executable(opencl main.c ${UTILS_SOURCES})
TARGET_LINK_LIBRARIES(opencl OpenCL)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# MACRO
macro(copy_files GLOBPAT DESTINATION)
  file(GLOB COPY_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLOBPAT})
  add_custom_target(copy ALL
    COMMENT "Copying files: ${GLOBPAT}")

  foreach(FILENAME ${COPY_FILES})
    set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}")
    set(DST "${DESTINATION}/${FILENAME}")

    add_custom_command(
      TARGET copy
      COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DST}
      )
  endforeach(FILENAME)
endmacro(copy_files)
copy_files(kernels/* .)