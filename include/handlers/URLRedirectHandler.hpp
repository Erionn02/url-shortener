#pragma once

#include "IRequestHandler.hpp"

#include "DatabaseManager.hpp"


class URLRedirectHandler: public IRequestHandler {
public:
    explicit URLRedirectHandler(std::shared_ptr<DatabaseManager> db_manager);

private:
    void doHandle(RequestData &request_data) override;
    bool canHandle(const RequestData &request_data) const override;


    std::shared_ptr<DatabaseManager> db_manager;
};


