#include "handlers/IRequestHandler.hpp"


IRequestHandler::IRequestHandler(web::http::method method): accepted_http_method(std::move(method)) {
}

IRequestHandler::IRequestHandler(std::set<std::string> available_path, web::http::method method)
        : accepted_http_method(std::move(method)),
          available_paths(std::move(available_path)) {}

IRequestHandler::IRequestHandler(std::string available_path, web::http::method method)
        : accepted_http_method(std::move(method)),
          available_paths({std::move(available_path)}) {}

void IRequestHandler::handle(RequestData &request_data) {
    try {
        doHandle(request_data);
        return;
    } catch (const std::exception &e) {
        spdlog::critical("Handler's unhandled exception: {}", e.what());
    } catch (...) {
        spdlog::critical("HTTP handler's unhandled exception...");
    }
    request_data.setResponse(web::http::status_codes::InternalError, "Internal error");
}

bool IRequestHandler::canHandle(const RequestData &request_data) const {
    return shouldHandleGivenRequestMethod(request_data.getMethod()) &&
           shouldHandleGivenRequestPath(request_data.path());
}

bool IRequestHandler::shouldHandleGivenRequestMethod(web::http::method method) const {
    return method == accepted_http_method;
}

bool IRequestHandler::shouldHandleGivenRequestPath(const std::string &path) const {
    return std::ranges::any_of(available_paths, [&path](const auto &uri) { return path == uri; });
}