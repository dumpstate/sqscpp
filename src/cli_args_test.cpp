#include "cli_args.hpp"

#include <gtest/gtest.h>

using namespace sqscpp;

std::vector<char *> as_argv(std::vector<std::string> args) {
  std::vector<char *> argv;
  for (const auto &arg : args) {
    std::cout << "pushing onto vector: " << arg.data() << std::endl;
    argv.push_back((char *)arg.data());
  }
  argv.push_back(nullptr);
  return argv;
}

// FIXME
// TEST(cli_args_test, parse_cli_args_parse_help) {
//   auto argv = as_argv({"sqscpp", "--help"});
//   auto res = parse_cli_args(argv.size() - 1, argv.data());

//   EXPECT_EQ(res.first, false);
// }

TEST(cli_args_test, parse_cli_args_parse_port) {
  auto argv = as_argv({"sqscpp", "--port", "9999"});
  auto res = parse_cli_args(argv.size() - 1, argv.data());

  EXPECT_EQ(res.first, true);
  EXPECT_EQ(res.second.port, 9999);
  EXPECT_EQ(res.second.host, "0.0.0.0");
  EXPECT_EQ(res.second.account_number, DEFAULT_ACCOUNT_NUMBER);
}

TEST(cli_args_test, parse_cli_args_parse_host) {
  auto argv = as_argv({"sqscpp", "--host", "localhost"});
  auto res = parse_cli_args(argv.size() - 1, argv.data());

  EXPECT_EQ(res.first, true);
  EXPECT_EQ(res.second.host, "localhost");
  EXPECT_EQ(res.second.port, DEFAULT_PORT);
  EXPECT_EQ(res.second.account_number, DEFAULT_ACCOUNT_NUMBER);
}

TEST(cli_args_test, parse_cli_args_parse_host_and_port) {
  auto argv = as_argv({"sqscpp", "--host", "localhost", "--port", "9999"});
  auto res = parse_cli_args(argv.size() - 1, argv.data());

  EXPECT_EQ(res.first, true);
  EXPECT_EQ(res.second.host, "localhost");
  EXPECT_EQ(res.second.port, 9999);
  EXPECT_EQ(res.second.account_number, DEFAULT_ACCOUNT_NUMBER);
}

TEST(cli_args_test, parse_cli_args_parse_account_number) {
  auto acctn = "123456789";
  auto argv = as_argv({"sqscpp", "--acctn", acctn});
  auto res = parse_cli_args(argv.size() - 1, argv.data());

  EXPECT_EQ(res.first, true);
  EXPECT_EQ(res.second.host, DEFAULT_HOST);
  EXPECT_EQ(res.second.port, DEFAULT_PORT);
  EXPECT_EQ(res.second.account_number, acctn);
}

TEST(cli_args_test, endpoint_url) {
  auto args = CliArgs(9999, "localhost", "123456789012");
  EXPECT_EQ(endpoint_url(&args), "http://localhost:9999/123456789012");
}
