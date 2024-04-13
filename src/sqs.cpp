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
  mtx.lock();
  std::string qurl = new_queue_url(input->get_queue_name());
  std::map<std::string, std::string> attrs = input->get_attrs();
  queues[qurl] = std::deque<Message>();
  queue_attrs[qurl] = attrs;
  mtx.unlock();
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
  mtx.lock();
  if (queues.find(qurl) == queues.end()) {
    mtx.unlock();
    return false;
  }

  queues.erase(qurl);
  mtx.unlock();
  return true;
}

bool SQS::tag_queue(std::string qurl,
                    std::map<std::string, std::string>* tags) {
  mtx.lock();
  if (queues.find(qurl) == queues.end()) {
    mtx.unlock();
    return false;
  }
  if (queue_tags.find(qurl) == queue_tags.end()) {
    queue_tags[qurl] = std::map<std::string, std::string>();
  }
  for (const auto& tag : *tags) {
    queue_tags[qurl][tag.first] = tag.second;
  }
  mtx.unlock();
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
  mtx.lock();
  if (queues.find(qurl) == queues.end()) {
    mtx.unlock();
    return false;
  }
  if (queue_tags.find(qurl) == queue_tags.end()) {
    mtx.unlock();
    return true;
  }
  for (const auto& key : *tag_keys) {
    queue_tags[qurl].erase(key);
  }
  mtx.unlock();
  return true;
}

bool SQS::send_message(SendMessageInput* msg) {
  mtx.lock();
  auto queue = queues.find(msg->get_queue_url());
  if (queue == queues.end()) {
    mtx.unlock();
    return false;
  }

  Message m;
  auto id = uuid_generator();
  m.message_id = boost::lexical_cast<std::string>(id);
  m.body = msg->get_message_body();
  m.md5_of_body = "md5";
  m.visible_at = 0;
  queue->second.push_back(m);
  mtx.unlock();
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
  mtx.lock();
  auto queue = queues.find(qurl);
  if (queue == queues.end()) {
    mtx.unlock();
    return false;
  }

  queue->second.clear();
  mtx.unlock();
  return true;
}

std::vector<Message> SQS::receive(std::string qurl, int count) {
  mtx.lock();
  auto queue = queues.find(qurl);
  if (queue == queues.end() || queue->second.empty()) {
    mtx.unlock();
    return {};
  }

  std::vector<Message> messages;
  auto total = 0;
  auto ts = now();

  for (Message& msg : queue->second) {
    if (msg.visible_at <= ts) {
      msg.visible_at = ts + 30;
      messages.push_back(msg);
      total++;
    }

    if (total == count) {
      break;
    }
  }
  mtx.unlock();
  return messages;
}

long SQS::now() { return std::time(nullptr); }
}  // namespace sqscpp
