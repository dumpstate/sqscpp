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

std::optional<long> JsonSerde::parse_long(json j) {
  if (j == nullptr || !j.is_number_integer()) return {};
  try {
    return j;
  } catch (json::type_error& e) {
    return {};
  }
}

std::optional<int> JsonSerde::parse_int(json j) {
  if (j == nullptr || !j.is_number_integer()) return {};
  try {
    return j;
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
  std::vector<std::string> urls;
  for (const auto& info : *(res->queue_urls)) {
    urls.push_back(info.queue_url);
  }
  j["QueueUrls"] = urls;
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

std::string JsonSerde::serialize(ReceivedMessageResponse* res) {
  json j;
  j["MessageId"] = res->message_id;
  j["ReceiptHandle"] = res->receipt_handle;
  j["MD5OfBody"] = res->md5_of_body;
  j["Body"] = res->body;
  return j.dump();
}

std::string JsonSerde::serialize(ReceivedMessagesResponse* res) {
  json j;
  std::vector<json> messages;
  for (const auto& msg : res->messages) {
    json m;
    m["MessageId"] = msg.message_id;
    m["ReceiptHandle"] = msg.receipt_handle;
    m["MD5OfBody"] = msg.md5_of_body;
    m["Body"] = msg.body;
    messages.push_back(m);
  }
  j["Messages"] = messages;
  return j.dump();
}

std::string JsonSerde::serialize(SendMessageResponse* res) {
  json j;
  j["MessageId"] = res->message_id;
  j["MD5OfMessageBody"] = res->md5_of_message_body;
  return j.dump();
}

std::optional<std::unique_ptr<CreateQueueInput>>
JsonSerde::deserialize_create_queue_input(std::string& str) {
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
JsonSerde::deserialize_get_queue_url_input(std::string& str) {
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
JsonSerde::deserialize_delete_queue_input(std::string& str) {
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
JsonSerde::deserialize_tag_queue_input(std::string& str) {
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
JsonSerde::deserialize_list_queue_tags_input(std::string& str) {
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
JsonSerde::deserialize_untag_queue_input(std::string& str) {
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

std::optional<std::unique_ptr<SendMessageInput>>
JsonSerde::deserialize_send_message_input(std::string& str) {
  try {
    json j = json::parse(str);

    auto qurl = parse_non_empty_string(j["QueueUrl"]);
    if (!qurl.has_value()) return {};

    auto msg = parse_non_empty_string(j["MessageBody"]);
    if (!msg.has_value()) return {};

    return std::make_unique<SendMessageInput>(
        qurl.value(), msg.value(), parse_long(j["DelaySeconds"]),
        parse_non_empty_string(j["MessageDeduplicationId"]));
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<std::unique_ptr<PurgeQueueInput>>
JsonSerde::deserialize_purge_queue_input(std::string& str) {
  try {
    json j = json::parse(str);

    auto qurl = parse_non_empty_string(j["QueueUrl"]);
    if (!qurl.has_value()) return {};

    return std::make_unique<PurgeQueueInput>(qurl.value());
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<std::unique_ptr<ReceiveMessageInput>>
JsonSerde::deserialize_receive_message_input(std::string& str) {
  try {
    json j = json::parse(str);

    auto qurl = parse_non_empty_string(j["QueueUrl"]);
    if (!qurl.has_value()) return {};

    return std::make_unique<ReceiveMessageInput>(
        qurl.value(), parse_int(j["MaxNumberOfMessages"]),
        parse_non_empty_string(j["ReceiveRequestAttemptId"]),
        parse_int(j["VisibilityTimeout"]), parse_long(j["WaitTimeSeconds"]));
  } catch (json::parse_error& e) {
    return {};
  }
}

std::optional<std::unique_ptr<DeleteMessageInput>>
JsonSerde::deserialize_delete_message_input(std::string& str) {
  try {
    json j = json::parse(str);

    auto qurl = parse_non_empty_string(j["QueueUrl"]);
    if (!qurl.has_value()) return {};

    auto receipt_handle = parse_non_empty_string(j["ReceiptHandle"]);
    if (!receipt_handle.has_value()) return {};

    return std::make_unique<DeleteMessageInput>(qurl.value(),
                                                receipt_handle.value());
  } catch (json::parse_error& e) {
    return {};
  }
}

std::string HtmlSerde::render_html(std::string& body) {
  std::stringstream ss;
  ss << "<!DOCTYPE html>";
  ss << "<html>";
  ss << "<head>";
  ss << "<title>sqscpp</title>";
  ss << "<link rel=\"stylesheet\" "
        "href=\"https://cdn.jsdelivr.net/npm/bulma@1.0.0/css/bulma.min.css\">";
  ss << "</head><body>";
  ss << "<div class=\"container\">";
  ss << body << "</div></body></html>";
  return ss.str();
}

std::string HtmlSerde::serialize(Error* err) {
  std::stringstream ss;
  ss << "<h1 class=\"title\">Error</h1><p>" << err->message << "</p>";
  auto body = ss.str();
  return render_html(body);
}

std::string HtmlSerde::serialize(CreateQueueResponse* res) {
  throw std::runtime_error("not implemented");
}

std::string HtmlSerde::serialize(ListQueuesResponse* res) {
  std::stringstream ss;
  ss << "<h1 class=\"title\">Queues</h1>";
  if (res->queue_urls->empty()) {
    ss << "<p>No queues found.</p>";
  } else {
    ss << "<table class=\"table is-fullwidth\">";
    ss << "<thead>";
    ss << "<tr>";
    ss << "<th>Queue URL</th>";
    ss << "<th>Message Count</th>";
    ss << "</tr>";
    ss << "</thead>";
    ss << "<tbody>";
    for (const auto& info : *(res->queue_urls)) {
      ss << "<tr>";
      ss << "<td>";
      ss << "<a href=\"/queues/" << info.queue_name << "\">";
      ss << info.queue_url << "</a></td>";
      ss << "<td>" << info.message_count << "</td>";
      ss << "</tr>";
    }
    ss << "</tbody>";
    ss << "</table>";
  }
  auto body = ss.str();
  return render_html(body);
}

std::string HtmlSerde::serialize(GetQueueUrlResponse* res) {
  throw std::runtime_error("not implemented");
}

std::string HtmlSerde::serialize(ListQueueTagsResponse* res) {
  throw std::runtime_error("not implemented");
}

std::string HtmlSerde::serialize(ReceivedMessageResponse* res) {
  throw std::runtime_error("not implemented");
}

std::string HtmlSerde::serialize(FullQueueDataResponse* res) {
  std::stringstream ss;
  ss << "<h1 class=\"title\">Queue Details</h1>";
  ss << "<h1 class=\"title is-4\">Actions</h1>";
  ss << "<a class=\"button is-danger\" href=\"/queues/" << res->queue_name
     << "/purge\">Purge Queue</a>";
  ss << "<h1 class=\"title is-4\">Attributes</h1>";
  ss << "<table class=\"table is-fullwidth\">";
  ss << "<tbody>";
  ss << "<tr><td>Queue Name</td><td>" << res->queue_name << "</td></tr>";
  ss << "<tr><td>Queue URL</td><td>" << res->queue_url << "</td></tr>";
  ss << "<tr><td>Message Count</td><td>" << res->messages.size()
     << "</td></tr>";
  for (const auto& [key, value] : res->attributes) {
    ss << "<tr><td>" << key << "</td><td>" << value << "</td></tr>";
  }
  if (!res->tags.empty()) {
    ss << "<tr><td>Tags</td><td>";
    for (const auto& [key, value] : res->tags) {
      ss << "<span class=\"tag\">" << key << ":" << value << "</span>";
    }
    ss << "</td></tr>";
  }
  ss << "</tbody>";
  ss << "</table>";
  ss << "<h1 class=\"title is-4\">Messages</h1>";
  ss << "<table class=\"table is-fullwidth\">";
  ss << "<thead>";
  ss << "<tr>";
  ss << "<th>Message ID</th>";
  ss << "<th>Body</th>";
  ss << "</tr>";
  ss << "</thead>";
  ss << "<tbody>";
  for (const auto& msg : res->messages) {
    ss << "<tr>";
    ss << "<td>" << msg.message_id << "</td>";
    ss << "<td><pre>" << msg.body << "</pre></td>";
    ss << "</tr>";
  }
  ss << "</table>";

  auto body = ss.str();
  return render_html(body);
}
}  // namespace sqscpp
