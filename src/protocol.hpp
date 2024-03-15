#ifndef SQSCPP_PROTOCOL_H
#define SQSCPP_PROTOCOL_H

#include <nlohmann/json.hpp>
#include <restinio/core.hpp>
#include <string>

using json = nlohmann::json;

namespace sqscpp {
struct Error {
  restinio::http_status_line_t status;
  std::string message;
};

class CreateQueueInput {
 private:
  std::string queue_name;

 public:
  CreateQueueInput(std::string qname) { queue_name = qname; }

  std::string get_queue_name() { return queue_name; }

  static std::optional<CreateQueueInput> from_str(std::string str) {
    if (str.empty()) return {};
    auto j = json::parse(str);
    auto queue_name = j["QueueName"];
    if (!queue_name.is_string() || queue_name.empty()) return {};
    return CreateQueueInput(queue_name);
  }
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
