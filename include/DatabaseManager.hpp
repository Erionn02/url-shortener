#pragma once

#include <string>
#include <optional>
#include <stdexcept>


class DatabaseManagerException: public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};


class DatabaseManager {
public:
    virtual ~DatabaseManager() = default;
    virtual std::optional<std::string> getOriginalUrl(const std::string& shortened_path) = 0;
    virtual std::string shortenUrl(const std::string& url_to_shorten) = 0;
    virtual void shortenUrl(const std::string& new_path, const std::string& url_to_shorten) = 0;
    virtual bool isForbidden(const std::string& new_path) = 0;
    virtual void addForbiddenPath(const std::string& forbidden_path) = 0;
};

