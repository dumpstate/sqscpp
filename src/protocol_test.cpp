#include "protocol.hpp"

#include <gtest/gtest.h>

using namespace sqscpp;

TEST(protocol_test, to_json) {
  auto err = new Error();
  err->message = "test message";
  auto res = to_json(err);

  EXPECT_EQ(res, "{\"Message\":\"test message\"}");
}

TEST(protocol_test, parse_dict) {
  auto j = json::parse("{\"key\": \"value\"}");
  auto res = parse_dict(j);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value().at("key"), "value");
}

TEST(protocol_test, parse_dict_empty) {
  auto j = json::parse("[]");
  auto res = parse_dict(j);

  EXPECT_EQ(res.has_value(), false);
}

TEST(protocol_test, parse_non_empty_string) {
  auto j = json::parse("\"test\"");
  auto res = parse_non_empty_string(j);

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value(), "test");
}

TEST(protocol_test, parse_non_empty_string_empty) {
  auto j = json::parse("\"\"");
  auto res = parse_non_empty_string(j);

  EXPECT_EQ(res.has_value(), false);
}

TEST(protocol_test, create_queue_input_from_str) {
  auto res = CreateQueueInput::from_str(
      "{\"QueueName\":\"test-queue\",\"Attributes\":{\"DelaySeconds\":\"5\"}}");

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value().get_queue_name(), "test-queue");
  EXPECT_EQ(res.value().get_attrs()->at("DelaySeconds"), "5");
}

TEST(protocol_test, create_queue_input_from_str_no_attrs) {
  auto res = CreateQueueInput::from_str("{\"QueueName\":\"test-queue\"}");

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value().get_queue_name(), "test-queue");
  EXPECT_EQ(res.value().get_attrs()->size(), 0);
}

TEST(protocol_test, create_queue_input_from_str_empty) {
  auto res = CreateQueueInput::from_str("{}");

  EXPECT_EQ(res.has_value(), false);
}

TEST(protocol_test, create_queue_input_from_str_no_queue_name) {
  auto res =
      CreateQueueInput::from_str("{\"Attributes\":{\"DelaySeconds\":\"5\"}}");

  EXPECT_EQ(res.has_value(), false);
}

TEST(protocol_test, create_queue_response_to_str) {
  auto res = new CreateQueueResponse();
  res->queue_url = "http://localhost:9999/test-queue";
  auto str = to_json(res);

  EXPECT_EQ(str, "{\"QueueUrl\":\"http://localhost:9999/test-queue\"}");
}

TEST(protocol_test, list_queues_response_to_str) {
  auto res = new ListQueuesResponse();
  std::vector<std::string> qs = {"foo", "bar"};
  res->queue_urls = qs;
  auto str = to_json(res);

  EXPECT_EQ(str, "{\"QueueUrls\":[\"foo\",\"bar\"]}");
}

TEST(protocol_test, delete_queue_input_from_str) {
  auto res = DeleteQueueInput::from_str("{\"QueueUrl\":\"test-url\"}");

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value().get_queue_url(), "test-url");
}

TEST(protocol_test, get_queue_url_from_str) {
  auto res = GetQueueUrlInput::from_str("{\"QueueName\":\"test-queue\"}");

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value().get_queue_name(), "test-queue");
}

TEST(protocol_test, get_queue_url_response_to_str) {
  auto res = new GetQueueUrlResponse();
  res->queue_url = "http://localhost:9999/test-queue";
  auto str = to_json(res);

  EXPECT_EQ(str, "{\"QueueUrl\":\"http://localhost:9999/test-queue\"}");
}

TEST(protocol_test, tag_queue_input_from_str) {
  auto res = TagQueueInput::from_str(
      "{\"QueueUrl\":\"test-url\",\"Tags\":{\"key\":\"value\"}}");

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value().get_queue_url(), "test-url");
  EXPECT_EQ(res.value().get_tags().at("key"), "value");
}

TEST(protocol_test, list_queue_tags_input_from_str) {
  auto res = ListQueueTagsInput::from_str("{\"QueueUrl\":\"test-url\"}");

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value().get_queue_url(), "test-url");
}

TEST(protocol_test, list_queue_tags_response_to_str) {
  auto res = new ListQueueTagsResponse();
  std::map<std::string, std::string> tags = {{"key", "value"}};
  res->tags = &tags;
  auto str = to_json(res);

  EXPECT_EQ(str, "{\"Tags\":{\"key\":\"value\"}}");
}

TEST(protocol_test, untag_queue_input_from_str) {
  auto res = UntagQueueInput::from_str(
      "{\"QueueUrl\":\"test-url\",\"TagKeys\":[\"key\"]}");

  EXPECT_EQ(res.has_value(), true);
  EXPECT_EQ(res.value().get_queue_url(), "test-url");
  EXPECT_EQ(res.value().get_tag_keys().at(0), "key");
}
