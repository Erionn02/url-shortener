#pragma once

#include "EnvironmentVariables.hpp"
#include "HTTPServer.hpp"


class URLShortenerFactory {
public:
    static std::unique_ptr<HTTPServer> create();

};