#include "URLShortenerFactory.hpp"

#include <spdlog/spdlog.h>

std::unique_ptr<HTTPServer> URLShortenerFactory::create() {
    auto db_manager = createPostgresDatabaseManager();
    addAllForbiddenPaths(*db_manager);
    return create(std::move(db_manager));
}

std::unique_ptr<HTTPServer> URLShortenerFactory::create(std::shared_ptr<PostgresDBManager> db_manager) {
    auto bind_address = getEnv(environment::HTTP_BIND_ADDRESS);

    std::unique_ptr<HTTPServer> http_server;
    if (isEnvSet(environment::ENABLE_KAFKA) && getEnv(environment::ENABLE_KAFKA) == "true") {
        const std::string brokers = getEnv(environment::KAFKA_BROKERS);;
        std::string topic{getEnv(environment::KAFKA_TOPIC)};
        spdlog::info("Creating KafkaReportingHttpServer with bind address: {}, topic: {}, brokers: {}", bind_address, topic, brokers);

        const kafka::Properties props({
            {"bootstrap.servers", brokers},
            {"security.protocol", {"SASL_SSL"}},
            {"sasl.mechanisms", {"PLAIN"}},
            {"sasl.username", {environment::KAFKA_USER}},
            {"sasl.password", {environment::KAFKA_PASSWORD}},
        });
        http_server =  std::make_unique<KafkaReportingHttpServer>(std::move(bind_address), props, topic);
    } else {
        spdlog::info("Creating HTTP HttpServerType with bind address {}", bind_address);
        http_server = std::make_unique<HTTPServer>(std::move(bind_address));
    }

    http_server->addHandler(createAPIVersionHandler());
    http_server->addHandler(createURLRedirectHandler(db_manager));
    http_server->addHandler(createFileRequestHandler());
    http_server->addHandler(createURLShortenerHandler(db_manager));
    http_server->addHandler(createNotFoundHandler());

    return http_server;
}


void URLShortenerFactory::addAllForbiddenPaths(PostgresDBManager &db_manager) {
    db_manager.addForbiddenPath(APIVersionHandler::HANDLER_URI);
    db_manager.addForbiddenPath(URLShortenerHandler::HANDLER_URI);
    auto web_files_directory = getEnv(environment::WEB_FILES_DIRECTORY);
    for(const auto& dir_entry: std::filesystem::directory_iterator(web_files_directory)) {
        db_manager.addForbiddenPath(dir_entry.path().filename().string());
    }
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
    auto main_website_filename = web_files_directory + "/index.html";
    spdlog::info("Creating FileRequestHandler with {} web files directory and main website filename: {}.",
                 web_files_directory, main_website_filename);
    auto file_request_handler = std::make_unique<FileRequestHandler>(std::move(web_files_directory),
                                         std::move(main_website_filename));
    return file_request_handler;
}

std::unique_ptr<URLShortenerHandler> URLShortenerFactory::createURLShortenerHandler(std::shared_ptr<PostgresDBManager> db_manager) {
    auto server_domain_name = getEnv(environment::DOMAIN_NAME);
    spdlog::info("Creating URLShortenerHandler with {} server domain.", server_domain_name);

    return std::make_unique<URLShortenerHandler>(std::move(db_manager), server_domain_name);
}

std::unique_ptr<URLRedirectHandler> URLShortenerFactory::createURLRedirectHandler(std::shared_ptr<PostgresDBManager> db_manager) {
    return std::make_unique<URLRedirectHandler>(std::move(db_manager));
}

std::unique_ptr<APIVersionHandler> URLShortenerFactory::createAPIVersionHandler() {
    return std::make_unique<APIVersionHandler>();
}

std::unique_ptr<NotFoundHandler> URLShortenerFactory::createNotFoundHandler() {
    auto web_files_directory = getEnv(environment::WEB_FILES_DIRECTORY);
    auto not_found_file = web_files_directory + "/404.html";
    spdlog::info("Creating NotFoundHandler with {} not found filename.", not_found_file);
    return std::make_unique<NotFoundHandler>(std::move(not_found_file));
}
