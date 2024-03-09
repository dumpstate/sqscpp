#include "router.hpp"

namespace sqscpp {
restinio::request_handling_status_t handler(restinio::request_handle_t req) {
  auto headers = req->header();
  auto protocol = parse_protocol(&headers);

  req->create_response()
      .set_body("AAA(protocol: " + to_str(protocol) + ")")
      .done();
  return restinio::request_accepted();
}

AWSProtocol parse_protocol(restinio::http_request_header_t* headers) {
  auto content_type = headers->opt_value_of(restinio::http_field::content_type);
  if (content_type.has_value() &&
      content_type.value() == AWS_JSON_PROTOCOL_1_0) {
    return AWSJsonProtocol1_0;
  }

  return AWSQueryProtocol;
}

std::string to_str(AWSProtocol protocol) {
  switch (protocol) {
    case AWSQueryProtocol:
      return "AWSQueryProtocol";
    case AWSJsonProtocol1_0:
      return "AWSJsonProtocol1.0";
  }
}
}  // namespace sqscpp
