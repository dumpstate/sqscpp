#ifndef SQSCPP_SQS_H
#define SQSCPP_SQS_H

#include <map>
#include <string>
#include <vector>

#include "protocol.hpp"

namespace sqscpp {
class SQS {
 private:
  std::map<std::string, std::vector<std::string>> queues;
  std::map<std::string, std::map<std::string, std::string>> queue_attrs;

 public:
  SQS();
  void create_queue(CreateQueueInput* input);
};
}  // namespace sqscpp

#endif  // SQSCPP_SQS_H
