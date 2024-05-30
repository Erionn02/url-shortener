#pragma once

#include <pqxx/connection>

class DatabaseManager {
public:
    DatabaseManager(const std::string& database_address, const std::string& pg_user, const std::string& pg_password, const std::string& database_name);

    std::string shortenUrl(const std::string& url_to_shorten);
    bool shortenUrl(const std::string& new_url, const std::string& url_to_shorten);
private:
    struct PreparedStatements{
        static constexpr const char* INSERT_WITH_RANDOM_KEY{"INSERT_WITH_RANDOM_KEY"};
        static constexpr const char* INSERT_WITH_SPECIFIED_KEY{"INSERT_WITH_SPECIFIED_KEY"};
    };
    pqxx::connection connection;
};

