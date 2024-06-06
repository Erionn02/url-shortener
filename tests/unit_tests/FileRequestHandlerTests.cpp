#include <gtest/gtest.h>

#include "handlers/FileRequestHandler.hpp"

#include <fstream>

struct FileRequestHandlerTests : public testing::Test {
    std::filesystem::path path{WEB_FILES_DIR};
    std::string main_website_filename{WEB_FILES_DIR "/index.html"};

    void TearDown() override {
        if (path != WEB_FILES_DIR) {
            std::filesystem::remove_all(path);
        }
    }

    FileRequestHandler create() {
        return {path, main_website_filename};
    }

    std::string readFileContent(const std::filesystem::path &path) {
        std::ifstream file{path};
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
};

TEST_F(FileRequestHandlerTests, throwsOnNonExistentDirectory) {
    path = "egmrngeiirg";
    ASSERT_THROW(create(), RequestHandlerException);
}

TEST_F(FileRequestHandlerTests, throwsOnEmptyDirectory) {
    path = std::filesystem::temp_directory_path() / "some_new_dir";
    std::filesystem::create_directories(path);
    ASSERT_THROW(create(), RequestHandlerException);
}

TEST_F(FileRequestHandlerTests, throwsOnFileInsteadOfDir) {
    path = std::filesystem::temp_directory_path() / "some_new_file";
    std::ofstream file{path};
    ASSERT_THROW(create(), RequestHandlerException);
}

TEST_F(FileRequestHandlerTests, throwsOnNonExistentMainWebsiteFile) {
    main_website_filename = std::filesystem::temp_directory_path() / "some_new_file";
    ASSERT_THROW(create(), RequestHandlerException);
}

TEST_F(FileRequestHandlerTests, canHandleGETRequestsForAllFilesInGivenDirectory) {
    FileRequestHandler handler{create()};

    for (const auto &dir_entry: std::filesystem::directory_iterator(path)) {
        web::http::http_request request{web::http::methods::GET};
        request.set_request_uri("/" + dir_entry.path().filename().string());
        RequestData request_wrapper{request};
        ASSERT_TRUE(handler.canHandle(request_wrapper));
    }
    web::http::http_request request{web::http::methods::GET};
    request.set_request_uri("/");
    RequestData request_wrapper{request};
    ASSERT_TRUE(handler.canHandle(request_wrapper));
}

TEST_F(FileRequestHandlerTests, canHandleDefaultRequest) {
    FileRequestHandler handler{create()};

    web::http::http_request request{web::http::methods::GET};
    request.set_request_uri("/");
    RequestData request_wrapper{request};
    ASSERT_TRUE(handler.canHandle(request_wrapper));
}

TEST_F(FileRequestHandlerTests, cannotHandleRequestsOtherThanGET) {
    FileRequestHandler handler{create()};

    web::http::http_request request{web::http::methods::POST};
    request.set_request_uri("/");
    RequestData request_wrapper{request};
    ASSERT_FALSE(handler.canHandle(request_wrapper));
}

TEST_F(FileRequestHandlerTests, cannotHandleRequestForNonExistentFile) {
    FileRequestHandler handler{create()};

    web::http::http_request request{web::http::methods::GET};
    request.set_request_uri("/some_file_that_does_not_exist_on_server.txt");
    RequestData request_wrapper{request};
    ASSERT_FALSE(handler.canHandle(request_wrapper));
}

TEST_F(FileRequestHandlerTests, forwardsDefaultRequestToMainWebsiteFile) {
    FileRequestHandler handler{create()};

    web::http::http_request request{web::http::methods::GET};
    request.set_request_uri("/");
    RequestData request_wrapper{request};
    handler.handle(request_wrapper);
    ASSERT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::OK);
    ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), readFileContent(main_website_filename));
}

TEST_F(FileRequestHandlerTests, handlesRequestsForFilesThatExist) {
    FileRequestHandler handler{create()};

    for (const auto &dir_entry: std::filesystem::directory_iterator(path)) {
        web::http::http_request request{web::http::methods::GET};
        request.set_request_uri("/" + dir_entry.path().filename().string());
        RequestData request_wrapper{request};
        handler.handle(request_wrapper);
        ASSERT_EQ(request_wrapper.getResponse().status_code(), web::http::status_codes::OK);
        ASSERT_EQ(request_wrapper.getResponse().extract_string().get(), readFileContent(dir_entry.path()));
    }
}