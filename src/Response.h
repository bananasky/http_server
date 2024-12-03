#ifndef RESPONSE_H
#define RESPONSE_H
#include <string>
#include <unordered_map>

namespace http_server {

enum HttpRespStatusEnum {OK, NotFound, MethodNotAllowed, InternalServerError};

class Response {
private:
    static const std::unordered_map<int, HttpRespStatusEnum> statusCodeMap;
    static std::string getHttpRespStatusMsg(HttpRespStatusEnum httpRespStatusEnum);
    static HttpRespStatusEnum getHttpRespStatusEnum(int statusCode);
    int statusCode;
    std::string data;

public:
    Response(int statusCode = 0, const std::string& data = "");
    std::string encodeOutput() const;
    int getStatusCode() { return statusCode; };
    std::string getData() { return data; };
};


}

#endif //RESPONSE_H
