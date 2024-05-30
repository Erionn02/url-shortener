#include "HTTPServer.hpp"

#include <spdlog/spdlog.h>


HTTPServer::HTTPServer(const utility::string_t& url) : listener(url) {
    listener.support(
            [this] (const web::http::http_request& request) {
                spdlog::debug("Got request, path: {}", request.relative_uri().path());
                auto request_data = RequestData(request);
                handleRequest(request_data);
            });
    spdlog::info("HTTPServer started listening on {} url.", url);
}

HTTPServer::~HTTPServer() {
    try {
        close();
    } catch (const std::exception& e) {
        spdlog::error("HTTPServer Encountered an exception during destruction: {}", e.what());
    } catch (...) {
        spdlog::error("HTTPServer Encountered an exception during destruction.");
    }
}

void HTTPServer::handleRequest(RequestData& request) {
    for (auto& handler: handlers) {
        if (handler->canHandle(request)) {
            handler->handle(request);
            request.reply();
            return;
        }
    }
    handleBadRequest(request);
}

void HTTPServer::handleBadRequest(RequestData& request) {
    request.setResponse(web::http::status_codes::NotFound, "lol");
    request.reply();
}

void HTTPServer::addHandler(
        std::unique_ptr<IRequestHandler> request_handler) {
    if (!is_opened) {
        handlers.emplace_back(std::move(request_handler));
    } else {
        throw HTTPServerException("Cannot add new handler in already running server.");
    }
}

void HTTPServer::open() {
    checkAnyHandlerExists();

    if (!is_opened) {
        listener.open().wait();
        is_opened = true;
    } else {
        throw HTTPServerException("Cannot open already opened http listener.");
    }
}

void HTTPServer::checkAnyHandlerExists() const {
    if (handlers.empty()) {
        throw HTTPServerException("Add at least one handler before starting the server.");
    }
}

void HTTPServer::close() {
    if (is_opened) {
        listener.close().wait();
        is_opened = false;
    }
}

