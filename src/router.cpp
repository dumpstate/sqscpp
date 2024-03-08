#include "router.hpp"

namespace sqscpp {
std::unique_ptr<router_t> create_router() {
  auto router = std::make_unique<router_t>();

  router->http_get("/", [](auto req, auto) {
    req->create_response()
        .append_header(restinio::http_field::content_type,
                       "text/plain; charset=utf-8")
        .set_body("Fooooo")
        .done();
    return restinio::request_accepted();
  });

  router->non_matched_request_handler([](auto req) {
    return req->create_response(restinio::status_not_found())
        .append_header(restinio::http_field::content_type,
                       "text/xml; charset=utf-8")
        .set_body("<ErrorResponse></ErrorResponse>")
        .connection_close()
        .done();
  });

  return router;
}
}  // namespace sqscpp
