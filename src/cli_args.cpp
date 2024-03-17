#include "cli_args.hpp"

#include <iostream>

#include "boost/program_options.hpp"

namespace po = boost::program_options;

namespace sqscpp {
std::pair<bool, CliArgs> parse_cli_args(int argc, char *argv[]) {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "print help message")(
      "host", po::value<std::string>(), "target hostname")(
      "port", po::value<int>(), "target port")(
      "account_number", po::value<std::string>(), "AWS account number");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.contains("help")) {
    std::cout << desc << "\n";
    return std::pair<bool, CliArgs>(false, CliArgs());
  }

  int target_port = DEFAULT_PORT;
  if (vm.contains("port")) {
    target_port = vm["port"].as<int>();
  }

  std::string host = DEFAULT_HOST;
  if (vm.contains("host")) {
    host = vm["host"].as<std::string>();
  }

  std::string account_number = DEFAULT_ACCOUNT_NUMBER;
  if (vm.contains("account_number")) {
    account_number = vm["account_number"].as<std::string>();
  }

  return std::pair<bool, CliArgs>(true,
                                  CliArgs{target_port, host, account_number});
}

std::string endpoint_url(CliArgs *args) {
  std::stringstream ss;
  ss << "http://" << args->host << ":" << args->port << "/"
     << args->account_number;
  return ss.str();
}
}  // namespace sqscpp
