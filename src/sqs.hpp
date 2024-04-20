#ifndef SQSCPP_SQS_H
#define SQSCPP_SQS_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "protocol.hpp"

namespace sqscpp {
struct Message {
  std::string message_id;
  std::string md5_of_body;
  std::string body;
  long visible_at;
};

class SQS {
 private:
  boost::uuids::random_generator uuid_generator;
  std::string endpoint;
  std::map<std::string, std::deque<Message>> queues;
  std::map<std::string, std::map<std::string, std::string>> queue_attrs;
  std::map<std::string, std::map<std::string, std::string>> queue_tags;

  std::string new_queue_url(std::string qname);
  std::string md5(std::string& data);
  long now();
  std::mutex mtx;

 public:
  SQS(std::string ep);
  std::string create_queue(CreateQueueInput* input);
  bool delete_queue(std::string qurl);
  std::unique_ptr<std::vector<std::string>> get_queue_urls();
  std::optional<std::string> get_queue_url(std::string qname);
  bool tag_queue(std::string qurl, std::map<std::string, std::string>* tags);
  std::optional<std::unique_ptr<std::map<std::string, std::string>>>
  get_queue_tags(std::string qurl);
  bool untag_queue(std::string qurl, std::vector<std::string>* tag_keys);
  std::unique_ptr<SendMessageResponse> send_message(SendMessageInput* input);
  int get_message_count(std::string& qurl);
  bool purge_queue(std::string qurl);
  std::vector<Message> receive(std::string qurl, int count);
  bool delete_message(DeleteMessageInput* input);
  std::string get_queue_name(std::string& qurl);
  std::unique_ptr<FullQueueDataResponse> get_queue_data(std::string qname);
};
}  // namespace sqscpp

#endif  // SQSCPP_SQS_H
