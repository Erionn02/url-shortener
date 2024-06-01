#pragma once

#include "EnvironmentVariables.hpp"
#include "HTTPServer.hpp"
#include "FileRequestHandler.hpp"
#include "URLShortenerHandler.hpp"
#include "DatabaseManager.hpp"

#include <memory>



class URLShortenerFactory {
public:
    static std::unique_ptr<HTTPServer> create();
    static std::unique_ptr<FileRequestHandler> createFileRequestHandler();
    static std::unique_ptr<URLShortenerHandler> createURLShortenerHandler();
    static std::shared_ptr<DatabaseManager> createDatabaseManager();
};