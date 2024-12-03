#include "Request.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <sys/socket.h>

const std::unordered_map<std::string, http_server::HttpMethodEnum> http_server::Request::httpMethodMap = {
    {"GET", HttpMethodEnum::GET},
    {"POST", HttpMethodEnum::POST},
    {"DELETE", HttpMethodEnum::DELETE}
};

http_server::Request::Request(Store *store, Socket *socket) : store(store), socket(socket) {}

http_server::Request::Request(std::shared_ptr<Store> store, std::shared_ptr<Socket> socket) : store(store), socket(socket) {}

std::shared_ptr<http_server::Response> http_server::Request::processRequest() {
    std::unordered_map<std::string, std::string> headerMetadata = readHeader();
    if (headerMetadata.empty()) {
        return nullptr;
    }

    const std::string& fullPath = headerMetadata["path"];
    if (fullPath.compare(0,5, "/key/") == 0) {
        return std::make_shared<Response>(prefixKeyPath(headerMetadata));
    } else if (fullPath.compare(0,9, "/counter/") == 0) {
        return std::make_shared<Response>(prefixCounterPath(headerMetadata));
    }

    return nullptr;

}

http_server::Response http_server::Request::prefixKeyPath(const std::unordered_map<std::string, std::string> &headerMetadata) {

    switch (httpMethodMap.at(headerMetadata.at("method"))) {
        case HttpMethodEnum::POST:
            return handlePostRequest(headerMetadata, KEY);
        case HttpMethodEnum::GET:
            return handleGetRequest(headerMetadata, KEY);
        case HttpMethodEnum::DELETE:
            return handleDeleteRequest(headerMetadata, KEY);
        default:
            return Response(404);
    }
}

http_server::Response http_server::Request::prefixCounterPath(const std::unordered_map<std::string, std::string> &headerMetadata) {

    switch (httpMethodMap.at(headerMetadata.at("method"))) {
        case HttpMethodEnum::POST:
            return handlePostRequest(headerMetadata, COUNTER);
        case HttpMethodEnum::GET:
            return handleGetRequest(headerMetadata, COUNTER);
        case HttpMethodEnum::DELETE:
            return handleDeleteRequest(headerMetadata, COUNTER);
        default:
            return Response(404);
    }
}

http_server::Response http_server::Request::handlePostRequest(const std::unordered_map<std::string, std::string> &headerMetadata, HttpRequestPrefix httpRequestPrefix) {

    if (httpRequestPrefix == HttpRequestPrefix::KEY) {
        std::string urlKey = headerMetadata.at("path").substr(5);

        int contentLength = std::stoi(headerMetadata.at("content-length"));
        std::string contentBody = readBody(contentLength);

        if (store->keyValueStore.count(urlKey) > 0 && store->counterStore[urlKey] > 0) {
            return Response(405);
        } else {
            store->keyValueStore[urlKey] = contentBody;
            return Response(200);
        }

    } else if (httpRequestPrefix == HttpRequestPrefix::COUNTER) {
        std::string urlKey = headerMetadata.at("path").substr(9);

        if (store->keyValueStore.count(urlKey) == 0) {
            return Response(405);
        }

        int contentLength = std::stoi(headerMetadata.at("content-length"));
        int counterForKey = std::stoi(readBody(contentLength));

        store->counterStore[urlKey] += counterForKey;
        return Response(200);

    }

    return Response(404);


}

http_server::Response http_server::Request::handleGetRequest(const std::unordered_map<std::string, std::string> &headerMetadata, HttpRequestPrefix httpRequestPrefix) {
    if (httpRequestPrefix == HttpRequestPrefix::KEY) {
        std::string urlKey = headerMetadata.at("path").substr(5);
        if (store->keyValueStore.count(urlKey) == 0) {
            return Response(404);
        }

        std::string kvStoreData = store->keyValueStore[urlKey];
        if (store->counterStore.count(urlKey) > 0 && store->counterStore[urlKey] > 0) {
            store->counterStore[urlKey]--;
            if (store->counterStore[urlKey] == 0) {
                store->keyValueStore.erase(urlKey);
                store->counterStore.erase(urlKey);
            }
        }
        return Response(200, kvStoreData);

    } else if (httpRequestPrefix == HttpRequestPrefix::COUNTER) {
        std::string urlKey = headerMetadata.at("path").substr(9);
        auto kvStoreIt = store->keyValueStore.find(urlKey);
        auto counterStoreIt = store->counterStore.find(urlKey);

        if (kvStoreIt == store->keyValueStore.end() && counterStoreIt == store->counterStore.end()) {
            return Response(404);
        } else if (kvStoreIt != store->keyValueStore.end() && counterStoreIt == store->counterStore.end()) {
            return Response(200, "Infinity");
        } else {
            return Response(200, std::to_string(counterStoreIt->second));
        }

    }

    return Response(404);

}

http_server::Response http_server::Request::handleDeleteRequest(const std::unordered_map<std::string, std::string> &headerMetadata, HttpRequestPrefix httpRequestPrefix) {
    if (httpRequestPrefix == HttpRequestPrefix::KEY) {
        std::string urlKey = headerMetadata.at("path").substr(5);
        if (store->keyValueStore.count(urlKey) == 0) {
            return Response(404);
        } else if (store->counterStore.count(urlKey) > 0 && store->counterStore[urlKey] > 0) {
            return Response(405);
        } else {
            std::string kvStoreData = store->keyValueStore[urlKey];
            store->keyValueStore.erase(urlKey);
            return Response(200, kvStoreData);
        }

    } else if (httpRequestPrefix == HttpRequestPrefix::COUNTER) {
        std::string urlKey = headerMetadata.at("path").substr(9);
        if (store->counterStore.count(urlKey) == 0) {
            return Response(404);
        }

        std::string counterStoreValue = std::to_string(store->counterStore[urlKey]);
        store->counterStore.erase(urlKey);
        return Response(200, counterStoreValue);

    }

    return Response(404);

}

std::unordered_map<std::string, std::string> http_server::Request::readHeader() {
    std::unordered_map<std::string, std::string> headerMetadata;

    std::vector<std::string> headerSegments;
    std::string subStr;
    std::string bufferStr;

    while (true) {
        if (socket->recv(bufferStr, 1) == 0) {
            break;
        }
        char ch = bufferStr[0];
        if (ch != ' ') {
            subStr.push_back(ch);
        } else {
            if (!subStr.empty()) {
                headerSegments.push_back(subStr);
                subStr = "";
            } else {
                break;
            }
        }

    }

    if (headerSegments.empty()) return headerMetadata;

    std::transform(headerSegments[0].begin(), headerSegments[0].end(), headerSegments[0].begin(), toupper);
    headerMetadata["method"] = headerSegments[0];
    headerMetadata["path"] = headerSegments[1];

    for (std::size_t i = 2; i < headerSegments.size(); i+= 2) {
        std::string segment = headerSegments[i];
        std::transform(segment.begin(), segment.end(), segment.begin(), tolower);

        if (segment == "content-length") {
            headerMetadata["content-length"] = headerSegments[i+1];
            break;
        }
    }

    return headerMetadata;

}

std::string http_server::Request::readBody(int contentLength) {
    std::string body;
    while (contentLength > 0) {
        std::string chunk;
        socket->recv(chunk, contentLength);
        body.append(chunk);
        contentLength -= static_cast<int>(chunk.length());
    }
    return body;
}

