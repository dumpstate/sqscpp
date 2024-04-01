cmake_minimum_required(VERSION 3.25)
project(sqscpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# fetching googletest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(restinio REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost 1.84.0 COMPONENTS program_options)

set(SOURCES src/cli_args.hpp src/cli_args.cpp src/router.hpp src/router.cpp src/protocol.hpp src/serde.hpp src/serde.cpp src/sqs.hpp src/sqs.cpp)
add_executable(sqscpp src/main.cpp ${SOURCES})
target_include_directories(sqscpp PRIVATE src)
target_link_libraries(sqscpp PRIVATE restinio::restinio)
target_link_libraries(sqscpp PRIVATE Boost::program_options)
target_link_libraries(sqscpp PRIVATE nlohmann_json::nlohmann_json)

# registering unit tests
enable_testing()
add_executable(sqscpp_test src/json_serde_test.cpp src/cli_args_test.cpp src/cli_args.hpp src/cli_args.cpp src/protocol.hpp src/serde.hpp src/serde.cpp)
target_link_libraries(sqscpp_test GTest::gtest_main)
target_link_libraries(sqscpp_test Boost::program_options)
target_link_libraries(sqscpp PRIVATE nlohmann_json::nlohmann_json)

include(GoogleTest)
gtest_discover_tests(sqscpp_test)
