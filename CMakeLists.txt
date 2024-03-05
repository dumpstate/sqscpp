cmake_minimum_required(VERSION 3.25)
project(sqscpp)

set(CMAKE_CXX_STANDARD 20)

find_package(restinio REQUIRED)
find_package(Boost 1.84.0 COMPONENTS program_options)

add_executable(sqscpp src/main.cpp)
target_link_libraries(sqscpp PRIVATE restinio::restinio)
target_link_libraries(sqscpp PRIVATE Boost::program_options)
