#include <cpprest/http_listener.h>
#include <cpprest/http_client.h>

#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>


int main() {
    web::http::experimental::listener::http_listener listener{"http://0.0.0.0:2137"};
    listener.support([](const web::http::http_request& request){
        web::http::http_response response;
        response.set_body("Hello world!.");
        response.set_status_code(web::http::status_codes::OK);
        request.reply(response);
    });
    listener.open().get();

    std::this_thread::sleep_for(std::chrono::seconds(std::numeric_limits<std::int64_t>::max()));

    return 0;
}
