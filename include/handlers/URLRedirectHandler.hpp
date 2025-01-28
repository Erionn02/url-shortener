#pragma once

#include "IRequestHandler.hpp"

#include "DatabaseManager.hpp"


/**
 *  Request handler for redirecting shortened URLs to their original form.
 */

class URLRedirectHandler: public IRequestHandler {
public:
    explicit URLRedirectHandler(std::shared_ptr<DatabaseManager> db_manager);

    bool canHandle(const RequestData &request_data) const override;
    void doHandle(RequestData &request_data) override;

private:
    std::shared_ptr<DatabaseManager> db_manager;
};


