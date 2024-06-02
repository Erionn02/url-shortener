#include "handlers/URLShortenerHandler.hpp"
#include "RequestConstants.hpp"

#include <cpprest/http_client.h>

#include <regex>

URLShortenerHandler::URLShortenerHandler(std::shared_ptr<DatabaseManager> db_manager, std::string server_domain_name) :
        IRequestHandler(HANDLER_URI, web::http::methods::POST),
        db_manager(std::move(db_manager)),
        SERVER_DOMAIN_NAME(std::move(server_domain_name)) {

}

void URLShortenerHandler::doHandle(RequestData &request_data) {
    if (!validateRequest(request_data)) {
        return;
    }

    std::string new_path = shortenUrl(request_data);
    std::string full_url = fmt::format("{}/{}", SERVER_DOMAIN_NAME, std::move(new_path));
    request_data.setResponse(web::http::status_codes::OK, full_url);
}

std::string URLShortenerHandler::shortenUrl(const RequestData &request_data) {
    auto url_to_shorten = request_data.tryGetHeaderValue(requests::headers::URL_TO_SHORTEN);
    addProtocolPrefix(url_to_shorten);
    auto requested_custom_url = request_data.getHeaderValue(requests::headers::CUSTOM_URL);
    if (requested_custom_url.has_value()) {
        db_manager->shortenUrl(*requested_custom_url, url_to_shorten);
        return *requested_custom_url;
    } else {
        return db_manager->shortenUrl(url_to_shorten);
    }
}

void URLShortenerHandler::addProtocolPrefix(std::string &url) {
    if (!url.starts_with("http://") && !url.starts_with("https://")) {
        url = "https://" + url;
    }
}

bool URLShortenerHandler::validateRequest(RequestData &request_data) {
    return containsRequiredHeaders(request_data) && isURLValid(request_data) && canRequestGivenURL(request_data);
}

bool URLShortenerHandler::containsRequiredHeaders(RequestData &request_data) {
    auto url_to_shorten = request_data.getHeaderValue(requests::headers::URL_TO_SHORTEN);
    if (!url_to_shorten.has_value()) {
        request_data.setResponse(web::http::status_codes::BadRequest, requests::errors::URL_TO_SHORTEN_MISSING);
    }
    return url_to_shorten.has_value();
}

bool URLShortenerHandler::isURLValid(RequestData &) {
//    bool is_valid = std::regex_match(request_data.tryGetHeaderValue(requests::headers::URL_TO_SHORTEN), requests::URL_VALIDATOR);
//    if (!is_valid) {
//        request_data.setResponse(web::http::status_codes::BadRequest, requests::errors::INVALID_URL);
//    }
//    return is_valid;
    return true;
}

bool URLShortenerHandler::canRequestGivenURL(RequestData &request_data) {
    auto url_to_shorten = request_data.tryGetHeaderValue(requests::headers::URL_TO_SHORTEN);
    addProtocolPrefix(url_to_shorten);
    try {
        web::http::client::http_client_config config;
        config.set_timeout(std::chrono::seconds{3});
        web::http::client::http_client client{url_to_shorten, config};
        auto response = client.request(web::http::methods::GET).get();
        return true;
    } catch (const std::exception &e) {
        spdlog::warn("Encountered an error when requesting url ({}): {}", url_to_shorten, e.what());
        request_data.setResponse(web::http::status_codes::BadRequest, requests::errors::GIVEN_URL_IS_NOT_RESPONSIVE);
        return false;
    }
}

bool URLShortenerHandler::isURLForbidden(RequestData &) {
    return false;
}



