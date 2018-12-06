cmake_minimum_required(VERSION 3.0)

add_library(global SHARED
    global.cpp
)

add_executable(inheritance inheritance.cpp)
target_link_libraries(inheritance global)

add_executable(template template.cpp)
target_link_libraries(template global)

add_executable(virtual_func virtual_func.cpp)
target_link_libraries(virtual_func global)

add_executable(exception exception.cpp)
target_link_libraries(exception global)

add_executable(smart_pointer smart_pointer.cpp)
target_link_libraries(smart_pointer global)

add_executable(std_string std_string.cpp)
target_link_libraries(std_string global)

# matches AppleClang and Clang
if (CMAKE_COMPILER_IS_CLANGXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    message(STATUS "optional:-std=c++11")
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()
