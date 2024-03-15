#include <iostream>
#include <restinio/core.hpp>

#include "cli_args.hpp"
#include "router.hpp"

auto main(int argc, char *argv[]) -> int {
  auto args_res = sqscpp::parse_cli_args(argc, argv);
  if (!args_res.first) return EXIT_FAILURE;
  auto args = args_res.second;

  std::cout << "Starting sqscpp " << args.host << ":" << args.port << std::endl;

  using namespace std::chrono;

  try {
    using traits_t =
        restinio::traits_t<restinio::asio_timer_manager_t,
                           restinio::single_threaded_ostream_logger_t>;

    restinio::run(restinio::on_this_thread<traits_t>()
                      .port(args.port)
                      .address(args.host)
                      .request_handler(sqscpp::handler));
  } catch (const std::exception &ex) {
    std::cerr << "ERR: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
