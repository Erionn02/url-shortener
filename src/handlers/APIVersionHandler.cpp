#include "handlers/APIVersionHandler.hpp"


APIVersionHandler::APIVersionHandler(): IRequestHandler(HANDLER_URI, web::http::methods::GET) {

}

void APIVersionHandler::doHandle(RequestData &request_data) {
    request_data.setResponse(web::http::status_codes::OK, CURRENT_API_VERSION);
}

