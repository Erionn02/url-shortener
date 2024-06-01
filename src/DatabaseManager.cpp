#include "DatabaseManager.hpp"

#include <fmt/format.h>
#include <pqxx/transaction>

#include <random>

DatabaseManager::DatabaseManager(const std::string &database_address, const std::string &pg_user,
                                 const std::string &pg_password, const std::string &database_name) :
        connection(fmt::format("dbname={} user={} password={} host={} port={}", database_name, pg_user, pg_password,
                               database_address, 5432)) {
    connection.prepare(PreparedStatements::INSERT_WITH_RANDOM_KEY, "INSERT INTO urls VALUES($1, $2)"); // todo generate hash in database
    connection.prepare(PreparedStatements::INSERT_WITH_SPECIFIED_KEY, "INSERT INTO urls VALUES($1, $2)");
    connection.prepare(PreparedStatements::GET_ORIGINAL_URL, "SELECT original_url FROM urls WHERE shortened_version = $1");
}

std::string DatabaseManager::shortenUrl(const std::string &url_to_shorten) {
    auto new_path = generateRandomString(DEFAULT_NEW_PATH_LEN);
    pqxx::work transaction{connection};
    transaction.exec_prepared(PreparedStatements::INSERT_WITH_RANDOM_KEY, new_path, url_to_shorten);
    transaction.commit();
    return new_path;
}

void DatabaseManager::shortenUrl(const std::string &new_path, const std::string &url_to_shorten) {
    pqxx::work transaction{connection};
    transaction.exec_prepared(PreparedStatements::INSERT_WITH_SPECIFIED_KEY, new_path, url_to_shorten);
    transaction.commit();
}

std::string DatabaseManager::generateRandomString(std::size_t length) {
    static auto& chrs = "0123456789"
                        "abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{std::random_device{}()};
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string s;

    s.reserve(length);

    while(length--)
        s += chrs[pick(rg)];

    return s;
}

std::optional<std::string> DatabaseManager::getOriginalUrl(const std::string &shortened_path) {
    pqxx::work transaction{connection};
    auto result = transaction.exec_prepared(PreparedStatements::GET_ORIGINAL_URL, shortened_path);
    if (result.empty()) {
        return std::nullopt;
    }
    return result.at(0).at(0).as<std::string>();
}
