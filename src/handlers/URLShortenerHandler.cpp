#include "handlers/URLShortenerHandler.hpp"
#include "RequestConstants.hpp"

#include <cpprest/http_client.h>

#include <regex>
#include <ranges>

URLShortenerHandler::URLShortenerHandler(std::shared_ptr<DatabaseManager> db_manager, std::string server_domain_address) :
        IRequestHandler(HANDLER_URI, web::http::methods::POST),
        db_manager(std::move(db_manager)),
        SERVER_DOMAIN_ADDRESS(std::move(server_domain_address)),
        SERVER_DOMAIN_NAME_WITHOUT_PROTOCOL(cutProtocolFromDomainAddress(SERVER_DOMAIN_ADDRESS)) {

}

std::string URLShortenerHandler::cutProtocolFromDomainAddress(const std::string &domain_address) {
    std::string domain_address_copy = domain_address;
    for(const auto& protocol: {"http://", "https://"}) {
        if(domain_address_copy.starts_with(protocol)) {
            domain_address_copy = domain_address_copy.substr(strlen(protocol));
        }
    }
    return domain_address_copy;
}

void URLShortenerHandler::doHandle(RequestData &request_data) {
    if (!validateRequest(request_data)) {
        return;
    }
    try {
        std::string new_path = shortenUrl(request_data);
        std::string full_url = fmt::format("{}/{}", SERVER_DOMAIN_ADDRESS, std::move(new_path));
        request_data.setResponse(web::http::status_codes::OK, full_url);
    } catch (const DatabaseManagerException &e) {
        spdlog::warn("URLShortenerHandler Encountered an exception: {}", e.what());
        request_data.setResponse(web::http::status_codes::BadRequest, e.what());
    }
}

std::string URLShortenerHandler::shortenUrl(const RequestData &request_data) {
    auto url_to_shorten = request_data.tryGetHeaderValue(requests::headers::URL_TO_SHORTEN);
    addProtocolPrefix(url_to_shorten);
    auto requested_custom_path = request_data.getHeaderValue(requests::headers::CUSTOM_PATH);
    if (requested_custom_path.has_value()) {
        db_manager->shortenUrl(*requested_custom_path, url_to_shorten);
        return *requested_custom_path;
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
                                  &URLShortenerHandler::isURLDomainNotThisServersDomain,
                                  &URLShortenerHandler::canRequestGivenURL,
                                  &URLShortenerHandler::isCustomPathPermitted
    };
    return std::ranges::all_of(validations, [&](const auto validation) {
        return std::invoke(validation, this, request_data);
    });
}

bool URLShortenerHandler::containsRequiredHeader(RequestData &request_data) {
    auto url_to_shorten = request_data.getHeaderValue(requests::headers::URL_TO_SHORTEN);
    if (!url_to_shorten.has_value()) {
        request_data.setResponse(web::http::status_codes::BadRequest, requests::errors::HEADER_URL_TO_SHORTEN_MISSING);
    }
    return url_to_shorten.has_value();
}

bool URLShortenerHandler::canRequestGivenURL(RequestData &request_data) {
    auto url_to_shorten = request_data.tryGetHeaderValue(requests::headers::URL_TO_SHORTEN);
    addProtocolPrefix(url_to_shorten);
    try {
        web::http::client::http_client_config config;
        config.set_timeout(std::chrono::seconds{3});
        config.set_max_redirects(0);
        web::http::client::http_client client{url_to_shorten, config};
        auto response = client.request(web::http::methods::GET).get();
        return true;
    } catch (const web::http::http_exception &e) {
        spdlog::warn("Encountered an error when requesting url ({}): {}", url_to_shorten, e.what());
        request_data.setResponse(web::http::status_codes::BadRequest, requests::errors::GIVEN_URL_IS_INVALID_OR_NOT_RESPONSIVE);
        return false;
    }
}

bool URLShortenerHandler::isCustomPathPermitted(RequestData &request_data) {
    const auto requested_custom_path = request_data.getHeaderValue(requests::headers::CUSTOM_PATH);
    if (!requested_custom_path.has_value()) {
        return true;
    }

    if (requested_custom_path->length() > MAX_CUSTOM_PATH_LENGTH) {
        request_data.setResponse(web::http::status_codes::BadRequest, requests::errors::GIVEN_CUSTOM_PATH_IS_TOO_LONG);
        return false;
    }

    if (requested_custom_path->length() < MIN_CUSTOM_PATH_LENGTH) {
        request_data.setResponse(web::http::status_codes::BadRequest, requests::errors::GIVEN_CUSTOM_PATH_IS_TOO_SHORT);
        return false;
    }

    if (doesCustomPathStartWithAnyForbiddenCharacter(*requested_custom_path)) {
        request_data.setResponse(web::http::status_codes::BadRequest, requests::errors::GIVEN_CUSTOM_PATH_STARTS_WITH_FORBIDDEN_CHARACTER);
        return false;
    }

    bool is_forbidden = db_manager->isForbidden(*requested_custom_path);
    if (is_forbidden) {
        request_data.setResponse(web::http::status_codes::BadRequest, requests::errors::GIVEN_CUSTOM_PATH_IS_FORBIDDEN);
    }
    return !is_forbidden;
}

bool URLShortenerHandler::isURLDomainNotThisServersDomain(RequestData &request_data) { // to avoid loops in redirections to itself
    auto url_to_shorten = request_data.tryGetHeaderValue(requests::headers::URL_TO_SHORTEN);
    bool is_ok = !url_to_shorten.starts_with(SERVER_DOMAIN_ADDRESS) && !url_to_shorten.starts_with(SERVER_DOMAIN_NAME_WITHOUT_PROTOCOL);
    if(!is_ok) {
        request_data.setResponse(web::http::status_codes::BadRequest, requests::errors::URL_TO_SHORTEN_CANNOT_LEAD_TO_THIS_SERVER);
    }
    return is_ok;
}

bool URLShortenerHandler::doesCustomPathStartWithAnyForbiddenCharacter(const std::string &custom_path) {
    static std::string forbidden_start_characters{"!@#$%^&*()/\\';<>={}[]?|`"};
    return forbidden_start_characters.contains(custom_path.front());
}



