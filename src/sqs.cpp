#include "sqs.hpp"

#include <openssl/evp.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cstdio>
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

std::string SQS::get_queue_name(std::string& qurl) {
  auto pos = qurl.find_last_of('/');
  return qurl.substr(pos + 1);
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

std::unique_ptr<SendMessageResponse> SQS::send_message(SendMessageInput* msg) {
  mtx.lock();
  auto queue = queues.find(msg->get_queue_url());
  if (queue == queues.end()) {
    mtx.unlock();
    return nullptr;
  }

  Message m;
  auto id = uuid_generator();
  m.message_id = boost::lexical_cast<std::string>(id);
  m.body = msg->get_message_body();
  m.md5_of_body = md5(m.body);
  m.visible_at = 0;
  queue->second.push_back(m);
  mtx.unlock();
  return std::make_unique<SendMessageResponse>(m.message_id, m.md5_of_body);
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

bool SQS::delete_message(DeleteMessageInput* input) {
  mtx.lock();
  auto queue = queues.find(input->get_queue_url());
  if (queue == queues.end()) {
    mtx.unlock();
    return false;
  }

  auto& msgs = queue->second;
  for (auto it = msgs.begin(); it != msgs.end(); it++) {
    if (it->message_id == input->get_receipt_handle()) {
      msgs.erase(it);
      mtx.unlock();
      return true;
    }
  }
  mtx.unlock();
  return false;
}

std::unique_ptr<FullQueueDataResponse> SQS::get_queue_data(std::string qname) {
  auto qurl = new_queue_url(qname);
  auto queue = queues.find(qurl);
  if (queue == queues.end()) {
    return nullptr;
  }

  auto info = FullQueueDataResponse();
  info.queue_name = qname;
  info.queue_url = qurl;
  for (const auto& msg : queue->second) {
    ReceivedMessageResponse res_msg;
    res_msg.message_id = msg.message_id;
    res_msg.receipt_handle = msg.message_id;
    res_msg.md5_of_body = msg.md5_of_body;
    res_msg.body = msg.body;
    info.messages.push_back(res_msg);
  }
  info.tags = queue_tags[qurl];
  info.attributes = queue_attrs[qurl];
  return std::make_unique<FullQueueDataResponse>(info);
}

long SQS::now() { return std::time(nullptr); }

std::string SQS::md5(std::string& content) {
  // copied over from stack overflow
  EVP_MD_CTX* context = EVP_MD_CTX_new();
  const EVP_MD* md = EVP_md5();
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len;
  std::string output;

  EVP_DigestInit_ex2(context, md, NULL);
  EVP_DigestUpdate(context, content.c_str(), content.length());
  EVP_DigestFinal_ex(context, md_value, &md_len);
  EVP_MD_CTX_free(context);

  output.resize(md_len * 2);
  for (unsigned int i = 0; i < md_len; ++i)
    std::sprintf(&output[i * 2], "%02x", md_value[i]);
  return output;
}
}  // namespace sqscpp
