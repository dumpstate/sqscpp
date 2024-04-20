#include "router.hpp"

#include "protocol.hpp"
#include "serde.hpp"

namespace sqscpp {
std::function<restinio::request_handling_status_t(restinio::request_handle_t)>
handler_factory(SQS* sqs, JsonSerde* json_serde, HtmlSerde* html_serde) {
  return [sqs, json_serde, html_serde](restinio::request_handle_t req) {
    auto headers = req->header();
    auto protocol = extract_protocol(&headers);

    switch (protocol) {
      case AWSJsonProtocol1_0:
        return sqs_query_handler(sqs, json_serde, &headers, req);
      case TextHtml:
        return html_query_handler(sqs, html_serde, &headers, req);
      default:
        return restinio::request_rejected();
    }
  };
}

restinio::request_handling_status_t sqs_query_handler(
    SQS* sqs, Serde* serde, restinio::http_request_header_t* headers,
    restinio::request_handle_t req) {
  auto trace_id = extract_trace_id(headers).value_or("");
  auto action = extract_action(headers);

  if (!action.has_value()) {
    return resp_err(serde, req,
                    BadRequestError(AWS_TARGET + " header not found"));
  }

  switch (action.value()) {
    case SQSCreateQueue: {
      auto body = serde->deserialize_create_queue_input(req->body());
      if (!body.has_value()) {
        return resp_err(serde, req, BadRequestError("invalid request body"));
      }
      auto qurl = sqs->create_queue(body.value().get());
      auto res = CreateQueueResponse{qurl};
      return resp_ok(serde, req, serde->serialize(&res));
    }
    case SQSListQueues: {
      auto qurls = sqs->get_queue_urls();
      std::vector<QueueInfo> queues;
      for (auto& qurl : *qurls) {
        auto count = sqs->get_message_count(qurl);
        queues.push_back(QueueInfo{qurl, count});
      }
      auto res = ListQueuesResponse{&queues};
      return resp_ok(serde, req, serde->serialize(&res));
    }
    case SQSDeleteQueue: {
      auto body = serde->deserialize_delete_queue_input(req->body());
      if (!body.has_value()) {
        return resp_err(serde, req, BadRequestError("invalid request body"));
      }
      auto qurl = body.value()->get_queue_url();
      if (!sqs->delete_queue(qurl)) {
        return resp_err(serde, req,
                        BadRequestError("The specified queue does not exist."));
      }
      return resp_ok(serde, req, "{}");
    }
    case SQSGetQueueUrl: {
      auto body = serde->deserialize_get_queue_url_input(req->body());
      if (!body.has_value()) {
        return resp_err(serde, req, BadRequestError("invalid request body"));
      }
      auto qurl = sqs->get_queue_url(body.value()->get_queue_name());
      if (!qurl.has_value()) {
        return resp_err(serde, req,
                        BadRequestError("The specified queue does not exist."));
      }
      auto res = GetQueueUrlResponse{qurl.value()};
      return resp_ok(serde, req, serde->serialize(&res));
    }
    case SQSTagQueue: {
      auto body = serde->deserialize_tag_queue_input(req->body());
      if (!body.has_value()) {
        return resp_err(serde, req, BadRequestError("invalid request body"));
      }
      auto tags = body.value()->get_tags();
      auto ok = sqs->tag_queue(body.value()->get_queue_url(), &tags);
      if (!ok) {
        return resp_err(serde, req,
                        BadRequestError("The specified queue does not exist."));
      }
      return resp_ok(serde, req, "{}");
    }
    case SQSListQueueTags: {
      auto body = serde->deserialize_list_queue_tags_input(req->body());
      if (!body.has_value()) {
        return resp_err(serde, req, BadRequestError("invalid request body"));
      }
      auto tags = sqs->get_queue_tags(body.value()->get_queue_url());
      if (!tags.has_value()) {
        return resp_err(serde, req,
                        BadRequestError("The specified queue does not exist."));
      }
      auto res = ListQueueTagsResponse{tags.value().get()};
      return resp_ok(serde, req, serde->serialize(&res));
    }
    case SQSUntagQueue: {
      auto body = serde->deserialize_untag_queue_input(req->body());
      if (!body.has_value()) {
        return resp_err(serde, req, BadRequestError("invalid request body"));
      }
      auto keys = body.value()->get_tag_keys();
      auto ok = sqs->untag_queue(body.value()->get_queue_url(), &keys);
      if (!ok) {
        return resp_err(serde, req,
                        BadRequestError("The specified queue does not exist."));
      }
      return resp_ok(serde, req, "{}");
    }
    case SQSSendMessage: {
      auto body = serde->deserialize_send_message_input(req->body());
      if (!body.has_value()) {
        return resp_err(serde, req, BadRequestError("invalid request body"));
      }
      if (!sqs->send_message(body.value().get())) {
        return resp_err(serde, req,
                        BadRequestError("The specified queue does not exist."));
      }
      return resp_ok(serde, req, "{}");
    }
    case SQSPurgeQueue: {
      auto body = serde->deserialize_purge_queue_input(req->body());
      if (!body.has_value()) {
        return resp_err(serde, req, BadRequestError("invalid request body"));
      }
      if (!sqs->purge_queue(body.value()->get_queue_url())) {
        return resp_err(serde, req,
                        BadRequestError("The specified queue does not exist."));
      }
      return resp_ok(serde, req, "{}");
    }
    case SQSReceiveMessage: {
      auto input = req->body();
      auto body = serde->deserialize_receive_message_input(input);
      if (!body.has_value()) {
        return resp_err(serde, req, BadRequestError("invalid request body"));
      }
      auto msgs = sqs->receive(
          body.value().get()->get_queue_url(),
          body.value().get()->get_max_number_of_messages().value_or(1));
      std::vector<ReceivedMessageResponse> res_msgs;
      for (auto& msg : msgs) {
        res_msgs.push_back(ReceivedMessageResponse{
            msg.message_id, msg.message_id, msg.md5_of_body, msg.body});
      }
      auto res = ReceivedMessagesResponse{res_msgs};
      return resp_ok(serde, req, serde->serialize(&res));
    }
    default:
      return resp_err(
          serde, req,
          Error(restinio::status_not_implemented(), "action not implemented"));
  }
}

restinio::request_handling_status_t html_query_handler(
    SQS* sqs, Serde* serde, restinio::http_request_header_t* headers,
    restinio::request_handle_t req) {
  auto path = req->header().path();
  if (path == "/") {
    headers->set_field(AWS_TARGET, "AmazonSQS.ListQueues");
  }
  return sqs_query_handler(sqs, serde, headers, req);
}

restinio::request_handling_status_t resp_ok(Serde* serde,
                                            restinio::request_handle_t req,
                                            std::string body) {
  return req->create_response()
      .append_header(restinio::http_field::content_type, serde->contentType())
      .set_body(std::move(body))
      .done();
}

restinio::request_handling_status_t resp_err(Serde* serde,
                                             restinio::request_handle_t req,
                                             Error err) {
  return req->create_response(err.status)
      .append_header(restinio::http_field::content_type, serde->contentType())
      .set_body(serde->serialize(&err))
      .done();
}

AWSProtocol extract_protocol(restinio::http_request_header_t* headers) {
  auto content_type = headers->opt_value_of(restinio::http_field::content_type);
  if (content_type.has_value()) {
    if (content_type.value() == AWS_JSON_PROTOCOL_1_0)
      return AWSJsonProtocol1_0;
    if (content_type.value() == AWS_QUERY_PROTOCOL) return AWSQueryProtocol;
  }
  return TextHtml;
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

}  // namespace sqscpp
