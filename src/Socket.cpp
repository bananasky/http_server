#include "Socket.h"

#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdexcept>
#include <unistd.h>
#include <errno.h>

http_server::Socket::Socket() {
    sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        throw std::runtime_error("Failed to create socket");
    }
    is_closed = false;
}

http_server::Socket::~Socket() {
    close();
}

void http_server::Socket::close() {
    if (!is_closed && sockfd != -1) {
        ::close(sockfd);
        sockfd = -1;
        is_closed = true;
    }
}

void http_server::Socket::bind(const std::string& host, int port) {
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(host.c_str());

    if (::bind(sockfd, (sockaddr*) &server_address, sizeof(server_address)) == -1) {
        throw std::runtime_error("Failed to bind socket");
    }
}

void http_server::Socket::listen(int backlog) {
    if (::listen(sockfd, backlog) == -1) {
        throw std::runtime_error("Failed to listen on socket");
    }
}

std::pair<std::shared_ptr<http_server::Socket>, std::string> http_server::Socket::accept() {
    sockaddr_in client_address{};
    socklen_t client_address_len = sizeof(client_address);
    int clientfd = ::accept(sockfd, (sockaddr*) &client_address, &client_address_len);
    if (clientfd == -1) {
        throw std::runtime_error("Failed to accept connection");
    }

    std::shared_ptr<Socket> clientSocket(new Socket());
    clientSocket->sockfd = clientfd;
    std::string clientAddr(inet_ntoa(client_address.sin_addr));
    return std::make_pair(clientSocket, clientAddr);
}

ssize_t http_server::Socket::send(const std::string& data) {
    return ::send(sockfd, data.c_str(), data.size(), 0);
}

ssize_t http_server::Socket::recv(std::string& buffer, size_t size) {
    char tempBuffer[size];
    ssize_t bytesReceived = ::recv(sockfd, tempBuffer, size,0);
    if (bytesReceived > 0) {
        buffer.assign(tempBuffer, bytesReceived);
    } else if (bytesReceived < 0) {
        throw std::runtime_error(strerror(errno));
    }
    return bytesReceived;
}




