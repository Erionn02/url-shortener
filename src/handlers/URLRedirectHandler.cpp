#include "handlers/URLRedirectHandler.hpp"

URLRedirectHandler::URLRedirectHandler(std::shared_ptr<DatabaseManager> db_manager) :
        IRequestHandler(web::http::methods::GET), db_manager(std::move(db_manager)) {
}

void URLRedirectHandler::doHandle(RequestData &request_data) {
    web::http::http_response response;
    response.set_status_code(web::http::status_codes::PermanentRedirect);
    response.headers().add("Location", *db_manager->getOriginalUrl(request_data.path().substr(1)));
    request_data.setResponse(response);
}

bool URLRedirectHandler::canHandle(const RequestData &request_data) const {
    return shouldHandleGivenRequestMethod(request_data.getMethod()) && db_manager->getOriginalUrl(request_data.path().substr(1)).has_value();
}
