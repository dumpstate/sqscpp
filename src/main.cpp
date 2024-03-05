#include "boost/program_options.hpp"
#include <iostream>
#include <restinio/core.hpp>

namespace po = boost::program_options;

auto main(int argc, char* argv[]) -> int {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "print help message")
        ("port", po::value<int>(), "target port");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    int target_port = 8080;
    if (vm.count("port")) {
        target_port = vm["port"].as<int>();
        if (target_port <= 0) {
            std::cerr << "invalid port: " << target_port << "\n";
            return 1;
        }
    }

    std::cout << "Starting sqscpp 0.0.0.0:" << target_port << "\n";

    restinio::run(
        restinio::on_this_thread()
        .port(target_port)
        .address("0.0.0.0")
        .request_handler([](auto req) {
            return req->create_response().set_body("Yellow!\n").done();
        }));
}
