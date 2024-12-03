#include "../src/Response.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

TEST_CASE("encodeOutput with empty data") {
    http_server::Response response(200);
    std::string encoded = response.encodeOutput();
    REQUIRE(encoded == "200 OK  ");
}

TEST_CASE("Response::encodeOutput with non-empty data") {
    http_server::Response response(200, "Hello, World!");
    std::string encoded = response.encodeOutput();
    REQUIRE(encoded == "200 OK content-length 13  Hello, World!");
}

TEST_CASE("Response::encodeOutput with 404 status") {
    http_server::Response response(404);
    std::string encoded = response.encodeOutput();
    REQUIRE(encoded == "404 NotFound  ");
}