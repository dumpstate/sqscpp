#include "protocol.hpp"

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace sqscpp {
std::string to_json(Error* err) {
  json j;
  j["Message"] = err->message;
  return j.dump();
}

std::optional<std::map<std::string, std::string>> parse_dict(json j) {
  if (!j.is_object()) return {};
  try {
    return j;
  } catch (json::type_error& e) {
    return {};
  }
}

std::optional<std::string> parse_non_empty_string(json j) {
  if (j == nullptr || !j.is_string()) return {};
  try {
    std::string str = j;
    if (str.empty()) return {};
    return str;
  } catch (json::type_error& e) {
    return {};
  }
}

std::optional<CreateQueueInput> CreateQueueInput::from_str(std::string str) {
  try {
    auto j = json::parse(str);

    auto queue_name = parse_non_empty_string(j["QueueName"]);
    if (!queue_name.has_value()) return {};

    auto attrs = parse_dict(j["Attributes"]);
    if (!attrs.has_value()) return {};

    return CreateQueueInput(queue_name.value(), attrs.value());
  } catch (json::parse_error& e) {
    return {};
  }
}
}  // namespace sqscpp
