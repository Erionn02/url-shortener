#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "handlers/URLShortenerHandler.hpp"
#include "RequestConstants.hpp"
#include "DatabaseManagerMock.hpp"

#include <fstream>


using namespace ::testing;


struct URLShortenerHandlerTests : public Test {
    std::string server_dummy_host{"www.this_is_dummy_host.com:3451"};
    std::shared_ptr<DatabaseManagerMock> db_mock{std::make_shared<DatabaseManagerMock>()};
    URLShortenerHandler handler{db_mock, server_dummy_host};
    web::http::http_request default_request{web::http::methods::POST};

    void SetUp() override {
        ON_CALL(*db_mock, isForbidden(_)).WillByDefault(Return(false));
        default_request.set_request_uri(URLShortenerHandler::HANDLER_URI);
    }
};

TEST_F(URLShortenerHandlerTests, canHandlePOSTRequestForVersionPath) {
    RequestData request_wrapper{default_request};
    ASSERT_TRUE(handler.canHandle(request_wrapper));
}

TEST_F(URLShortenerHandlerTests, cannotHandleOtherMethodThanPOST) {
    web::http::http_request request{web::http::methods::GET};
    request.set_request_uri(URLShortenerHandler::HANDLER_URI);
    RequestData request_wrapper{request};
    ASSERT_FALSE(handler.canHandle(request_wrapper));
}

TEST_F(URLShortenerHandlerTests, cannotHandleOtherPathThanHandlersURI) {
    web::http::http_request request{web::http::methods::POST};
    request.set_request_uri("/some_random_path_that_is/not_handled");
    RequestData request_wrapper{request};
    ASSERT_FALSE(handler.canHandle(request_wrapper));
}

TEST_F(URLShortenerHandlerTests, badRequestWhenMissingURLToShortenHeader) {
    RequestData request_wrapper{default_request};
    handler.handle(request_wrapper);

    ASSERT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::BadRequest);
    ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), requests::errors::URL_TO_SHORTEN_MISSING);
}

TEST_F(URLShortenerHandlerTests, badRequestWhenURLToShortenIsNotAProperURL) {
    default_request.headers().add(requests::headers::URL_TO_SHORTEN, "wubiefiwebuf");
    RequestData request_wrapper{default_request};
    handler.handle(request_wrapper);

    ASSERT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::BadRequest);
    ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), requests::errors::GIVEN_URL_IS_INVALID_OR_NOT_RESPONSIVE);
}

TEST_F(URLShortenerHandlerTests, badRequestWhenURLToShortenIsValidButUnresponsive) {
    default_request.headers().add(requests::headers::URL_TO_SHORTEN, "www.wsnfuweiowigbwuei1misdnuvnbed.com");
    RequestData request_wrapper{default_request};
    handler.handle(request_wrapper);

    ASSERT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::BadRequest);
    ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), requests::errors::GIVEN_URL_IS_INVALID_OR_NOT_RESPONSIVE);
}

TEST_F(URLShortenerHandlerTests, OKWhenURLIsOK) {
    std::array example_url_types{"www.google.com", "google.com", "http://google.com", "https://google.com",
                                 "http://www.google.com", "https://www.google.com"};

    for (const auto &example_url: example_url_types) {
        web::http::http_request request{web::http::methods::POST};
        request.set_request_uri(URLShortenerHandler::HANDLER_URI);
        request.headers().add(requests::headers::URL_TO_SHORTEN, example_url);
        RequestData request_wrapper{request};
        handler.handle(request_wrapper);

        EXPECT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::OK);
        EXPECT_TRUE(request_wrapper.getResponse().extract_string().get().starts_with(server_dummy_host));
    }
}

TEST_F(URLShortenerHandlerTests, canSetACustomURL) {
    const std::string custom_url{"my_custom_url"};
    default_request.headers().add(requests::headers::URL_TO_SHORTEN, "www.google.com");
    default_request.headers().add(requests::headers::CUSTOM_PATH, custom_url);
    RequestData request_wrapper{default_request};
    EXPECT_CALL(*db_mock, shortenUrl(_,_));


    handler.handle(request_wrapper);

    ASSERT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::OK);
    ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), fmt::format("{}/{}", server_dummy_host, custom_url));
}

TEST_F(URLShortenerHandlerTests, cannotSetTheSameCustomURLTwice) {
    const std::string custom_url{"my_custom_url"};
    default_request.headers().add(requests::headers::URL_TO_SHORTEN, "www.google.com");
    default_request.headers().add(requests::headers::CUSTOM_PATH, custom_url);
    RequestData request_wrapper{default_request};
    EXPECT_CALL(*db_mock, shortenUrl(_,_)).WillOnce(Return()).WillOnce(Throw(DatabaseManagerException{requests::errors::GIVEN_URL_ALREADY_EXISTS}));


    handler.handle(request_wrapper);

    ASSERT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::OK);
    ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), fmt::format("{}/{}", server_dummy_host, custom_url));

    handler.handle(request_wrapper);

    ASSERT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::BadRequest);
    ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), requests::errors::GIVEN_URL_ALREADY_EXISTS);
}

TEST_F(URLShortenerHandlerTests, cannotSetCustomPathThatIsTooLong) {
    const std::string custom_url(static_cast<std::size_t>(URLShortenerHandler::MAX_CUSTOM_PATH_LENGTH + 1), 'a');
    default_request.headers().add(requests::headers::URL_TO_SHORTEN, "www.google.com");
    default_request.headers().add(requests::headers::CUSTOM_PATH, custom_url);
    RequestData request_wrapper{default_request};


    handler.handle(request_wrapper);

    ASSERT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::BadRequest);
    ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), requests::errors::GIVEN_CUSTOM_PATH_IS_TOO_LONG);
}

TEST_F(URLShortenerHandlerTests, cannotSetForbiddenURL) {
    EXPECT_CALL(*db_mock, isForbidden(_)).WillOnce(Return(true));
    const std::string custom_path{"my_custom_path"};
    default_request.headers().add(requests::headers::URL_TO_SHORTEN, "www.google.com");
    default_request.headers().add(requests::headers::CUSTOM_PATH, custom_path);
    RequestData request_wrapper{default_request};


    handler.handle(request_wrapper);

    ASSERT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::BadRequest);
    ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), requests::errors::GIVEN_CUSTOM_URL_IS_FORBIDDEN);
}

TEST_F(URLShortenerHandlerTests, cannotShortenURLThatLeadsToThisServer) {
    const std::string custom_path{"my_custom_path"};
    default_request.headers().add(requests::headers::URL_TO_SHORTEN, server_dummy_host);
    default_request.headers().add(requests::headers::CUSTOM_PATH, custom_path);
    RequestData request_wrapper{default_request};


    handler.handle(request_wrapper);

    ASSERT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::BadRequest);
    ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), requests::errors::URL_TO_SHORTEN_CANNOT_LEAD_TO_THIS_SERVER);
}



