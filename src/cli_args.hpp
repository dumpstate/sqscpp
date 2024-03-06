#ifndef SQSCPP_CLI_ARGS_H
#define SQSCPP_CLI_ARGS_H

namespace sqscpp {
    constexpr int DEFAULT_PORT = 8080;
    constexpr std::string DEFAULT_HOST = "0.0.0.0";

    struct CliArgs {
        int port;
        std::string host;
    };

    std::pair<bool, CliArgs> parse_cli_args(int argc, char* argv[]);
}

#endif // SQSCPP_CLI_ARGS_H
