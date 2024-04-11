#include "sqs.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <ctime>

namespace sqscpp {
SQS::SQS(std::string ep) {
  endpoint = ep;
  queues = std::map<std::string, std::deque<Message>>();
  queue_attrs = std::map<std::string, std::map<std::string, std::string>>();
}

std::string SQS::create_queue(CreateQueueInput* input) {
  std::string qurl = new_queue_url(input->get_queue_name());
  std::map<std::string, std::string> attrs = input->get_attrs();
  queues[qurl] = std::deque<Message>();
  queue_attrs[qurl] = attrs;
  return qurl;
}

std::string SQS::new_queue_url(std::string qname) {
  std::stringstream ss;
  ss << endpoint << "/" << qname;
  return ss.str();
}

std::unique_ptr<std::vector<std::string>> SQS::get_queue_urls() {
  std::vector<std::string> urls;

  for (const auto q : queues) {
    urls.push_back(q.first);
  }

  return std::make_unique<std::vector<std::string>>(urls);
}

std::optional<std::string> SQS::get_queue_url(std::string qname) {
  auto qurl = new_queue_url(qname);
  if (queues.find(qurl) == queues.end()) {
    return {};
  }
  return qurl;
}

bool SQS::delete_queue(std::string qurl) {
  if (queues.find(qurl) == queues.end()) {
    return false;
  }

  queues.erase(qurl);
  return true;
}

bool SQS::tag_queue(std::string qurl,
                    std::map<std::string, std::string>* tags) {
  if (queues.find(qurl) == queues.end()) {
    return false;
  }
  if (queue_tags.find(qurl) == queue_tags.end()) {
    queue_tags[qurl] = std::map<std::string, std::string>();
  }
  for (const auto& tag : *tags) {
    queue_tags[qurl][tag.first] = tag.second;
  }
  return true;
}

std::optional<std::unique_ptr<std::map<std::string, std::string>>>
SQS::get_queue_tags(std::string qurl) {
  if (queues.find(qurl) == queues.end()) {
    return {};
  }
  return std::make_unique<std::map<std::string, std::string>>(queue_tags[qurl]);
}

bool SQS::untag_queue(std::string qurl, std::vector<std::string>* tag_keys) {
  if (queues.find(qurl) == queues.end()) {
    return false;
  }
  if (queue_tags.find(qurl) == queue_tags.end()) {
    return true;
  }
  for (const auto& key : *tag_keys) {
    queue_tags[qurl].erase(key);
  }
  return true;
}

bool SQS::send_message(SendMessageInput* msg) {
  auto queue = queues.find(msg->get_queue_url());
  if (queue == queues.end()) {
    return false;
  }

  Message m;
  auto id = uuid_generator();
  m.message_id = boost::lexical_cast<std::string>(id);
  m.body = msg->get_message_body();
  m.md5_of_body = "md5";
  m.visible_at = 0;
  queue->second.push_back(m);
  return true;
}

int SQS::get_message_count(std::string& qurl) {
  auto queue = queues.find(qurl);
  if (queue == queues.end()) {
    return -1;
  }

  return queue->second.size();
}

bool SQS::purge_queue(std::string qurl) {
  auto queue = queues.find(qurl);
  if (queue == queues.end()) {
    return false;
  }

  queue->second.clear();
  return true;
}

std::vector<Message> SQS::receive(std::string qurl, int count) {
  auto queue = queues.find(qurl);
  if (queue == queues.end() || queue->second.empty()) {
    return {};
  }

  std::vector<Message> messages;
  auto total = 0;
  auto ts = now();

  while (!queue->second.empty() && queue->second.front().visible_at <= ts) {
    queue->second.front().visible_at = ts + 30;
    messages.push_back(queue->second.front());
  }

  return messages;
}

long SQS::now() { return std::time(nullptr); }
}  // namespace sqscpp
