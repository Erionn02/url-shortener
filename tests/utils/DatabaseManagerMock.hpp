#pragma once

#include <gmock/gmock.h>

#include "DatabaseManager.hpp"


class DatabaseManagerMock : public DatabaseManager {
public:
    MOCK_METHOD(std::optional<std::string>, getOriginalUrl, (const std::string&), (override));
    MOCK_METHOD(std::string, shortenUrl, (const std::string&), (override));
    MOCK_METHOD(void, shortenUrl, (const std::string&, const std::string&), (override));
    MOCK_METHOD(bool, isForbidden, (const std::string&), (override));
    MOCK_METHOD(void, addForbiddenPath, (const std::string&), (override));
};
