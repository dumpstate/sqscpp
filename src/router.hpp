#ifndef SQSCPP_ROUTER_H
#define SQSCPP_ROUTER_H

#include <restinio/core.hpp>

namespace sqscpp {
enum AWSProtocol { AWSQueryProtocol, AWSJsonProtocol1_0 };
enum SQSAction { SQSListQueues };

const std::string AWS_JSON_PROTOCOL_1_0 = "application/x-amz-json-1.0";
const std::string AWS_TRACE_ID = "x-amzn-trace-id";
const std::string AWS_TARGET = "x-amz-target";
const std::map<std::string, SQSAction> AWS_SQS_ACTIONS = {
    {"AmazonSQS.ListQueues", SQSListQueues}};

restinio::request_handling_status_t handler(restinio::request_handle_t req);
restinio::request_handling_status_t json_handler(
    restinio::http_request_header_t* headers, restinio::request_handle_t req);
AWSProtocol extract_protocol(restinio::http_request_header_t* headers);
std::optional<SQSAction> extract_action(
    restinio::http_request_header_t* headers);
std::optional<std::string> extract_trace_id(
    restinio::http_request_header_t* headers);
std::string to_str(AWSProtocol protocol);
}  // namespace sqscpp

#endif  // SQSCPP_ROUTER_H
