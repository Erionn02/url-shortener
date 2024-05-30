#include <gtest/gtest.h>

#include "URLShortenerFactory.hpp"

using namespace ::testing;

struct URLShortenerFactoryTests : public Test {
    void SetUp() override {
        setenv(environment::HTTP_BIND_ADDRESS, "http://127.0.0.1:27539", 1);
        setenv(environment::WEB_FILES_DIRECTORY, WEB_FILES_DIR, 1);
        setenv(environment::MAIN_WEBSITE_FILENAME, "main.html", 1);
    }

    void TearDown() override {
        unsetenv(environment::HTTP_BIND_ADDRESS);
        unsetenv(environment::WEB_FILES_DIRECTORY);
        unsetenv(environment::MAIN_WEBSITE_FILENAME);
    }
};


TEST_F(URLShortenerFactoryTests, canCreate) {
    ASSERT_NO_THROW(URLShortenerFactory::create());
}

TEST_F(URLShortenerFactoryTests, canOpen) {
    auto url_shortener = URLShortenerFactory::create();
    ASSERT_NO_THROW(url_shortener->open());
}