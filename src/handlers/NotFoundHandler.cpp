#include "handlers/NotFoundHandler.hpp"
#include "Utils.hpp"

#include <fmt/format.h>

NotFoundHandler::NotFoundHandler(std::filesystem::path not_found_page): NOT_FOUND_PAGE(std::move(not_found_page)) {
    if (!std::filesystem::exists(NOT_FOUND_PAGE)) {
        throw RequestHandlerException(fmt::format("'Not found' file {} does not exist!", NOT_FOUND_PAGE.string()));
    }

    if (!std::filesystem::is_regular_file(NOT_FOUND_PAGE)) {
        throw RequestHandlerException(fmt::format("'Not found' file {} should be a regular file!", NOT_FOUND_PAGE.string()));
    }

    if (NOT_FOUND_PAGE.extension() != ".html") {
        throw RequestHandlerException(fmt::format("'Not found' file {} should be html!", NOT_FOUND_PAGE.string()));
    }
}

void NotFoundHandler::doHandle(RequestData &request_data) {
    web::http::http_response response{web::http::status_codes::NotFound};
    response.headers().set_content_type("text/html");
    response.set_body(utils::readFileContent(NOT_FOUND_PAGE));
    request_data.setResponse(response);
}

bool NotFoundHandler::canHandle(const RequestData &) const {
    return true;
}

