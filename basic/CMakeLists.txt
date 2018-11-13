cmake_minimum_required(VERSION 3.0)

add_executable(inheritance inheritance.cpp)

# matches AppleClang and Clang
if (CMAKE_COMPILER_IS_CLANGXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    message(STATUS "optional:-std=c++11")
endif()
