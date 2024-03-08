#include <iostream>
#include <restinio/core.hpp>

#include "cli_args.hpp"

auto main(int argc, char *argv[]) -> int {
  auto args_res = sqscpp::parse_cli_args(argc, argv);
  if (!args_res.first) return EXIT_FAILURE;
  auto args = args_res.second;

  std::cout << "Starting sqscpp " << args.host << ":" << args.port << "\n";

  restinio::run(
      restinio::on_this_thread()
          .port(args.port)
          .address(args.host)
          .request_handler([](auto req) {
            return req->create_response().set_body("Yellow!\n").done();
          }));
}
