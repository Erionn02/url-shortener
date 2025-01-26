#pragma once

#include "EnvironmentVariables.hpp"
#include "HTTPServer.hpp"
#include "KafkaReportingHttpServer.hpp"
#include "handlers/FileRequestHandler.hpp"
#include "handlers/URLShortenerHandler.hpp"
#include "handlers/APIVersionHandler.hpp"
#include "handlers/URLRedirectHandler.hpp"
#include "handlers/NotFoundHandler.hpp"
#include "PostgresDBManager.hpp"

#include <memory>


class URLShortenerFactory {
public:
    static std::unique_ptr<HTTPServer> create();
    static std::unique_ptr<HTTPServer> create(std::shared_ptr<PostgresDBManager> db_manager);
    static std::unique_ptr<FileRequestHandler> createFileRequestHandler();
    static std::unique_ptr<URLShortenerHandler> createURLShortenerHandler(std::shared_ptr<PostgresDBManager> db_manager);
    static std::unique_ptr<URLRedirectHandler> createURLRedirectHandler(std::shared_ptr<PostgresDBManager> db_manager);
    static std::unique_ptr<APIVersionHandler> createAPIVersionHandler();
    static std::unique_ptr<NotFoundHandler> createNotFoundHandler();
    static std::shared_ptr<PostgresDBManager> createPostgresDatabaseManager();
    static void addAllForbiddenPaths(PostgresDBManager& db_manager);
};