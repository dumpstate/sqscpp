#include "router.hpp"

#include "protocol.hpp"

namespace sqscpp {
std::function<restinio::request_handling_status_t(restinio::request_handle_t)>
handler_factory(SQS* sqs) {
  return [sqs](restinio::request_handle_t req) {
    auto headers = req->header();
    auto protocol = extract_protocol(&headers);

    if (protocol == AWSJsonProtocol1_0) {
      return aws_json_handler(sqs, &headers, req);
    }

    return aws_query_handler(sqs, &headers, req);
  };
}

restinio::request_handling_status_t aws_json_handler(
    SQS* sqs, restinio::http_request_header_t* headers,
    restinio::request_handle_t req) {
  auto trace_id = extract_trace_id(headers).value_or("");
  auto action = extract_action(headers);

  if (!action.has_value()) {
    return resp_err(req, BadRequestError(AWS_TARGET + " header not found"));
  }

  switch (action.value()) {
    case SQSCreateQueue: {
      auto body = CreateQueueInput::from_str(req->body());
      if (!body.has_value()) {
        return resp_err(req, BadRequestError("invalid request body"));
      }
      auto qurl = sqs->create_queue(&body.value());
      auto res = CreateQueueResponse{qurl};
      return resp_ok(req, to_json(&res));
    }
    case SQSListQueues: {
      auto qurls = sqs->get_queue_urls();
      auto res = ListQueuesResponse{qurls};
      return resp_ok(req, to_json(&res));
    }
    default:
      return resp_err(req, Error(restinio::status_not_implemented(),
                                 "action not implemented"));
  }
}

restinio::request_handling_status_t resp_ok(restinio::request_handle_t req,
                                            std::string body) {
  return req->create_response().set_body(std::move(body)).done();
}

restinio::request_handling_status_t resp_err(restinio::request_handle_t req,
                                             Error err) {
  return req->create_response(err.status).set_body(to_json(&err)).done();
}

restinio::request_handling_status_t aws_query_handler(
    SQS* sqs, restinio::http_request_header_t* headers,
    restinio::request_handle_t req) {
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
