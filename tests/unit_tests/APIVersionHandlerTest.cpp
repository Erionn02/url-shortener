#include <gtest/gtest.h>

#include "handlers/APIVersionHandler.hpp"

#include <fstream>

TEST(APIVersionHandlerTests, canCreate) {
    ASSERT_NO_THROW(APIVersionHandler{});
}

TEST(APIVersionHandlerTests, canHandleGetRequestForVersionPath) {
    APIVersionHandler handler{};

    web::http::http_request request{web::http::methods::GET};
    request.set_request_uri(APIVersionHandler::HANDLER_URI);
    RequestData request_wrapper{request};
    ASSERT_TRUE(handler.canHandle(request_wrapper));
}

TEST(APIVersionHandlerTests, cannotHandleOtherMethodThanGET) {
    APIVersionHandler handler{};

    web::http::http_request request{web::http::methods::POST};
    request.set_request_uri(APIVersionHandler::HANDLER_URI);
    RequestData request_wrapper{request};
    ASSERT_FALSE(handler.canHandle(request_wrapper));
}

TEST(APIVersionHandlerTests, cannotHandleOtherPathThanHandlersURI) {
    APIVersionHandler handler{};

    web::http::http_request request{web::http::methods::GET};
    request.set_request_uri("/some_random_path_that_is/not_handled");
    RequestData request_wrapper{request};
    ASSERT_FALSE(handler.canHandle(request_wrapper));
}

TEST(APIVersionHandlerTests, returnsAPIVersion) {
    APIVersionHandler handler{};

    web::http::http_request request{web::http::methods::GET};
    request.set_request_uri(APIVersionHandler::HANDLER_URI);
    RequestData request_wrapper{request};
    handler.handle(request_wrapper);

    ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), APIVersionHandler::CURRENT_API_VERSION);
}



