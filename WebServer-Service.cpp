#include <iostream>

#include "src/WebServer.h"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    std::string port = argv[1];
    for (char ch : port) {
        if (!isdigit(ch)) {
            std::cerr << "Invalid port number" << std::endl;
            return 1;
        }
    }

    http_server::WebServer server("127.0.0.1", std::stoi(port));
    server.start();
    std::cout << "Starting webserver on port " << port << std::endl;


    return 0;
}
