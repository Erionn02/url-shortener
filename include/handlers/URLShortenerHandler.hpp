#pragma once

#include "IRequestHandler.hpp"
#include "DatabaseManager.hpp"

#include <memory>

class URLShortenerHandler: public IRequestHandler {
public:
    URLShortenerHandler(std::shared_ptr<DatabaseManager> db_manager, std::string server_domain_name);

private:
    void doHandle(RequestData &request_data) override;
    bool validateRequest(RequestData &request_data);
    bool containsRequiredHeaders(RequestData &request_data);
    bool isURLValid(RequestData &request_data);
    bool canRequestGivenURL(RequestData &request_data);
    std::string shortenUrl(const RequestData &request_data);
    void addProtocolPrefix(std::string& url);
    bool isURLForbidden(RequestData &request_data);


    std::shared_ptr<DatabaseManager> db_manager;
    const std::string SERVER_DOMAIN_NAME;
public:
    static inline std::string HANDLER_URI{"/shorten_url"};
};


