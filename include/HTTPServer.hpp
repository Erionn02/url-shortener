#pragma once

#include "handlers/IRequestHandler.hpp"

#include <fmt/format.h>
#include <cpprest/http_listener.h>

#include <vector>
#include <stdexcept>

class HTTPServerException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class HTTPServer {
public:
    explicit HTTPServer(const utility::string_t &url);
    virtual ~HTTPServer();
    void addHandler(std::unique_ptr<IRequestHandler> request_handler);
    void open();
    void close();

protected:
    virtual void handleRequest(const web::http::http_request &request);
    void checkAnyHandlerExists() const;

    std::vector<std::unique_ptr<IRequestHandler>> handlers;
    web::http::experimental::listener::http_listener listener;
    bool is_opened{false};
};

