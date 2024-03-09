#ifndef SQSCPP_PROTOCOL_H
#define SQSCPP_PROTOCOL_H

#include <restinio/core.hpp>
#include <string>

namespace sqscpp {
struct Error {
  restinio::http_status_line_t status;
  std::string message;
};

struct BadRequestError : Error {
  BadRequestError(std::string msg) {
    status = restinio::status_bad_request();
    message = msg;
  }
};

std::string to_json(Error* err);
}  // namespace sqscpp

#endif  // SQSCPP_PROTOCOL_H
