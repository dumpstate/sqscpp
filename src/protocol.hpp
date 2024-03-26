#ifndef SQSCPP_PROTOCOL_H
#define SQSCPP_PROTOCOL_H

#include <nlohmann/json.hpp>
#include <restinio/core.hpp>
#include <string>

using json = nlohmann::json;

namespace sqscpp {
std::optional<std::map<std::string, std::string>> parse_dict(json j);
std::optional<std::string> parse_non_empty_string(json j);

struct Error {
  restinio::http_status_line_t status;
  std::string message;
};

class CreateQueueInput {
 private:
  std::string queue_name;
  std::map<std::string, std::string> attributes;

 public:
  CreateQueueInput(std::string qname,
                   std::optional<std::map<std::string, std::string>> attrs);

  std::string get_queue_name() { return queue_name; }
  std::map<std::string, std::string>* get_attrs() { return &attributes; }

  static std::optional<CreateQueueInput> from_str(std::string str);
};

class GetQueueUrlInput {
 private:
  std::string queue_name;

 public:
  GetQueueUrlInput(std::string qname) : queue_name(qname) {}
  std::string get_queue_name() { return queue_name; }
  static std::optional<GetQueueUrlInput> from_str(std::string str);
};

class DeleteQueueInput {
 private:
  std::string queue_url;

 public:
  DeleteQueueInput(std::string qurl) : queue_url(qurl){};
  std::string get_queue_url() { return queue_url; }

  static std::optional<DeleteQueueInput> from_str(std::string str);
};

class TagQueueInput {
 private:
  std::string queue_url;
  std::map<std::string, std::string> tags;

 public:
  TagQueueInput(std::string qurl, std::map<std::string, std::string> tags)
      : queue_url(qurl), tags(tags) {}
  std::string get_queue_url() { return queue_url; }
  std::map<std::string, std::string> get_tags() { return tags; }

  static std::optional<TagQueueInput> from_str(std::string str);
};

class ListQueueTagsInput {
 private:
  std::string queue_url;

 public:
  ListQueueTagsInput(std::string qurl) : queue_url(qurl) {}
  std::string get_queue_url() { return queue_url; }

  static std::optional<ListQueueTagsInput> from_str(std::string str);
};

struct BadRequestError : Error {
  BadRequestError(std::string msg) {
    status = restinio::status_bad_request();
    message = msg;
  }
};

struct CreateQueueResponse {
  std::string queue_url;
};

struct ListQueuesResponse {
  std::vector<std::string> queue_urls;
};

struct GetQueueUrlResponse {
  std::string queue_url;
};

struct ListQueueTagsResponse {
  std::map<std::string, std::string>* tags;
};

std::string to_json(Error* err);
std::string to_json(CreateQueueResponse* res);
std::string to_json(ListQueuesResponse* res);
std::string to_json(GetQueueUrlResponse* res);
std::string to_json(ListQueueTagsResponse* res);
}  // namespace sqscpp

#endif  // SQSCPP_PROTOCOL_H
