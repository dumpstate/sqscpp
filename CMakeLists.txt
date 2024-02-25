cmake_minimum_required(VERSION 3.25)
project(sqscpp)

set(CMAKE_CXX_STANDARD 20)

find_package(restinio REQUIRED)

add_executable(sqscpp src/main.cpp)
target_link_libraries(sqscpp PRIVATE restinio::restinio)
