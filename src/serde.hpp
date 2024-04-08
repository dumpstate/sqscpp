#ifndef SQSCPP_SERDE_H
#define SQSCPP_SERDE_H

#include <nlohmann/json.hpp>
#include <string>

#include "protocol.hpp"

using json = nlohmann::json;

namespace sqscpp {
class Serde {
 public:
  virtual std::string contentType() = 0;
  virtual std::string serialize(Error *err) = 0;
  virtual std::string serialize(CreateQueueResponse *res) = 0;
  virtual std::string serialize(ListQueuesResponse *res) = 0;
  virtual std::string serialize(GetQueueUrlResponse *res) = 0;
  virtual std::string serialize(ListQueueTagsResponse *res) = 0;

  virtual std::optional<std::unique_ptr<CreateQueueInput>>
  deserialize_create_queue_input(std::string str) = 0;
  virtual std::optional<std::unique_ptr<GetQueueUrlInput>>
  deserialize_get_queue_url_input(std::string str) = 0;
  virtual std::optional<std::unique_ptr<DeleteQueueInput>>
  deserialize_delete_queue_input(std::string str) = 0;
  virtual std::optional<std::unique_ptr<TagQueueInput>>
  deserialize_tag_queue_input(std::string str) = 0;
  virtual std::optional<std::unique_ptr<ListQueueTagsInput>>
  deserialize_list_queue_tags_input(std::string str) = 0;
  virtual std::optional<std::unique_ptr<UntagQueueInput>>
  deserialize_untag_queue_input(std::string str) = 0;
  virtual std::optional<std::unique_ptr<SendMessageInput>>
  deserialize_send_message_input(std::string str) = 0;
};

class JsonSerde : public Serde {
 public:
  std::optional<std::map<std::string, std::string>> parse_dict(json j);
  std::optional<std::vector<std::string>> parse_list(json j);
  std::optional<std::string> parse_non_empty_string(json j);
  std::optional<long> parse_long(json j);

  std::string contentType() override { return "application/json"; }

  std::string serialize(Error *err) override;
  std::string serialize(CreateQueueResponse *res) override;
  std::string serialize(ListQueuesResponse *res) override;
  std::string serialize(GetQueueUrlResponse *res) override;
  std::string serialize(ListQueueTagsResponse *res) override;

  std::optional<std::unique_ptr<CreateQueueInput>>
  deserialize_create_queue_input(std::string str) override;
  std::optional<std::unique_ptr<GetQueueUrlInput>>
  deserialize_get_queue_url_input(std::string str) override;
  std::optional<std::unique_ptr<DeleteQueueInput>>
  deserialize_delete_queue_input(std::string str) override;
  std::optional<std::unique_ptr<TagQueueInput>> deserialize_tag_queue_input(
      std::string str) override;
  std::optional<std::unique_ptr<ListQueueTagsInput>>
  deserialize_list_queue_tags_input(std::string str) override;
  std::optional<std::unique_ptr<UntagQueueInput>> deserialize_untag_queue_input(
      std::string str) override;
  std::optional<std::unique_ptr<SendMessageInput>>
  deserialize_send_message_input(std::string str) override;
};

class HtmlSerde : public Serde {
 public:
  std::string contentType() override { return "text/html"; }

  std::string serialize(Error *err) override;
  std::string serialize(CreateQueueResponse *res) override;
  std::string serialize(ListQueuesResponse *res) override;
  std::string serialize(GetQueueUrlResponse *res) override;
  std::string serialize(ListQueueTagsResponse *res) override;

  std::optional<std::unique_ptr<CreateQueueInput>>
  deserialize_create_queue_input(std::string str) override;
  std::optional<std::unique_ptr<GetQueueUrlInput>>
  deserialize_get_queue_url_input(std::string str) override;
  std::optional<std::unique_ptr<DeleteQueueInput>>
  deserialize_delete_queue_input(std::string str) override;
  std::optional<std::unique_ptr<TagQueueInput>> deserialize_tag_queue_input(
      std::string str) override;
  std::optional<std::unique_ptr<ListQueueTagsInput>>
  deserialize_list_queue_tags_input(std::string str) override;
  std::optional<std::unique_ptr<UntagQueueInput>> deserialize_untag_queue_input(
      std::string str) override;
  std::optional<std::unique_ptr<SendMessageInput>>
  deserialize_send_message_input(std::string str) override;
};
}  // namespace sqscpp

#endif  // SQSCPP_SERDE_H
