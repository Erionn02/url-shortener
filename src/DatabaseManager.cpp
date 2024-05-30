#include "DatabaseManager.hpp"

#include <fmt/format.h>

DatabaseManager::DatabaseManager(const std::string &database_address, const std::string &pg_user,
                                 const std::string &pg_password, const std::string &database_name): connection(fmt::format("insert credentials here")) {

}

std::string DatabaseManager::shortenUrl(const std::string &url_to_shorten) {
    return std::string();
}

bool DatabaseManager::shortenUrl(const std::string &new_url, const std::string &url_to_shorten) {
    return false;
}
