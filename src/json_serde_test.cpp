#include <gtest/gtest.h>

#include "serde.hpp"

using namespace sqscpp;

TEST(json_serde_test, error_serialize) {
  JsonSerde serde;
  Error err;
  err.message = "test message";
  auto res = serde.serialize(&err);

  EXPECT_EQ(res, "{\"Message\":\"test message\"}");
}

TEST(json_serde_test, parse_dict) {
  JsonSerde serde;
  auto j = json::parse("{\"key\": \"value\"}");
  auto res = serde.parse_dict(j);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value().at("key"), "value");
}

TEST(json_serde_test, parse_dict_empty) {
  JsonSerde serde;
  auto j = json::parse("[]");
  auto res = serde.parse_dict(j);

  EXPECT_EQ(res.has_value(), false);
}

TEST(json_serde_test, parse_non_empty_string) {
  JsonSerde serde;
  auto j = json::parse("\"test\"");
  auto res = serde.parse_non_empty_string(j);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value(), "test");
}

TEST(json_serde_test, parse_non_empty_string_empty) {
  JsonSerde serde;
  auto j = json::parse("\"\"");
  auto res = serde.parse_non_empty_string(j);

  EXPECT_EQ(res.has_value(), false);
}

TEST(json_serde_test, create_queue_input_from_str) {
  JsonSerde serde;
  std::string input = "{\"QueueName\":\"test-queue\",\"Attributes\":{\"DelaySeconds\":\"5\"}}";
  auto res = serde.deserialize_create_queue_input(input);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value()->get_queue_name(), "test-queue");
  EXPECT_EQ(res.value()->get_attrs().at("DelaySeconds"), "5");
}

TEST(json_serde_test, create_queue_input_from_str_no_attrs) {
  JsonSerde serde;
  std::string input = "{\"QueueName\":\"test-queue\"}";
  auto res = serde.deserialize_create_queue_input(input);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value()->get_queue_name(), "test-queue");
  EXPECT_EQ(res.value()->get_attrs().size(), 0);
}

TEST(json_serde_test, create_queue_input_from_str_empty) {
  JsonSerde serde;
  std::string input = "{}";
  auto res = serde.deserialize_create_queue_input(input);

  EXPECT_EQ(res.has_value(), false);
}

TEST(json_serde_test, create_queue_input_from_str_no_queue_name) {
  JsonSerde serde;
  std::string input = "{\"Attributes\":{\"DelaySeconds\":\"5\"}}";
  auto res = serde.deserialize_create_queue_input(input);

  EXPECT_EQ(res.has_value(), false);
}

TEST(json_serde_test, create_queue_response_to_str) {
  JsonSerde serde;
  CreateQueueResponse res;
  res.queue_url = "http://localhost:9999/test-queue";
  auto str = serde.serialize(&res);

  EXPECT_EQ(str, "{\"QueueUrl\":\"http://localhost:9999/test-queue\"}");
}

TEST(json_serde_test, list_queues_response_to_str) {
  JsonSerde serde;
  ListQueuesResponse res;
  std::vector<QueueInfo> qs = {QueueInfo{"foo", 0}, QueueInfo{"bar", 1}};
  res.queue_urls = &qs;
  auto str = serde.serialize(&res);

  EXPECT_EQ(str, "{\"QueueUrls\":[\"foo\",\"bar\"]}");
}

TEST(json_serde_test, delete_queue_input_from_str) {
  JsonSerde serde;
  std::string input = "{\"QueueUrl\":\"test-url\"}";
  auto res = serde.deserialize_delete_queue_input(input);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value()->get_queue_url(), "test-url");
}

TEST(json_serde_test, get_queue_url_from_str) {
  JsonSerde serde;
  std::string input = "{\"QueueName\":\"test-queue\"}";
  auto res = serde.deserialize_get_queue_url_input(input);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value()->get_queue_name(), "test-queue");
}

TEST(json_serde_test, get_queue_url_response_to_str) {
  JsonSerde serde;
  GetQueueUrlResponse res;
  res.queue_url = "http://localhost:9999/test-queue";
  auto str = serde.serialize(&res);

  EXPECT_EQ(str, "{\"QueueUrl\":\"http://localhost:9999/test-queue\"}");
}

TEST(json_serde_test, tag_queue_input_from_str) {
  JsonSerde serde;
  std::string input =
      "{\"QueueUrl\":\"test-url\",\"Tags\":{\"key\":\"value\"}}";
  auto res = serde.deserialize_tag_queue_input(input);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value()->get_queue_url(), "test-url");
  EXPECT_EQ(res.value()->get_tags().at("key"), "value");
}

TEST(json_serde_test, list_queue_tags_input_from_str) {
  JsonSerde serde;
  std::string input = "{\"QueueUrl\":\"test-url\"}";
  auto res = serde.deserialize_list_queue_tags_input(input);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value()->get_queue_url(), "test-url");
}

TEST(json_serde_test, list_queue_tags_response_to_str) {
  JsonSerde serde;
  ListQueueTagsResponse res;
  std::map<std::string, std::string> tags = {{"key", "value"}};
  res.tags = &tags;
  auto str = serde.serialize(&res);

  EXPECT_EQ(str, "{\"Tags\":{\"key\":\"value\"}}");
}

TEST(json_serde_test, untag_queue_input_from_str) {
  JsonSerde serde;
  std::string input = "{\"QueueUrl\":\"test-url\",\"TagKeys\":[\"key\"]}";
  auto res = serde.deserialize_untag_queue_input(input);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value()->get_queue_url(), "test-url");
  EXPECT_EQ(res.value()->get_tag_keys().at(0), "key");
}

TEST(json_serde_test, purge_queue_input_from_str) {
  JsonSerde serde;
  std::string input = "{\"QueueUrl\":\"test-url\"}";
  auto res = serde.deserialize_purge_queue_input(input);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value()->get_queue_url(), "test-url");
}

TEST(json_serde_test, receive_message_input_from_str) {
  JsonSerde serde;
  std::string input = "{\"QueueUrl\":\"test-url\",\"MaxNumberOfMessages\":1}";
  auto res = serde.deserialize_receive_message_input(input);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value()->get_queue_url(), "test-url");
  EXPECT_EQ(res.value()->get_max_number_of_messages(), 1);
}

TEST(json_serde_test, received_message_response_to_str) {
  JsonSerde serde;
  ReceivedMessageResponse res;
  res.message_id = "test-id";
  res.body = "test-body";
  res.md5_of_body = "test-md5";
  res.receipt_handle = "test-handle";
  auto str = serde.serialize(&res);

  EXPECT_EQ(str,
            "{\"Body\":\"test-body\",\"MD5OfBody\":\"test-md5\","
            "\"MessageId\":\"test-id\",\"ReceiptHandle\":\"test-handle\"}");
}

TEST(json_serde_test, delete_message_input_from_str) {
  JsonSerde serde;
  std::string input =
      "{\"QueueUrl\":\"test-url\",\"ReceiptHandle\":\"test-handle\"}";
  auto res = serde.deserialize_delete_message_input(input);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value()->get_queue_url(), "test-url");
  EXPECT_EQ(res.value()->get_receipt_handle(), "test-handle");
}
