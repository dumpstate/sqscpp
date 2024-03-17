#ifndef SQSCPP_CLI_ARGS_H
#define SQSCPP_CLI_ARGS_H

#include <string>

namespace sqscpp {
const int DEFAULT_PORT = 8080;
const std::string DEFAULT_HOST = "0.0.0.0";
const std::string DEFAULT_ACCOUNT_NUMBER = "000000000000";

struct CliArgs {
  int port;
  std::string host;
  std::string account_number;
};

std::pair<bool, CliArgs> parse_cli_args(int argc, char *argv[]);
std::string endpoint_url(CliArgs *args);
}  // namespace sqscpp

#endif  // SQSCPP_CLI_ARGS_H
