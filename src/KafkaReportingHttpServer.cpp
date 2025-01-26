#include "KafkaReportingHttpServer.hpp"

#include <nlohmann/json.hpp>

#include <chrono>
#include <sstream>


using kafka::clients::producer::ProducerRecord;
using kafka::clients::producer::RecordMetadata;


KafkaReportingHttpServer::KafkaReportingHttpServer(const utility::string_t &url, const kafka::Properties &properties,
                                                   std::string topic): HTTPServer(url), producer(properties),
                                                                       topic(std::move(topic)) {
}

void KafkaReportingHttpServer::handleRequest(const web::http::http_request &request) {
    nlohmann::json json{};
    json["ip"] = request.remote_address();
    json["path"] = request.relative_uri().path();
    json["method"] = request.method();
    json["timestamp"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto user_agent = request.headers().find("User-Agent");
    if (user_agent != request.headers().end()) {
        json["User-Agent"] = user_agent->second;
    }
    auto start = std::chrono::high_resolution_clock::now();
    HTTPServer::handleRequest(request);
    auto end = std::chrono::high_resolution_clock::now();
    std::stringstream ss;
    ss << (end-start);
    json["handle_duration"] = ss.str();

    auto serialized = json.dump();
    ProducerRecord record(topic, kafka::NullKey, kafka::Value(serialized.data(), serialized.size()));


    try {
        producer.syncSend(record);
    } catch (const kafka::KafkaException& e) {
        spdlog::error("Failed to publish data to kafka '{}' topic, details: {}", topic, e.what());
    }
}
