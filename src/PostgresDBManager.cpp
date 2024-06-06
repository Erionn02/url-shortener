#include "PostgresDBManager.hpp"
#include "Utils.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <pqxx/transaction>


PostgresDBManager::PostgresDBManager(const std::string &database_address, const std::string &pg_user,
                                     const std::string &pg_password, const std::string &database_name) :
        connection_pool(DEFAULT_AMOUNT_OF_CONNECTIONS,
                        fmt::format("dbname={} user={} password={} host={} port={}", database_name, pg_user,
                                    pg_password, database_address, 5432)) {
    connection_pool.invokeOnAllThreadSafe([](std::shared_ptr<pqxx::connection> &connection) {
        connection->prepare(PreparedStatements::GET_ORIGINAL_URL,
                            "SELECT original_url FROM urls WHERE shortened_version = $1");
        connection->prepare(PreparedStatements::INSERT_URL, "INSERT INTO urls SELECT $1, $2 WHERE NOT EXISTS (SELECT 1 FROM forbidden_paths WHERE shortened_version = $1)");
        connection->prepare(PreparedStatements::IS_PATH_FORBIDDEN,
                            "SELECT COUNT(*) FROM forbidden_paths WHERE shortened_version = $1");
        connection->prepare(PreparedStatements::ADD_FORBIDDEN_PATH,
                            "INSERT INTO forbidden_paths VALUES($1) ON CONFLICT DO NOTHING");
    });

}

std::string PostgresDBManager::shortenUrl(const std::string &url_to_shorten) {
    auto connection = connection_pool.getObject();
    pqxx::work transaction{*connection};
    while (true) {
        try {
            auto new_path = utils::generateRandomString(current_new_random_path_len);
            auto result = transaction.exec_prepared(PreparedStatements::INSERT_URL, new_path, url_to_shorten);
            if (result.affected_rows() == 1) {
                transaction.commit();
                return new_path;
            }
        } catch (const pqxx::unique_violation &e) {
            spdlog::warn(
                    "Somehow random generated new path is already present in database, increasing new path length (previous: {}).",
                    current_new_random_path_len);
            ++current_new_random_path_len;
        }
    }
}

void PostgresDBManager::shortenUrl(const std::string &new_path, const std::string &url_to_shorten) {
    auto connection = connection_pool.getObject();
    pqxx::work transaction{*connection};
    try {
        auto result = transaction.exec_prepared(PreparedStatements::INSERT_URL, new_path,
                                                url_to_shorten);
        if (result.affected_rows() == 1) {
            transaction.commit();
            return;
        } else {
            throw DatabaseManagerException(fmt::format("Path {} is forbidden!", new_path));
        }
    } catch (const pqxx::unique_violation &e) {
        throw DatabaseManagerException(fmt::format("Path {} is already taken!", new_path));
    }

}

std::optional<std::string> PostgresDBManager::getOriginalUrl(const std::string &shortened_path) {
    auto connection = connection_pool.getObject();
    pqxx::work transaction{*connection};
    auto result = transaction.exec_prepared(PreparedStatements::GET_ORIGINAL_URL, shortened_path);
    if (result.empty()) {
        return std::nullopt;
    }
    return result.at(0).at(0).as<std::string>();
}

bool PostgresDBManager::isForbidden(const std::string &new_path) {
    auto connection = connection_pool.getObject();
    pqxx::work transaction{*connection};
    auto result = transaction.exec_prepared(PreparedStatements::IS_PATH_FORBIDDEN, new_path);
    return result.at(0).at(0).as<std::size_t>() != 0;
}

void PostgresDBManager::addForbiddenPath(const std::string &forbidden_path) {
    auto connection = connection_pool.getObject();
    pqxx::work transaction{*connection};
    auto result = transaction.exec_prepared(PreparedStatements::ADD_FORBIDDEN_PATH, forbidden_path);
    if (result.affected_rows() > 0) {
        spdlog::info("Added forbidden path: {}", forbidden_path);
    }
    transaction.commit();
}
