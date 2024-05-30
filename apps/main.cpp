#include "URLShortenerFactory.hpp"

#include <spdlog/spdlog.h>

#include <thread>
#include <chrono>

void sleepForever();

int main() {
    spdlog::set_level(spdlog::level::debug);
    std::unique_ptr<HTTPServer> url_shortener_server = URLShortenerFactory::create();
    spdlog::info("HTTP server created");

    url_shortener_server->open();
    spdlog::info("HTTP server is ready to handle incoming requests");

    sleepForever();
    return 0;
}

void sleepForever() { std::this_thread::sleep_for(std::chrono::seconds(std::numeric_limits<int64_t>::max())); }
