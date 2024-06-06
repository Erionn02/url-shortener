#include <gtest/gtest.h>

#include "handlers/NotFoundHandler.hpp"
#include "Utils.hpp"

#include <fstream>

TEST(NotFoundHandlerTests, throwsWhenGivenDoesNotExist) {
    ASSERT_THROW(NotFoundHandler{WEB_FILES_DIR"/does_not_exist_file.html"}, RequestHandlerException);
}

TEST(NotFoundHandlerTests, throwsWhenGivenPathIsDirectory) {
    ASSERT_THROW(NotFoundHandler{WEB_FILES_DIR}, RequestHandlerException);
}

TEST(NotFoundHandlerTests, throwsWhenGivenPathIsNotHTMLFile) {
    ASSERT_THROW(NotFoundHandler{WEB_FILES_DIR"/file_1.txt"}, RequestHandlerException);
}

TEST(NotFoundHandlerTests, canCreateOnWithHTMLFile) {
    ASSERT_NO_THROW(NotFoundHandler{WEB_FILES_DIR"/404.html"});
}

TEST(NotFoundHandlerTests, canHandleAnyRequest) {
    NotFoundHandler handler{WEB_FILES_DIR"/404.html"};


    web::http::http_request request{};
    RequestData request_wrapper{request};
    ASSERT_TRUE(handler.canHandle(request_wrapper));
}

TEST(NotFoundHandlerTests, cannotHandleOtherMethodThanGET) {
    NotFoundHandler handler{WEB_FILES_DIR"/404.html"};


    web::http::http_request request{};
    RequestData request_wrapper{request};
    handler.handle(request_wrapper);

    ASSERT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::NotFound);
    ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), utils::readFileContent(WEB_FILES_DIR"/404.html"));
}


