#include "DatabaseManager.hpp"
#include "ObjectPool.hpp"

#include <pqxx/connection>

using ConnectionPool = ObjectPool<pqxx::connection>;

/**
 *  Class for managing database operations.
 */
class PostgresDBManager : public DatabaseManager {
public:
    PostgresDBManager(const std::string& database_address, const std::string& pg_user, const std::string& pg_password, const std::string& database_name);

    std::string shortenUrl(const std::string& url_to_shorten) override;
    void shortenUrl(const std::string& new_path, const std::string& url_to_shorten) override;
    std::optional<std::string> getOriginalUrl(const std::string& shortened_path) override;
    bool isForbidden(const std::string& new_path) override;
    void addForbiddenPath(const std::string &forbidden_path) override;

private:
    static inline const std::size_t DEFAULT_AMOUNT_OF_CONNECTIONS{10};
    std::size_t current_new_random_path_len{8};
    ConnectionPool connection_pool;

    struct PreparedStatements { // to avoid sql injects
        static constexpr const char* INSERT_URL{"INSERT_URL"};
        static constexpr const char* GET_ORIGINAL_URL{"GET_ORIGINAL_URL"};
        static constexpr const char* IS_PATH_FORBIDDEN{"IS_PATH_FORBIDDEN"};
        static constexpr const char* ADD_FORBIDDEN_PATH{"ADD_FORBIDDEN_PATH"};
    };
};