#include "sqs.hpp"

namespace sqscpp {
SQS::SQS() {
  queues = std::map<std::string, std::vector<std::string>>();
  queue_attrs = std::map<std::string, std::map<std::string, std::string>>();
}

void SQS::create_queue(CreateQueueInput* input) {
  std::string qname = input->get_queue_name();
  std::map<std::string, std::string>* attrs = input->get_attrs();
  queues[qname] = std::vector<std::string>();
  queue_attrs[qname] = *attrs;
}
}  // namespace sqscpp
