#pragma once

#include "IRequestHandler.hpp"

/**
 *  Handler for getting the current API version.
 */

class APIVersionHandler : public IRequestHandler {
public:
    APIVersionHandler();

private:
    void doHandle(RequestData &request_data) override;

public:
    static inline std::string CURRENT_API_VERSION{"1.0.0"};
    static inline std::string HANDLER_URI{"/version"};
};


