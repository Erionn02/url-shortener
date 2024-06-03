#include <gtest/gtest.h>

#include "URLShortenerFactory.hpp"
#include "RAIIEnvs.hpp"

using namespace ::testing;

struct URLShortenerFactoryTests : public Test {
    RAIIEnvs envs;
};


TEST_F(URLShortenerFactoryTests, canCreate) {
    ASSERT_NO_THROW(URLShortenerFactory::create());
}

TEST_F(URLShortenerFactoryTests, canOpen) {
    auto url_shortener = URLShortenerFactory::create();
    ASSERT_NO_THROW(url_shortener->open());
}