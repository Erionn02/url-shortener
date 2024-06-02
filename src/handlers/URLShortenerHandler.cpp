#include "handlers/URLShortenerHandler.hpp"
#include "RequestConstants.hpp"

#include <cpprest/http_client.h>

#include <regex>
#include <ranges>

URLShortenerHandler::URLShortenerHandler(std::shared_ptr<DatabaseManager> db_manager, std::string server_domain_name) :
        IRequestHandler(HANDLER_URI, web::http::methods::POST),
        db_manager(std::move(db_manager)),
        SERVER_DOMAIN_NAME(std::move(server_domain_name)) {

}

void URLShortenerHandler::doHandle(RequestData &request_data) {
    if (!validateRequest(request_data)) {
        return;
    }
    try {
        std::string new_path = shortenUrl(request_data);
        std::string full_url = fmt::format("{}/{}", SERVER_DOMAIN_NAME, std::move(new_path));
        request_data.setResponse(web::http::status_codes::OK, full_url);
    } catch (const DatabaseManagerException &e) {
        spdlog::warn("URLShortenerHandler Encountered an exception: {}", e.what());
        request_data.setResponse(web::http::status_codes::BadRequest, e.what());
    }
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
    static std::array validations{&URLShortenerHandler::containsRequiredHeader,
                                  &URLShortenerHandler::isURLValid,
                                  &URLShortenerHandler::canRequestGivenURL,
                                  &URLShortenerHandler::isCustomURLPermitted
    };
    return std::ranges::all_of(validations, [&](const auto validation) {
        return std::invoke(validation, this, request_data);
    });
}

bool URLShortenerHandler::containsRequiredHeader(RequestData &request_data) {
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
//    return is_valid; // todo use actual regex
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

bool URLShortenerHandler::isCustomURLPermitted(RequestData &request_data) {
    auto requested_custom_url = request_data.getHeaderValue(requests::headers::CUSTOM_URL);
    if (!requested_custom_url.has_value()) {
        return true;
    }

    if (requested_custom_url->length() > MAX_CUSTOM_URL_LENGTH) {
        request_data.setResponse(web::http::status_codes::BadRequest, requests::errors::GIVEN_CUSTOM_URL_IS_TOO_LONG);
        return false;
    }

    bool is_forbidden = db_manager->isForbidden(*requested_custom_url);
    if (is_forbidden) {
        request_data.setResponse(web::http::status_codes::BadRequest, requests::errors::GIVEN_CUSTOM_URL_IS_FORBIDDEN);
    }
    return !is_forbidden;
}



