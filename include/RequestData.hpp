#pragma once

#include <cpprest/http_msg.h>

#include <optional>

class RequestData {
public:
    explicit RequestData(web::http::http_request request);
    void reply();
    void setResponse(web::http::http_response new_response);

    template<typename Body>
    void setResponse(web::http::status_code status_code, Body &&body) {
        web::http::http_response response_local;

        response_local.set_status_code(status_code);
        response_local.set_body(std::forward<Body>(body));
        setResponse(std::move(response_local));
    }

    [[nodiscard]] web::http::method getMethod() const;
    [[nodiscard]] web::http::http_response &getResponse();
    [[nodiscard]] std::string tryGetHeaderValue(const std::string &name) const;
    [[nodiscard]] std::optional<std::string> getHeaderValue(const std::string &name) const;
    [[nodiscard]] std::string path() const;

private:
    web::http::http_request request;
    web::http::http_response response{web::http::status_codes::BadRequest};
};

