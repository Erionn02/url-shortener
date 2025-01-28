#pragma once

#include "IRequestHandler.hpp"

#include <filesystem>

/** Fallback handler for requests that do not match any other handler.
  * @return 404 Not Found response with a custom HTML not found page.
  */
class NotFoundHandler : public IRequestHandler{
public:
    NotFoundHandler(std::filesystem::path not_found_page);
    bool canHandle(const RequestData &request_data) const override;
protected:
    void doHandle(RequestData &request_data) override;
private:
    const std::filesystem::path NOT_FOUND_PAGE;
};


