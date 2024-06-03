#pragma once
#include "RequestConstants.hpp"

struct RAIIEnvs{
    RAIIEnvs() {
        setenv(environment::HTTP_BIND_ADDRESS, SERVER_ADDRESS.c_str(), 1);
        setenv(environment::WEB_FILES_DIRECTORY, WEB_FILES_DIR, 1);
        setenv(environment::MAIN_WEBSITE_FILENAME, "main.html", 1);
        setenv(environment::DOMAIN_NAME, SERVER_DOMAIN.c_str(), 1);
        setenv(environment::POSTGRES_DB, "url-shortener", 1);
        setenv(environment::POSTGRES_USER, "postgres", 1);
        setenv(environment::POSTGRES_PASSWORD, "root", 1);
        setenv(environment::POSTGRES_ADDRESS, "172.26.1.2", 1);
    }

    ~RAIIEnvs() {
        unsetenv(environment::HTTP_BIND_ADDRESS);
        unsetenv(environment::WEB_FILES_DIRECTORY);
        unsetenv(environment::MAIN_WEBSITE_FILENAME);
        unsetenv(environment::DOMAIN_NAME);
        unsetenv(environment::POSTGRES_DB);
        unsetenv(environment::POSTGRES_USER);
        unsetenv(environment::POSTGRES_PASSWORD);
        unsetenv(environment::POSTGRES_ADDRESS);
    }

    const std::string SERVER_ADDRESS{"http://127.0.0.1:27539"};
    const std::string SERVER_DOMAIN{"www.some_test_domain.pl"};
};