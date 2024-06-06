#include <gtest/gtest.h>

#include "HTTPServer.hpp"
#include "URLShortenerFactory.hpp"
#include "RAIIEnvs.hpp"
#include "DatabaseUtils.hpp"
#include "Utils.hpp"

#include <cpprest/http_client.h>

using namespace ::testing;

struct URLShortenerServerTests : public Test {
    const std::string EXAMPLE_URL_TO_SHORTEN{"https://github.com/Erionn02/url-shortener"};
    const std::string EXAMPLE_CUSTOM_PATH{"heyy_this_is/some/custom/path"};
    RAIIEnvs envs;
    std::unique_ptr<HTTPServer> url_shortener_server;

    void SetUp() override {
        clearDatabase();
        url_shortener_server = URLShortenerFactory::create();
        url_shortener_server->open();
    }

    void TearDown() override {
        clearDatabase();
    }

    web::http::http_request createRequestWithCustomPath(const std::string& custom_path) {
        web::http::http_request request{web::http::methods::POST};
        request.set_request_uri(URLShortenerHandler::HANDLER_URI);
        request.headers().add(requests::headers::URL_TO_SHORTEN, EXAMPLE_URL_TO_SHORTEN);
        request.headers().add(requests::headers::CUSTOM_PATH, custom_path);
        return request;
    }

    web::http::http_request createRequestWithCustomPath() {
        return createRequestWithCustomPath(EXAMPLE_CUSTOM_PATH);
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

    ASSERT_EQ(response.extract_string().get(), utils::readFileContent(WEB_FILES_DIR"/index.html"));
}

TEST_F(URLShortenerServerTests, canRequestMainPageExplicitly) {
    web::http::client::http_client client{envs.SERVER_ADDRESS};
    auto response = client.request(web::http::methods::GET, "/index.html").get();

    ASSERT_EQ(response.extract_string().get(), utils::readFileContent(WEB_FILES_DIR"/index.html"));
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

    auto response = client.request(createRequestWithCustomPath()).get();

    ASSERT_EQ(response.status_code(), web::http::status_codes::OK);
    auto shortened_version = response.extract_string().get();
    ASSERT_EQ(shortened_version, fmt::format("{}/{}", envs.SERVER_DOMAIN, EXAMPLE_CUSTOM_PATH));
    response = client.request(web::http::methods::GET, EXAMPLE_CUSTOM_PATH).get();

    ASSERT_EQ(response.status_code(), web::http::status_codes::PermanentRedirect);
    ASSERT_TRUE(response.headers().has("Location"));

    // returns original url
    ASSERT_EQ(response.headers().find("Location")->second, EXAMPLE_URL_TO_SHORTEN);
}

TEST_F(URLShortenerServerTests, badRequestWhenTriesToShortenTwiceToTheSamePath) {
    web::http::client::http_client client{envs.SERVER_ADDRESS};

    auto response = client.request(createRequestWithCustomPath()).get();

    ASSERT_EQ(response.status_code(), web::http::status_codes::OK);
    response = client.request(createRequestWithCustomPath()).get();
    ASSERT_EQ(response.status_code(), web::http::status_codes::BadRequest);
}

TEST_F(URLShortenerServerTests, badRequestWhenTriesToShortenURLToForbiddenPath) {
    web::http::client::http_client client{envs.SERVER_ADDRESS};

    for (const auto& forbidden_path: {APIVersionHandler::HANDLER_URI, URLShortenerHandler::HANDLER_URI}) {
        auto response = client.request(createRequestWithCustomPath(forbidden_path)).get();
        ASSERT_EQ(response.status_code(), web::http::status_codes::BadRequest);
    }
}

TEST_F(URLShortenerServerTests, badRequestWhenTriesToShortenURLThatLeadsToThisServer) {
    web::http::client::http_client client{envs.SERVER_ADDRESS};
    web::http::http_request request{web::http::methods::POST};
    request.set_request_uri(URLShortenerHandler::HANDLER_URI);
    request.headers().add(requests::headers::URL_TO_SHORTEN, envs.SERVER_DOMAIN);

    auto response = client.request(request).get();

    ASSERT_EQ(response.status_code(), web::http::status_codes::BadRequest);
    ASSERT_EQ(response.extract_string().get(), requests::errors::URL_TO_SHORTEN_CANNOT_LEAD_TO_THIS_SERVER);
}

TEST_F(URLShortenerServerTests, notFoundOnNonExistentURL) {
    web::http::client::http_client client{envs.SERVER_ADDRESS};
    web::http::http_request request{web::http::methods::GET};
    request.set_request_uri("wergnuwegwegwegwebsdbdsb");

    auto response = client.request(request).get();

    ASSERT_EQ(response.status_code(), web::http::status_codes::NotFound);
    ASSERT_EQ(response.extract_string().get(), utils::readFileContent(WEB_FILES_DIR"/404.html"));
}