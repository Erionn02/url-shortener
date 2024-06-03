#pragma once

#include "EnvironmentVariables.hpp"

#include <pqxx/transaction>

inline pqxx::connection getConnection() {
    auto postgres_db = getEnv(environment::POSTGRES_DB);
    auto postgres_user = getEnv(environment::POSTGRES_USER);
    auto postgres_password = getEnv(environment::POSTGRES_PASSWORD);
    auto postgres_address = getEnv(environment::POSTGRES_ADDRESS);
    return pqxx::connection{fmt::format("dbname={} user={} password={} host={} port={}", postgres_db, postgres_user,
                        postgres_password, postgres_address, 5432)};
}

inline void clearDatabase() {
    pqxx::connection conn{getConnection()};
    pqxx::work work{conn};
    work.exec("DELETE FROM urls;");
    work.exec("DELETE FROM forbidden_urls;");
    work.commit();
}