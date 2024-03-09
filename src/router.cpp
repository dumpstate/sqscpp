#include "router.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace sqscpp {
restinio::request_handling_status_t handler(restinio::request_handle_t req) {
  auto headers = req->header();
  auto protocol = extract_protocol(&headers);

  if (protocol == AWSJsonProtocol1_0) {
    return aws_json_handler(&headers, req);
  }

  return aws_query_handler(&headers, req);
}

restinio::request_handling_status_t aws_json_handler(
    restinio::http_request_header_t* headers, restinio::request_handle_t req) {
  auto trace_id = extract_trace_id(headers).value_or("");
  auto action = extract_action(headers);

  if (!action.has_value()) {
    json res_body;
    res_body["Message"] = AWS_TARGET + " header not found";
    return req->create_response(restinio::status_bad_request())
        .set_body(res_body.dump())
        .done();
  }

  switch (action.value()) {
    default:
      return restinio::request_rejected();
  }
}

restinio::request_handling_status_t aws_query_handler(
    restinio::http_request_header_t* headers, restinio::request_handle_t req) {
  return restinio::request_rejected();
}

AWSProtocol extract_protocol(restinio::http_request_header_t* headers) {
  auto content_type = headers->opt_value_of(restinio::http_field::content_type);
  if (content_type.has_value() &&
      content_type.value() == AWS_JSON_PROTOCOL_1_0) {
    return AWSJsonProtocol1_0;
  }
  return AWSQueryProtocol;
}

std::optional<std::string> extract_trace_id(
    restinio::http_request_header_t* headers) {
  auto trace_id = headers->opt_value_of(AWS_TRACE_ID);
  if (trace_id.has_value()) {
    return trace_id.value().data();
  }
  return {};
}

std::optional<SQSAction> extract_action(
    restinio::http_request_header_t* headers) {
  auto target = headers->opt_value_of(AWS_TARGET);
  if (target.has_value()) {
    auto action_key = target.value().data();
    try {
      return AWS_SQS_ACTIONS.at(action_key);
    } catch (const std::out_of_range&) {
      return {};
    }
  }
  return {};
}

std::string to_str(AWSProtocol protocol) {
  switch (protocol) {
    case AWSQueryProtocol:
      return "AWSQueryProtocol";
    case AWSJsonProtocol1_0:
      return "AWSJsonProtocol1.0";
    default:
      throw std::exception();
  }
}
}  // namespace sqscpp
