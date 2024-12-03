#ifndef SOCKET_H
#define SOCKET_H
#include <memory>
#include <sys/types.h>

namespace http_server {

class Socket {
private:
    int sockfd;
    bool is_closed;
public:
    Socket();
    ~Socket();

    void bind(const std::string& host, int port);
    void listen(int backlog = 10); // backlog - max length of the queue for pending connections
    std::pair<std::shared_ptr<Socket>, std::string> accept();
    ssize_t send(const std::string& data);

    virtual ssize_t recv(std::string& buffer, size_t size);
    void close();
    int getFd() { return sockfd; };
};
};

#endif //SOCKET_H
