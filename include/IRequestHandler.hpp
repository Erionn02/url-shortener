#pragma once
#include "RequestData.hpp"

#include <spdlog/spdlog.h>
#include <cpprest/http_headers.h>

#include <set>

class RequestHandlerException: public std::runtime_error{
public:
    using std::runtime_error::runtime_error;
};

class IRequestHandler {
public:
    IRequestHandler() = default;
    IRequestHandler(std::string available_path, web::http::method method);
    IRequestHandler(std::set<std::string> available_path, web::http::method method);
    virtual ~IRequestHandler() = default;
    void handle(RequestData& request_data);
    [[nodiscard]] virtual bool canHandle(const RequestData& request_data) const;
    const std::set<std::string> & getAvailablePaths();
protected:
    virtual void doHandle(RequestData& request_data) = 0;
    bool shouldHandleGivenRequestMethod(web::http::method method) const;
    [[nodiscard]] bool shouldHandleGivenRequestPath(const std::string& path) const;

    web::http::method accepted_http_method;
    std::set<std::string> available_paths;
};

