#ifndef REQUEST_H
#define REQUEST_H
#include <memory>
#include <unordered_map>

#include "Response.h"
#include "Store.h"
#include "Socket.h"

namespace http_server {

enum HttpRequestPrefix {KEY, COUNTER};
enum HttpMethodEnum {GET, POST, DELETE};


class Request {
private:
    std::shared_ptr<Store> store;
    std::shared_ptr<Socket> socket;

    static const std::unordered_map<std::string, HttpMethodEnum> httpMethodMap;

    std::unordered_map<std::string, std::string> readHeader();
    std::string readBody(int contentLength);
    Response prefixKeyPath(const std::unordered_map<std::string, std::string>& headerMetadata);
    Response prefixCounterPath(const std::unordered_map<std::string, std::string>& headerMetadata);
    Response handlePostRequest(const std::unordered_map<std::string, std::string> &headerMetadata,
                                            HttpRequestPrefix httpRequestPrefix);
    Response handleGetRequest(const std::unordered_map<std::string, std::string> &headerMetadata,
                                           HttpRequestPrefix httpRequestPrefix);
    Response handleDeleteRequest(const std::unordered_map<std::string, std::string> &headerMetadata,
                                              HttpRequestPrefix httpRequestPrefix);



public:
    Request(Store* store, Socket* socket);
    Request(std::shared_ptr<Store> store, std::shared_ptr<Socket> socket);
    std::shared_ptr<Response> processRequest();
};

}


#endif //REQUEST_H
