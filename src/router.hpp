#ifndef SQSCPP_ROUTER_H
#define SQSCPP_ROUTER_H

#include <restinio/core.hpp>

namespace sqscpp {
using router_t = restinio::router::express_router_t<>;
std::unique_ptr<router_t> create_router();
}  // namespace sqscpp

#endif  // SQSCPP_ROUTER_H
