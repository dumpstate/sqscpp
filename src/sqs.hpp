#ifndef SQSCPP_SQS_H
#define SQSCPP_SQS_H

#include <map>
#include <string>
#include <vector>

#include "protocol.hpp"

namespace sqscpp {
class SQS {
 private:
  std::string endpoint;
  std::map<std::string, std::vector<std::string>> queues;
  std::map<std::string, std::map<std::string, std::string>> queue_attrs;
  std::map<std::string, std::map<std::string, std::string>> queue_tags;

  std::string new_queue_url(std::string qname);

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
  bool send_message(SendMessageInput* input);
  int get_message_count(std::string& qurl);
  bool purge_queue(std::string qurl);
};
}  // namespace sqscpp

#endif  // SQSCPP_SQS_H
