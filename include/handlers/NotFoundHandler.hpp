#pragma once

#include "IRequestHandler.hpp"

#include <filesystem>

class NotFoundHandler : public IRequestHandler{
public:
    NotFoundHandler(std::filesystem::path not_found_page);
    bool canHandle(const RequestData &request_data) const override;
protected:
    void doHandle(RequestData &request_data) override;
private:
    const std::filesystem::path NOT_FOUND_PAGE;
};


