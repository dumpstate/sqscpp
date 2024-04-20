#ifndef SQSCPP_ROUTER_H
#define SQSCPP_ROUTER_H

#include <restinio/core.hpp>

#include "protocol.hpp"
#include "serde.hpp"
#include "sqs.hpp"

namespace sqscpp {
enum AWSProtocol { AWSQueryProtocol, AWSJsonProtocol1_0, TextHtml };
enum SQSAction {
  SQSListQueues,
  SQSAddPermission,
  SQSChangeMessageVisibilityBatch,
  SQSChangeMessageVisibility,
  SQSCreateQueue,
  SQSDeleteMessageBatch,
  SQSDeleteMessage,
  SQSDeleteQueue,
  SQSGetQueueUrl,
  SQSListDeadLetterSourceQueues,
  SQSListQueueTags,
  SQSPurgeQueue,
  SQSGetQueueAttributes,
  SQSSetQueueAttributes,
  SQSReceiveMessage,
  SQSRemovePermission,
  SQSSendMessageBatch,
  SQSSendMessage,
  SQSTagQueue,
  SQSUntagQueue,
  // off AWS SQS, for GUI
  FullQueueData
};

const std::string AWS_JSON_PROTOCOL_1_0 = "application/x-amz-json-1.0";
const std::string AWS_QUERY_PROTOCOL = "text/xml";
const std::string TEXT_HTML = "text/html";
const std::string AWS_TRACE_ID = "x-amzn-trace-id";
const std::string AWS_TARGET = "x-amz-target";
const std::string QUEUE_NAME = "x-queue-name";
const std::map<std::string, SQSAction> AWS_SQS_ACTIONS = {
    {"AmazonSQS.AddPermission", SQSAddPermission},
    {"AmazonSQS.ChangeMessageVisibilityBatch", SQSChangeMessageVisibilityBatch},
    {"AmazonSQS.ChangeMessageVisibility", SQSChangeMessageVisibility},
    {"AmazonSQS.CreateQueue", SQSCreateQueue},
    {"AmazonSQS.DeleteMessageBatch", SQSDeleteMessageBatch},
    {"AmazonSQS.DeleteMessage", SQSDeleteMessage},
    {"AmazonSQS.DeleteQueue", SQSDeleteQueue},
    {"AmazonSQS.GetQueueUrl", SQSGetQueueUrl},
    {"AmazonSQS.ListDeadLetterSourceQueues", SQSListDeadLetterSourceQueues},
    {"AmazonSQS.ListQueueTags", SQSListQueueTags},
    {"AmazonSQS.ListQueues", SQSListQueues},
    {"AmazonSQS.PurgeQueue", SQSPurgeQueue},
    {"AmazonSQS.GetQueueAttributes", SQSGetQueueAttributes},
    {"AmazonSQS.SetQueueAttributes", SQSSetQueueAttributes},
    {"AmazonSQS.ReceiveMessage", SQSReceiveMessage},
    {"AmazonSQS.RemovePermission", SQSRemovePermission},
    {"AmazonSQS.SendMessageBatch", SQSSendMessageBatch},
    {"AmazonSQS.SendMessage", SQSSendMessage},
    {"AmazonSQS.TagQueue", SQSTagQueue},
    {"AmazonSQS.UntagQueue", SQSUntagQueue},
    {"FullQueueData", FullQueueData}};

std::function<restinio::request_handling_status_t(restinio::request_handle_t)>
handler_factory(SQS* sqs, JsonSerde* serde, HtmlSerde* html_serde);
restinio::request_handling_status_t sqs_query_handler(
    SQS* sqs, Serde* serde, restinio::http_request_header_t* headers,
    restinio::request_handle_t req);
restinio::request_handling_status_t html_query_handler(
    SQS* sqs, Serde* serde, restinio::http_request_header_t* headers,
    restinio::request_handle_t req);

AWSProtocol extract_protocol(restinio::http_request_header_t* headers);
std::optional<SQSAction> extract_action(
    restinio::http_request_header_t* headers);
std::optional<std::string> extract_trace_id(
    restinio::http_request_header_t* headers);
std::optional<std::string> extract_queue_name(
    restinio::http_request_header_t* headers);

restinio::request_handling_status_t resp_ok(Serde* serde,
                                            restinio::request_handle_t req,
                                            std::string body);
restinio::request_handling_status_t resp_err(Serde* serde,
                                             restinio::request_handle_t req,
                                             Error err);
}  // namespace sqscpp

#endif  // SQSCPP_ROUTER_H
