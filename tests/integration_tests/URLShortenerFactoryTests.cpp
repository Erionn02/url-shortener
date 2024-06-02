#include <gtest/gtest.h>

#include "URLShortenerFactory.hpp"

using namespace ::testing;

struct URLShortenerFactoryTests : public Test {
    void SetUp() override {
        setenv(environment::HTTP_BIND_ADDRESS, "http://127.0.0.1:27539", 1);
        setenv(environment::WEB_FILES_DIRECTORY, WEB_FILES_DIR, 1);
        setenv(environment::MAIN_WEBSITE_FILENAME, "main.html", 1);
        setenv(environment::DOMAIN_NAME, "www.some_test_domain.pl", 1);
        setenv(environment::POSTGRES_DB, "url-shortener", 1);
        setenv(environment::POSTGRES_USER, "postgres", 1);
        setenv(environment::POSTGRES_PASSWORD, "root", 1);
        setenv(environment::POSTGRES_ADDRESS, "172.26.1.2", 1);
    }

    void TearDown() override {
        unsetenv(environment::HTTP_BIND_ADDRESS);
        unsetenv(environment::WEB_FILES_DIRECTORY);
        unsetenv(environment::MAIN_WEBSITE_FILENAME);
        unsetenv(environment::DOMAIN_NAME);
        unsetenv(environment::POSTGRES_DB);
        unsetenv(environment::POSTGRES_USER);
        unsetenv(environment::POSTGRES_PASSWORD);
        unsetenv(environment::POSTGRES_ADDRESS);
    }
};


TEST_F(URLShortenerFactoryTests, canCreate) {
    ASSERT_NO_THROW(URLShortenerFactory::create());
}

TEST_F(URLShortenerFactoryTests, canOpen) {
    auto url_shortener = URLShortenerFactory::create();
    ASSERT_NO_THROW(url_shortener->open());
}