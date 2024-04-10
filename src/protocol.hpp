#ifndef SQSCPP_PROTOCOL_H
#define SQSCPP_PROTOCOL_H

#include <restinio/core.hpp>
#include <string>

namespace sqscpp {
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
                   std::optional<std::map<std::string, std::string>> attrs) {
    queue_name = qname;
    if (attrs.has_value()) {
      attributes = attrs.value();
    }
  }

  std::string &get_queue_name() { return queue_name; }
  std::map<std::string, std::string> &get_attrs() { return attributes; }
};

class GetQueueUrlInput {
 private:
  std::string queue_name;

 public:
  GetQueueUrlInput(std::string qname) : queue_name(qname) {}
  std::string &get_queue_name() { return queue_name; }
};

class DeleteQueueInput {
 private:
  std::string queue_url;

 public:
  DeleteQueueInput(std::string qurl) : queue_url(qurl){};
  std::string &get_queue_url() { return queue_url; }
};

class TagQueueInput {
 private:
  std::string queue_url;
  std::map<std::string, std::string> tags;

 public:
  TagQueueInput(std::string qurl, std::map<std::string, std::string> tags)
      : queue_url(qurl), tags(tags) {}
  std::string &get_queue_url() { return queue_url; }
  std::map<std::string, std::string> &get_tags() { return tags; }
};

class ListQueueTagsInput {
 private:
  std::string queue_url;

 public:
  ListQueueTagsInput(std::string qurl) : queue_url(qurl) {}
  std::string &get_queue_url() { return queue_url; }
};

class UntagQueueInput {
 private:
  std::string queue_url;
  std::vector<std::string> tag_keys;

 public:
  UntagQueueInput(std::string qurl, std::vector<std::string> keys)
      : queue_url(qurl), tag_keys(keys) {}
  std::string &get_queue_url() { return queue_url; }
  std::vector<std::string> &get_tag_keys() { return tag_keys; }
};

class SendMessageInput {
 private:
  std::string queue_url;
  std::string message_body;
  std::optional<long> delay_seconds;
  std::optional<std::string> message_deduplication_id;

 public:
  SendMessageInput(std::string qurl, std::string body,
                   std::optional<long> delay,
                   std::optional<std::string> deduplication_id)
      : queue_url(qurl),
        message_body(body),
        delay_seconds(delay),
        message_deduplication_id(deduplication_id) {}
  std::string &get_queue_url() { return queue_url; }
  std::string &get_message_body() { return message_body; }
  std::optional<long> &get_delay_seconds() { return delay_seconds; }
  std::optional<std::string> &get_message_deduplication_id() {
    return message_deduplication_id;
  }
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
  std::vector<std::string> *queue_urls;
  int message_count = 0;
};

struct GetQueueUrlResponse {
  std::string queue_url;
};

struct ListQueueTagsResponse {
  std::map<std::string, std::string> *tags;
};

}  // namespace sqscpp

#endif  // SQSCPP_PROTOCOL_H
