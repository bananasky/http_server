#include "WebServer.h"

#include <iostream>
#include <string>

#include "Request.h"

http_server::WebServer::WebServer(const std::string &host, int port) : host(host), port(port), store(new Store()) {}

void http_server::WebServer::start() {
    std::shared_ptr<Socket> serverSocket(new Socket());

    try {
        serverSocket->bind(host, port);
        serverSocket->listen();

        while (true) {
            std::shared_ptr<Socket> clientSocket = serverSocket->accept().first;
            try {
                while (true) {
                    Request req(store, clientSocket);
                    std::shared_ptr<Response> resp = req.processRequest();
                    if (resp == nullptr) {
                        break;
                    }
                    clientSocket->send(resp->encodeOutput());
                }
                clientSocket->close();

            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                clientSocket->close();
                std::cout << "Client connection closed" << std::endl;
            }
        }

        serverSocket->close();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        serverSocket->close();
        std::cout << "Server connection closed" << std::endl;
    }
}
