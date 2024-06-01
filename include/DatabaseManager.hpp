#pragma once

#include <pqxx/connection>

#include <optional>

class DatabaseManager {
public:
    DatabaseManager(const std::string& database_address, const std::string& pg_user, const std::string& pg_password, const std::string& database_name);

    std::string shortenUrl(const std::string& url_to_shorten);
    void shortenUrl(const std::string& new_path, const std::string& url_to_shorten);
    std::optional<std::string> getOriginalUrl(const std::string& shortened_path);
private:
    std::string generateRandomString(std::size_t length);

    const std::size_t DEFAULT_NEW_PATH_LEN{10};
    pqxx::connection connection;

    struct PreparedStatements { // to avoid sql injects
        static constexpr const char* INSERT_WITH_RANDOM_KEY{"INSERT_WITH_RANDOM_KEY"};
        static constexpr const char* INSERT_WITH_SPECIFIED_KEY{"INSERT_WITH_SPECIFIED_KEY"};
        static constexpr const char* GET_ORIGINAL_URL{"GET_ORIGINAL_URL"};
    };
};

