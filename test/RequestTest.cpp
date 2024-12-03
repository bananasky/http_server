#include "../src/Request.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>
#include <queue>
#include <unordered_map>

class MockSocket : public http_server::Socket {
public:
    std::queue<std::string> mockData;

    void pushMockData(const std::string& data) {
        mockData.push(data);
    }

    ssize_t recv(std::string& buffer, size_t size) override {
        if (mockData.empty()) return 0;
        buffer = mockData.front().substr(0, size);
        mockData.front().erase(0, size);
        if (mockData.front().empty()) mockData.pop();
        return static_cast<ssize_t>(buffer.size());
    }
};


TEST_CASE("Request processes GET /key/{key} request correctly") {
    auto store = std::make_shared<http_server::Store>();
    auto socket = std::make_shared<MockSocket>();
    http_server::Request request(store, socket);

    socket->pushMockData("GET /key/exampleKey  ");
    auto responseNotFound = request.processRequest();
    REQUIRE(responseNotFound->getStatusCode() == 404);

    socket->pushMockData("GET /key/exampleKey  ");
    store->keyValueStore["exampleKey"] = "exampleValue";
    auto responseOK = request.processRequest();
    REQUIRE(responseOK->getStatusCode() == 200);
}

TEST_CASE("Request processes POST /key/{key} request correctly") {
    auto store = std::make_shared<http_server::Store>();
    auto socket = std::make_shared<MockSocket>();
    http_server::Request request(store, socket);

    socket->pushMockData("POST /key/newKey content-length 14  exampleContent");

    auto response = request.processRequest();

    REQUIRE(response->getStatusCode() == 200);
    REQUIRE(store->keyValueStore["newKey"] == "exampleContent");
}

TEST_CASE("Request rejects duplicate POST /key/{key} with counter > 0 requests with 405") {
    auto store = std::make_shared<http_server::Store>();
    auto socket = std::make_shared<MockSocket>();
    http_server::Request request(store, socket);

    store->keyValueStore["duplicateKey"] = "originalContent";
    store->counterStore["duplicateKey"] = 3;

    socket->pushMockData("POST /key/duplicateKey content-length 10  newContent");
    auto response = request.processRequest();

    REQUIRE(response->getStatusCode() == 405);
}

TEST_CASE("Request processes DELETE /key/{key} request correctly") {
    auto store = std::make_shared<http_server::Store>();
    auto socket = std::make_shared<MockSocket>();
    http_server::Request request(store, socket);

    store->keyValueStore["deleteKey"] = "contentToDelete";

    socket->pushMockData("DELETE /key/deleteKey  ");

    auto responseOK = request.processRequest();

    REQUIRE(responseOK->getStatusCode() == 200);
    REQUIRE(store->keyValueStore.count("deleteKey") == 0);


    socket->pushMockData("DELETE /key/nonExistentKey  ");
    auto responseNotFound = request.processRequest();
    REQUIRE(responseNotFound->getStatusCode() == 404);
}

TEST_CASE("Request handles GET /counter/{key} correctly") {
    auto store = std::make_shared<http_server::Store>();
    auto socket = std::make_shared<MockSocket>();
    http_server::Request request(store, socket);

    store->keyValueStore["counterKey"] = "someContent";
    store->counterStore["counterKey"] = 5;

    socket->pushMockData("GET /counter/counterKey  ");

    auto responseFiniteCounter = request.processRequest();

    REQUIRE(responseFiniteCounter->getStatusCode() == 200);
    REQUIRE(responseFiniteCounter->getData() == "5");
}

TEST_CASE("Request handles GET /counter/{key} without counter correctly") {
    auto store = std::make_shared<http_server::Store>();
    auto socket = std::make_shared<MockSocket>();
    http_server::Request request(store, socket);

    store->keyValueStore["infKey"] = "someContent";

    socket->pushMockData("GET /counter/infKey  ");

    auto responseInfiniteCounter = request.processRequest();

    REQUIRE(responseInfiniteCounter->getStatusCode() == 200);
    REQUIRE(responseInfiniteCounter->getData() == "Infinity");
}

