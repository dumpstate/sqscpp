#include "cli_args.hpp"

#include <gtest/gtest.h>

using namespace sqscpp;

std::vector<char *> as_argv(std::vector<std::string> *args) {
  std::vector<char *> argv;
  for (const auto &arg : *args) {
    argv.push_back((char *)arg.data());
  }
  argv.push_back(nullptr);
  return argv;
}

TEST(cli_args_test, parse_cli_args_parse_help) {
  std::vector<std::string> cmd = {"sqscpp", "--help"};
  auto argv = as_argv(&cmd);
  auto res = parse_cli_args(argv.size() - 1, argv.data());

  EXPECT_EQ(res.first, false);
}

TEST(cli_args_test, parse_cli_args_parse_port) {
  std::vector<std::string> cmd = {"sqscpp", "--port", "9999"};
  auto argv = as_argv(&cmd);
  auto res = parse_cli_args(argv.size() - 1, argv.data());

  EXPECT_EQ(res.first, true);
  EXPECT_EQ(res.second.port, 9999);
  EXPECT_EQ(res.second.host, "0.0.0.0");
  EXPECT_EQ(res.second.account_number, DEFAULT_ACCOUNT_NUMBER);
}

TEST(cli_args_test, parse_cli_args_parse_host) {
  std::vector<std::string> cmd = {"sqscpp", "--host", "localhost"};
  auto argv = as_argv(&cmd);
  auto res = parse_cli_args(argv.size() - 1, argv.data());

  EXPECT_EQ(res.first, true);
  EXPECT_EQ(res.second.host, "localhost");
  EXPECT_EQ(res.second.port, DEFAULT_PORT);
  EXPECT_EQ(res.second.account_number, DEFAULT_ACCOUNT_NUMBER);
}

TEST(cli_args_test, parse_cli_args_parse_host_and_port) {
  std::vector<std::string> cmd = {"sqscpp", "--host", "localhost", "--port",
                                  "9999"};
  auto argv = as_argv(&cmd);
  auto res = parse_cli_args(argv.size() - 1, argv.data());

  EXPECT_EQ(res.first, true);
  EXPECT_EQ(res.second.host, "localhost");
  EXPECT_EQ(res.second.port, 9999);
  EXPECT_EQ(res.second.account_number, DEFAULT_ACCOUNT_NUMBER);
}

TEST(cli_args_test, parse_cli_args_parse_account_number) {
  auto account_number = "123456789";
  std::vector<std::string> cmd = {"sqscpp", "--account-number", account_number};
  auto argv = as_argv(&cmd);
  auto res = parse_cli_args(argv.size() - 1, argv.data());

  EXPECT_EQ(res.first, true);
  EXPECT_EQ(res.second.host, DEFAULT_HOST);
  EXPECT_EQ(res.second.port, DEFAULT_PORT);
  EXPECT_EQ(res.second.account_number, account_number);
}

TEST(cli_args_test, endpoint_url) {
  auto args = CliArgs(9999, "localhost", "123456789012");
  EXPECT_EQ(endpoint_url(&args), "http://localhost:9999/123456789012");
}
