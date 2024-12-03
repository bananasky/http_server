#ifndef STORE_H
#define STORE_H
#include <string>
#include <unordered_map>

namespace http_server {
class Store {
public:
    std::unordered_map<std::string, std::string> keyValueStore;
    std::unordered_map<std::string, int> counterStore;

    Store() = default;
};

}

#endif //STORE_H
