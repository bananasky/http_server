cmake_minimum_required(VERSION 3.22)
project(http_server)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_library(http_server_core
        src/WebServer.cpp
        src/WebServer.h
        src/Request.cpp
        src/Request.h
        src/Response.cpp
        src/Response.h
        src/Store.h
        src/Socket.h
        src/Socket.cpp
)

add_executable(WebServer WebServer-Service.cpp)

target_link_libraries(WebServer PRIVATE http_server_core)

add_subdirectory(test)