#include "URLShortenerFactory.hpp"
#include "FileRequestHandler.hpp"

#include <spdlog/spdlog.h>

std::unique_ptr<HTTPServer> URLShortenerFactory::create() {
    auto bind_address = getEnv(environment::HTTP_BIND_ADDRESS);

    spdlog::info("Creating HTTP server with bind address {}", bind_address);
    auto http_server = std::make_unique<HTTPServer>(std::move(bind_address));

    auto web_files_directory = getEnv(environment::WEB_FILES_DIRECTORY);
    auto main_website_filename = getEnv(environment::MAIN_WEBSITE_FILENAME);
    spdlog::info("Creating FileRequestHandler with {} web files directory and main website filename: {}.",
                 web_files_directory, main_website_filename);
    http_server->addHandler(std::make_unique<FileRequestHandler>(std::move(web_files_directory),
                                                                 std::move(main_website_filename)));
    return http_server;
}
