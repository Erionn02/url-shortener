#include "RequestData.hpp"

#include <fmt/format.h>

RequestData::RequestData(web::http::http_request request)
        : request(request) {
    response.set_body("Request was not handled!");
}

web::http::method RequestData::getMethod() const { return request.method(); }

void RequestData::setResponse(web::http::http_response new_response) { response = std::move(new_response); }

void RequestData::reply() { request.reply(response); }

std::string RequestData::path() const { return request.relative_uri().path(); }

web::http::http_headers& RequestData::getHeaders() { return request.headers(); }

web::http::http_response& RequestData::getResponse() { return response; }

std::string RequestData::tryGetHeaderValue(const std::string& name) const {
    auto value = request.headers().find(name);
    if (value == request.headers().end()) {
        throw std::out_of_range {fmt::format("No header named {}", name)};
    }
    return value->second;
}

std::optional<std::string> RequestData::getHeaderValue(const std::string& name) const {
    std::optional<std::string> header_value;
    auto value_it = request.headers().find(name);
    if (value_it != request.headers().end()) {
        header_value = value_it->second;
    }
    return header_value;
}

web::http::http_request& RequestData::getRequest() { return request; }
