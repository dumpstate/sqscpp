#include <restinio/core.hpp>

auto main() -> int {
    restinio::run(
        restinio::on_this_thread()
        .port(8080)
        .address("0.0.0.0")
        .request_handler([](auto req) {
            return req->create_response().set_body("Yellow!\n").done();
        }));
}
