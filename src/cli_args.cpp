#include <iostream>
#include "boost/program_options.hpp"

namespace po = boost::program_options;

namespace sqscpp {
    struct CliArgs {
        int port;
        std::string host;
    };

    std::pair<bool, CliArgs> parse_cli_args(int argc, char* argv[]) {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "print help message")
            ("host", po::value<std::string>(), "target hostname")
            ("port", po::value<int>(), "target port");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return std::pair<bool, CliArgs>(false, CliArgs());
        }

        int target_port = 8080;
        if (vm.count("port")) {
            target_port = vm["port"].as<int>();
        }

        std::string host = "0.0.0.0";
        if (vm.count("host")) {
            host = vm["host"].as<std::string>();
        }

        return std::pair<bool, CliArgs>(true, CliArgs(target_port, host));
    }
}
