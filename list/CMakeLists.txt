cmake_minimum_required(VERSION 2.8)

add_executable(stest single_list.cpp)
target_link_libraries(stest)

add_executable(dtest double_list.cpp)
target_link_libraries(dtest)
