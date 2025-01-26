#pragma once

#include "HTTPServer.hpp"

#include <kafka/KafkaProducer.h>



class KafkaReportingHttpServer : public HTTPServer {
public:
    KafkaReportingHttpServer(const utility::string_t &url, const kafka::Properties& properties, std::string topic);

    void handleRequest(const web::http::http_request &request) override;
private:
    kafka::clients::producer::KafkaProducer producer;
    std::string topic;
};