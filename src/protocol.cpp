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

std::string to_json(CreateQueueResponse* res) {
  json j;
  j["QueueUrl"] = res->queue_url;
  return j.dump();
}

std::string to_json(ListQueuesResponse* res) {
  json j;
  j["QueueUrls"] = res->queue_urls;
  return j.dump();
}

std::string to_json(GetQueueUrlResponse* res) {
  json j;
  j["QueueUrl"] = res->queue_url;
  return j.dump();
}

std::string to_json(ListQueueTagsResponse* res) {
  json j;
  j["Tags"] = *(res->tags);
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

std::optional<std::vector<std::string>> parse_list(json j) {
  if (!j.is_array()) return {};
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

CreateQueueInput::CreateQueueInput(
    std::string qname,
    std::optional<std::map<std::string, std::string>> attrs) {
  queue_name = qname;
  if (attrs.has_value()) {
    attributes = attrs.value();
  }
}

std::optional<CreateQueueInput> CreateQueueInput::from_str(std::string str) {
  try {
    auto j = json::parse(str);

    auto queue_name = parse_non_empty_string(j["QueueName"]);
    if (!queue_name.has_value()) return {};

    auto attrs = parse_dict(j["Attributes"]);

    return CreateQueueInput(queue_name.value(), attrs);
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<DeleteQueueInput> DeleteQueueInput::from_str(std::string str) {
  try {
    auto j = json::parse(str);

    auto qurl = parse_non_empty_string(j["QueueUrl"]);
    if (!qurl.has_value()) return {};
    return DeleteQueueInput{qurl.value()};
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<GetQueueUrlInput> GetQueueUrlInput::from_str(std::string str) {
  try {
    auto j = json::parse(str);

    auto qname = parse_non_empty_string(j["QueueName"]);
    if (!qname.has_value()) return {};
    return GetQueueUrlInput{qname.value()};
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<TagQueueInput> TagQueueInput::from_str(std::string str) {
  try {
    auto j = json::parse(str);

    auto qurl = parse_non_empty_string(j["QueueUrl"]);
    if (!qurl.has_value()) return {};

    auto tags = parse_dict(j["Tags"]);
    if (!tags.has_value()) return {};

    return TagQueueInput{qurl.value(), tags.value()};
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<ListQueueTagsInput> ListQueueTagsInput::from_str(
    std::string str) {
  try {
    auto j = json::parse(str);

    auto qurl = parse_non_empty_string(j["QueueUrl"]);
    if (!qurl.has_value()) return {};

    return ListQueueTagsInput{qurl.value()};
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<UntagQueueInput> UntagQueueInput::from_str(std::string str) {
  try {
    auto j = json::parse(str);

    auto qurl = parse_non_empty_string(j["QueueUrl"]);
    if (!qurl.has_value()) return {};

    auto tags = parse_list(j["TagKeys"]);
    if (!tags.has_value()) return {};

    return UntagQueueInput{qurl.value(), tags.value()};
  } catch (json::parse_error& e) {
    return {};
  }
}
}  // namespace sqscpp
