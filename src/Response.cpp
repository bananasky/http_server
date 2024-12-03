#include "Response.h"
#include <sstream>

const std::unordered_map<int, http_server::HttpRespStatusEnum> http_server::Response::statusCodeMap = {
   {200, OK},
   {404, NotFound},
   {405, MethodNotAllowed},
   {500, InternalServerError}
};

http_server::HttpRespStatusEnum http_server::Response::getHttpRespStatusEnum(int statusCode) {
   try {
      return statusCodeMap.at(statusCode);
   } catch (const std::exception& e) {
      return InternalServerError;
   }
}

std::string http_server::Response::getHttpRespStatusMsg(HttpRespStatusEnum httpRespStatusEnum) {
   switch (httpRespStatusEnum) {
      case OK:
         return "OK";
      case NotFound:
         return "NotFound";
      case MethodNotAllowed:
         return "MethodNotAllowed";
      default:
         return "InternalServerError";
   }
};

http_server::Response::Response(int statusCode, const std::string &data) : statusCode(statusCode), data(data) {};

std::string http_server::Response::encodeOutput() const {
   std::ostringstream outputStream;

   std::string statusCodeStr = std::to_string(this->statusCode);
   std::string statusCodeMsg = getHttpRespStatusMsg(getHttpRespStatusEnum(this->statusCode));
   outputStream << statusCodeStr << " " << statusCodeMsg;

   if (data.empty()) {
      outputStream << "  ";
      return outputStream.str();
   } else {
      std::string contentLength = std::to_string(data.length());
      outputStream << " content-length " << contentLength << "  ";
      return outputStream.str() + data;
   }

};
