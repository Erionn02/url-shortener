#include <gtest/gtest.h>

#include "HTTPServer.hpp"
#include "URLShortenerFactory.hpp"
#include "RAIIEnvs.hpp"
#include "DatabaseUtils.hpp"

#include <cpprest/http_client.h>

using namespace ::testing;

struct URLShortenerServerTests : public Test {
    const std::string EXAMPLE_URL_TO_SHORTEN{"https://github.com/Erionn02/url-shortener"};
    const std::string EXAMPLE_CUSTOM_PATH{"heyy_this_is/some/custom/path"};
    RAIIEnvs envs;
    std::unique_ptr<HTTPServer> url_shortener_server;

    void SetUp() override {
        url_shortener_server = URLShortenerFactory::create();
        url_shortener_server->open();
    }

    void TearDown() override {
        clearDatabase();
    }
};

TEST_F(URLShortenerServerTests, canRequestVersion) {
    web::http::client::http_client client{envs.SERVER_ADDRESS};
    auto response = client.request(web::http::methods::GET, APIVersionHandler::HANDLER_URI).get();

    ASSERT_EQ(APIVersionHandler::CURRENT_API_VERSION, response.extract_string().get());
}

TEST_F(URLShortenerServerTests, canRequestMainPage) {
    web::http::client::http_client client{envs.SERVER_ADDRESS};
    auto response = client.request(web::http::methods::GET).get();

    ASSERT_TRUE(response.extract_string().get().starts_with("<!doctype html>"));
}

TEST_F(URLShortenerServerTests, canRequestMainPageExplicitly) {
    web::http::client::http_client client{envs.SERVER_ADDRESS};
    auto response = client.request(web::http::methods::GET, "/main.html").get();

    ASSERT_TRUE(response.extract_string().get().starts_with("<!doctype html>"));
}

TEST_F(URLShortenerServerTests, canShortenURLAndThenRetrieveOriginal) {
    web::http::client::http_client_config config{};
    config.set_max_redirects(0);
    web::http::client::http_client client{envs.SERVER_ADDRESS, config};

    web::http::http_request request{web::http::methods::POST};
    request.set_request_uri(URLShortenerHandler::HANDLER_URI);
    request.headers().add(requests::headers::URL_TO_SHORTEN, EXAMPLE_URL_TO_SHORTEN);
    auto response = client.request(request).get();

    ASSERT_EQ(response.status_code(), web::http::status_codes::OK);
    auto shortened_version = response.extract_string().get();
    ASSERT_TRUE(shortened_version.starts_with(envs.SERVER_DOMAIN));

    auto only_path = shortened_version.substr(envs.SERVER_DOMAIN.length() + 1);
    response = client.request(web::http::methods::GET, only_path).get();
    ASSERT_EQ(response.status_code(), web::http::status_codes::PermanentRedirect);
    ASSERT_TRUE(response.headers().has("Location"));

    // returns original url
    ASSERT_EQ(response.headers().find("Location")->second, EXAMPLE_URL_TO_SHORTEN);
}

TEST_F(URLShortenerServerTests, canShortenURLToCustomAndThenRetrieveOriginal) {
    web::http::client::http_client_config config{};
    config.set_max_redirects(0);
    web::http::client::http_client client{envs.SERVER_ADDRESS, config};

    web::http::http_request request{web::http::methods::POST};
    request.set_request_uri(URLShortenerHandler::HANDLER_URI);
    request.headers().add(requests::headers::URL_TO_SHORTEN, EXAMPLE_URL_TO_SHORTEN);
    request.headers().add(requests::headers::CUSTOM_URL, EXAMPLE_CUSTOM_PATH);
    auto response = client.request(request).get();

    ASSERT_EQ(response.status_code(), web::http::status_codes::OK);
    auto shortened_version = response.extract_string().get();
    ASSERT_EQ(shortened_version, fmt::format("{}/{}", envs.SERVER_DOMAIN, EXAMPLE_CUSTOM_PATH));
    response = client.request(web::http::methods::GET, EXAMPLE_CUSTOM_PATH).get();

    ASSERT_EQ(response.status_code(), web::http::status_codes::PermanentRedirect);
    ASSERT_TRUE(response.headers().has("Location"));

    // returns original url
    ASSERT_EQ(response.headers().find("Location")->second, EXAMPLE_URL_TO_SHORTEN);
}
