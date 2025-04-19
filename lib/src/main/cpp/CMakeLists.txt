cmake_minimum_required(VERSION 3.22.1)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        module.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries  link to the target library
        android
        log)