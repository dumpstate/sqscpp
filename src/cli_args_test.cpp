#include <gtest/gtest.h>
#include "cli_args.hpp"

using namespace sqscpp;

std::vector<char*> as_argv(std::vector<std::string> args) {
    std::vector<char*> argv;
    for (const auto& arg : args)
        argv.push_back((char*)arg.data());
    argv.push_back(nullptr);
    return argv;
}

TEST(cli_args_test, parse_cli_args_parse_help) {
    auto argv = as_argv({"sqscpp", "--help"});
    auto res = parse_cli_args(argv.size() - 1, argv.data());

    EXPECT_EQ(res.first, false);
}

TEST(cli_args_test, parse_cli_args_parse_port) {
    auto argv = as_argv({"sqscpp", "--port", "9999"});
    auto res = parse_cli_args(argv.size() - 1, argv.data());

    EXPECT_EQ(res.first, true);
    EXPECT_EQ(res.second.port, 9999);
    EXPECT_EQ(res.second.host, "0.0.0.0");
}

TEST(cli_args_test, parse_cli_args_parse_host) {
    auto argv = as_argv({"sqscpp", "--host", "localhost"});
    auto res = parse_cli_args(argv.size() - 1, argv.data());

    EXPECT_EQ(res.first, true);
    EXPECT_EQ(res.second.host, "localhost");
    EXPECT_EQ(res.second.port, 8080);
}

TEST(cli_args_test, parse_cli_args_parse_host_and_port) {
    auto argv = as_argv({"sqscpp", "--host", "localhost", "--port", "9999"});
    auto res = parse_cli_args(argv.size() - 1, argv.data());

    EXPECT_EQ(res.first, true);
    EXPECT_EQ(res.second.host, "localhost");
    EXPECT_EQ(res.second.port, 9999);
}
