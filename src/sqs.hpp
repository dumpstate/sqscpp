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

  std::string get_queue_url(std::string qname);

 public:
  SQS(std::string ep);
  std::string create_queue(CreateQueueInput* input);
  std::vector<std::string> get_queue_urls();
};
}  // namespace sqscpp

#endif  // SQSCPP_SQS_H
