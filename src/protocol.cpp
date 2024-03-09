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
}  // namespace sqscpp
