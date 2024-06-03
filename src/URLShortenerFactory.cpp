#include "URLShortenerFactory.hpp"

#include <spdlog/spdlog.h>

std::unique_ptr<HTTPServer> URLShortenerFactory::create() {
    auto bind_address = getEnv(environment::HTTP_BIND_ADDRESS);

    spdlog::info("Creating HTTP server with bind address {}", bind_address);
    auto http_server = std::make_unique<HTTPServer>(std::move(bind_address));

    http_server->addHandler(createAPIVersionHandler());
    http_server->addHandler(createURLRedirectHandler());
    http_server->addHandler(createFileRequestHandler());
    http_server->addHandler(createURLShortenerHandler());
    // todo add forbidden urls
    return http_server;
}

std::shared_ptr<PostgresDBManager> URLShortenerFactory::createPostgresDatabaseManager() {
    auto postgres_db = getEnv(environment::POSTGRES_DB);
    auto postgres_user = getEnv(environment::POSTGRES_USER);
    auto postgres_password = getEnv(environment::POSTGRES_PASSWORD);
    auto postgres_address = getEnv(environment::POSTGRES_ADDRESS);
    spdlog::info("Creating DatabaseManager with address {} and database: {}", postgres_address, postgres_db);

    return std::make_shared<PostgresDBManager>(postgres_address, postgres_user, postgres_password, postgres_db);
}

std::unique_ptr<FileRequestHandler> URLShortenerFactory::createFileRequestHandler() {
    auto web_files_directory = getEnv(environment::WEB_FILES_DIRECTORY);
    auto main_website_filename = getEnv(environment::MAIN_WEBSITE_FILENAME);
    spdlog::info("Creating FileRequestHandler with {} web files directory and main website filename: {}.",
                 web_files_directory, main_website_filename);
    auto file_request_handler = std::make_unique<FileRequestHandler>(std::move(web_files_directory),
                                         std::move(main_website_filename));
    return file_request_handler;
}

std::unique_ptr<URLShortenerHandler> URLShortenerFactory::createURLShortenerHandler() {
    auto server_domain_name = getEnv(environment::DOMAIN_NAME);
    spdlog::info("Creating URLShortenerHandler with {} server domain.", server_domain_name);

    return std::make_unique<URLShortenerHandler>(createPostgresDatabaseManager(), server_domain_name);
}

std::unique_ptr<URLRedirectHandler> URLShortenerFactory::createURLRedirectHandler() {
    return std::make_unique<URLRedirectHandler>(createPostgresDatabaseManager());
}

std::unique_ptr<APIVersionHandler> URLShortenerFactory::createAPIVersionHandler() {
    return std::make_unique<APIVersionHandler>();
}

