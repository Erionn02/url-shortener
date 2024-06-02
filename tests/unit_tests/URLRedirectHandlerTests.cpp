#include <gtest/gtest.h>

#include "handlers/URLRedirectHandler.hpp"
#include "DatabaseManagerMock.hpp"

#include <fstream>

using namespace ::testing;

struct URLRedirectHandlerTests : public Test {
    std::shared_ptr<DatabaseManagerMock> db_manager_mock{std::make_shared<DatabaseManagerMock>()};
    URLRedirectHandler handler{db_manager_mock};
};

TEST_F(URLRedirectHandlerTests, canHandleGETRequest) {
    EXPECT_CALL(*db_manager_mock, getOriginalUrl(_)).WillOnce(Return(""));
    web::http::http_request request{web::http::methods::GET};
    RequestData request_wrapper{request};
    ASSERT_TRUE(handler.canHandle(request_wrapper));
}

TEST_F(URLRedirectHandlerTests, cannotHandleOtherMethodThanGET) {
    web::http::http_request request{web::http::methods::POST};
    RequestData request_wrapper{request};
    ASSERT_FALSE(handler.canHandle(request_wrapper));
}

TEST_F(URLRedirectHandlerTests, setsRedirectionProperly) {
    std::string dummy_original_url{"https://www.google.com"};
    EXPECT_CALL(*db_manager_mock, getOriginalUrl(_)).WillOnce(Return(dummy_original_url));

    web::http::http_request request{web::http::methods::GET};
    RequestData request_wrapper{request};
    handler.doHandle(request_wrapper);

    EXPECT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::PermanentRedirect);
    auto result = request_wrapper.getResponse().headers().find("Location");
    ASSERT_NE(result, request_wrapper.getResponse().headers().end());
    ASSERT_EQ(result->second, dummy_original_url);
}

