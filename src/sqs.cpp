#include "sqs.hpp"

namespace sqscpp {
SQS::SQS(std::string ep) {
  endpoint = ep;
  queues = std::map<std::string, std::vector<std::string>>();
  queue_attrs = std::map<std::string, std::map<std::string, std::string>>();
}

std::string SQS::create_queue(CreateQueueInput* input) {
  std::string qurl = get_queue_url(input->get_queue_name());
  std::map<std::string, std::string>* attrs = input->get_attrs();
  queues[qurl] = std::vector<std::string>();
  queue_attrs[qurl] = *attrs;
  return qurl;
}

std::string SQS::get_queue_url(std::string qname) {
  std::stringstream ss;
  ss << endpoint << "/" << qname;
  return ss.str();
}
}  // namespace sqscpp
