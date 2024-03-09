#ifndef SQSCPP_ROUTER_H
#define SQSCPP_ROUTER_H

#include <restinio/core.hpp>

namespace sqscpp {
const std::string AWS_JSON_PROTOCOL_1_0 = "application/x-amz-json-1.0";

enum AWSProtocol { AWSQueryProtocol, AWSJsonProtocol1_0 };

restinio::request_handling_status_t handler(restinio::request_handle_t req);
AWSProtocol parse_protocol(restinio::http_request_header_t* headers);
std::string to_str(AWSProtocol protocol);
}  // namespace sqscpp

#endif  // SQSCPP_ROUTER_H
