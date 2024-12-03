#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <memory>

#include "Store.h"

namespace http_server {

class WebServer {
private:
    std::shared_ptr<Store> store;
    std::string host;
    int port;

public:
    WebServer(const std::string& host = "127.0.0.1", int port = 4567);
    void start();
};

}


#endif //WEBSERVER_H
