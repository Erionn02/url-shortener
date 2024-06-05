#pragma once

#include "IRequestHandler.hpp"
#include "DatabaseManager.hpp"

#include <memory>

class URLShortenerHandler: public IRequestHandler {
public:
    URLShortenerHandler(std::shared_ptr<DatabaseManager> db_manager, std::string server_domain_address);

private:
    void doHandle(RequestData &request_data) override;
    bool validateRequest(RequestData &request_data);
    bool containsRequiredHeader(RequestData &request_data);
    bool isURLValid(RequestData &request_data);
    bool isURLDomainNotThisServersDomain(RequestData &request_data);
    bool canRequestGivenURL(RequestData &request_data);
    std::string shortenUrl(const RequestData &request_data);
    void addProtocolPrefix(std::string& url);
    bool isCustomPathPermitted(RequestData &request_data);
    std::string cutProtocolFromDomainAddress(const std::string& domain_address);


    std::shared_ptr<DatabaseManager> db_manager;
    const std::string SERVER_DOMAIN_ADDRESS;
    const std::string SERVER_DOMAIN_NAME_WITHOUT_PROTOCOL;
public:
    static inline std::size_t MAX_CUSTOM_PATH_LENGTH{100};
    static inline std::string HANDLER_URI{"/shorten_url"};
};


