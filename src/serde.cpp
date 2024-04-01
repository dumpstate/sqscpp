#include "serde.hpp"

namespace sqscpp {
std::optional<std::map<std::string, std::string>> JsonSerde::parse_dict(
    json j) {
  if (!j.is_object()) return {};
  try {
    return j;
  } catch (json::type_error& e) {
    return {};
  }
}

std::optional<std::vector<std::string>> JsonSerde::parse_list(json j) {
  if (!j.is_array()) return {};
  try {
    return j;
  } catch (json::type_error& e) {
    return {};
  }
}

std::optional<std::string> JsonSerde::parse_non_empty_string(json j) {
  if (j == nullptr || !j.is_string()) return {};
  try {
    std::string str = j;
    if (str.empty()) return {};
    return str;
  } catch (json::type_error& e) {
    return {};
  }
}

std::string JsonSerde::serialize(Error* err) {
  json j;
  j["Message"] = err->message;
  return j.dump();
}

std::string JsonSerde::serialize(CreateQueueResponse* res) {
  json j;
  j["QueueUrl"] = res->queue_url;
  return j.dump();
}

std::string JsonSerde::serialize(ListQueuesResponse* res) {
  json j;
  j["QueueUrls"] = *(res->queue_urls);
  return j.dump();
}

std::string JsonSerde::serialize(GetQueueUrlResponse* res) {
  json j;
  j["QueueUrl"] = res->queue_url;
  return j.dump();
}

std::string JsonSerde::serialize(ListQueueTagsResponse* res) {
  json j;
  j["Tags"] = *(res->tags);
  return j.dump();
}

std::optional<std::unique_ptr<CreateQueueInput>>
JsonSerde::deserialize_create_queue_input(std::string str) {
  try {
    json j = json::parse(str);

    auto qname = parse_non_empty_string(j["QueueName"]);
    if (!qname.has_value()) return {};

    auto attrs = parse_dict(j["Attributes"]);
    return std::make_unique<CreateQueueInput>(qname.value(), attrs);
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<std::unique_ptr<GetQueueUrlInput>>
JsonSerde::deserialize_get_queue_url_input(std::string str) {
  try {
    json j = json::parse(str);

    auto qname = parse_non_empty_string(j["QueueName"]);
    if (!qname.has_value()) return {};

    return std::make_unique<GetQueueUrlInput>(qname.value());
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<std::unique_ptr<DeleteQueueInput>>
JsonSerde::deserialize_delete_queue_input(std::string str) {
  try {
    json j = json::parse(str);

    auto qurl = parse_non_empty_string(j["QueueUrl"]);
    if (!qurl.has_value()) return {};

    return std::make_unique<DeleteQueueInput>(qurl.value());
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<std::unique_ptr<TagQueueInput>>
JsonSerde::deserialize_tag_queue_input(std::string str) {
  try {
    json j = json::parse(str);

    auto qurl = parse_non_empty_string(j["QueueUrl"]);
    if (!qurl.has_value()) return {};

    auto tags = parse_dict(j["Tags"]);
    if (!tags.has_value()) return {};

    return std::make_unique<TagQueueInput>(qurl.value(), tags.value());
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<std::unique_ptr<ListQueueTagsInput>>
JsonSerde::deserialize_list_queue_tags_input(std::string str) {
  try {
    json j = json::parse(str);

    auto qurl = parse_non_empty_string(j["QueueUrl"]);
    if (!qurl.has_value()) return {};

    return std::make_unique<ListQueueTagsInput>(qurl.value());
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<std::unique_ptr<UntagQueueInput>>
JsonSerde::deserialize_untag_queue_input(std::string str) {
  try {
    json j = json::parse(str);

    auto qurl = parse_non_empty_string(j["QueueUrl"]);
    if (!qurl.has_value()) return {};

    auto tags = parse_list(j["TagKeys"]);
    if (!tags.has_value()) return {};

    return std::make_unique<UntagQueueInput>(qurl.value(), tags.value());
  } catch (json::parse_error& e) {
    return {};
  }
}

}  // namespace sqscpp