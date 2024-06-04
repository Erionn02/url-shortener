#include <gtest/gtest.h>

#include "DatabaseManager.hpp"
#include "URLShortenerFactory.hpp"
#include "DatabaseUtils.hpp"
#include "RAIIEnvs.hpp"

using namespace ::testing;

struct DatabaseManagerTests : public Test {
    RAIIEnvs envs;
    std::shared_ptr<PostgresDBManager> db_manager{URLShortenerFactory::createPostgresDatabaseManager()};
    pqxx::connection connection{getConnection()};
    const std::string EXAMPLE_URL_TO_SHORTEN{"https://github.com/Erionn02/url-shortener"};
    const std::string EXAMPLE_CUSTOM_PATH{"heyy_this_is/some/custom/path"};

    void SetUp() override {
        clearDatabase(); // just to make sure that the database is clear before every test
    }

    void TearDown() override {
        clearDatabase();
    }

    std::size_t getAmountOfEntriesInTable(const std::string &table_name) {
        pqxx::work work{connection};
        auto result = work.exec(fmt::format("SELECT COUNT(*) FROM {};", table_name));
        return result.at(0).at(0).as<std::size_t>();
    }
};


TEST_F(DatabaseManagerTests, doesNotReturnURLOnNonExistentShortenedPath) {
    auto url = db_manager->getOriginalUrl("lmaofowefwefwefIDonTExist");
    ASSERT_FALSE(url.has_value());
}

TEST_F(DatabaseManagerTests, URLIsForbiddenWhenAdded) {
    ASSERT_EQ(getAmountOfEntriesInTable("forbidden_urls"), 0);
    ASSERT_FALSE(db_manager->isForbidden(EXAMPLE_CUSTOM_PATH));
    db_manager->addForbiddenPath(EXAMPLE_CUSTOM_PATH);
    ASSERT_EQ(getAmountOfEntriesInTable("forbidden_urls"), 1);
    ASSERT_TRUE(db_manager->isForbidden(EXAMPLE_CUSTOM_PATH));
}

TEST_F(DatabaseManagerTests, doesNotThrowWhenTriedToAddTheSameForbidenURLTwice) {
    db_manager->addForbiddenPath(EXAMPLE_CUSTOM_PATH);
    ASSERT_EQ(getAmountOfEntriesInTable("forbidden_urls"), 1);
    ASSERT_NO_THROW(db_manager->addForbiddenPath(EXAMPLE_CUSTOM_PATH));
    ASSERT_EQ(getAmountOfEntriesInTable("forbidden_urls"), 1);
}

TEST_F(DatabaseManagerTests, canShortenURLAndRetrieve) {
    ASSERT_EQ(getAmountOfEntriesInTable("urls"), 0);
    auto new_path = db_manager->shortenUrl(EXAMPLE_URL_TO_SHORTEN);
    ASSERT_EQ(getAmountOfEntriesInTable("urls"), 1);
    auto url = db_manager->getOriginalUrl(new_path);
    ASSERT_TRUE(url.has_value());
    ASSERT_EQ(*url, EXAMPLE_URL_TO_SHORTEN);
}

TEST_F(DatabaseManagerTests, canShortenURLToCustomPathAndRetrieve) {
    ASSERT_EQ(getAmountOfEntriesInTable("urls"), 0);
    db_manager->shortenUrl(EXAMPLE_CUSTOM_PATH, EXAMPLE_URL_TO_SHORTEN);
    ASSERT_EQ(getAmountOfEntriesInTable("urls"), 1);
    auto url = db_manager->getOriginalUrl(EXAMPLE_CUSTOM_PATH);
    ASSERT_TRUE(url.has_value());
    ASSERT_EQ(*url, EXAMPLE_URL_TO_SHORTEN);
}

TEST_F(DatabaseManagerTests, cannotUseTheSamePathTwice) {
    db_manager->shortenUrl(EXAMPLE_CUSTOM_PATH, EXAMPLE_URL_TO_SHORTEN);
    ASSERT_THROW(db_manager->shortenUrl(EXAMPLE_CUSTOM_PATH, EXAMPLE_URL_TO_SHORTEN), DatabaseManagerException);
}