#pragma once

#include <fmt/format.h>

#include <string>

inline std::string getEnv(const std::string &env_name) {
    auto value = std::getenv(env_name.c_str());
    if (value == nullptr) {
        throw std::runtime_error(fmt::format("Environment name {} is not set, cannot retrieve value.", env_name));
    }
    return {value};
}

namespace environment {
    static constexpr const char *HTTP_BIND_ADDRESS{"http_bind_address"};
    static constexpr const char *WEB_FILES_DIRECTORY{"web_files_directory"};
    static constexpr const char *DOMAIN_NAME{"domain_name"};
    static constexpr const char *POSTGRES_DB{"POSTGRES_DB"};
    static constexpr const char *POSTGRES_USER{"POSTGRES_USER"};
    static constexpr const char *POSTGRES_PASSWORD{"POSTGRES_PASSWORD"};
    static constexpr const char *POSTGRES_ADDRESS{"POSTGRES_ADDRESS"};
};
