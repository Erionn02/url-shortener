#include "handlers/FileRequestHandler.hpp"
#include "Utils.hpp"


FileRequestHandler::FileRequestHandler(std::filesystem::path base_dir, std::string main_website_filename)
        : IRequestHandler(getHandledPaths(base_dir),
                          web::http::methods::GET),
          BASE_DIR(std::move(base_dir)),
          MAIN_WEBSITE_FILE_PATH(BASE_DIR / std::move(main_website_filename)) {
    if (!std::filesystem::exists(MAIN_WEBSITE_FILE_PATH)) {
        throw RequestHandlerException(fmt::format("FileRequestHandler's MAIN_WEBSITE_FILE_PATH {} does not exist.",
                                                  MAIN_WEBSITE_FILE_PATH.string()));
    }
}

std::set<std::string> FileRequestHandler::getFilesInDirectory(const std::filesystem::path &directory) {
    validateDirectory(directory);
    std::set<std::string> paths{};
    for (const auto &dir_entry: std::filesystem::directory_iterator(directory)) {
        if (!dir_entry.is_directory()) { // for now only files in the directory will be allowed, no recursion
            paths.emplace(dir_entry.path().filename());
        }
    }
    return paths;
}

void FileRequestHandler::validateDirectory(const std::filesystem::path &directory) {
    if (!std::filesystem::exists(directory)) {
        throw RequestHandlerException(
                fmt::format("FileRequestHandler's base dir {} does not exist.", directory.string()));
    }

    if (!std::filesystem::is_directory(directory)) {
        throw RequestHandlerException(
                fmt::format("FileRequestHandler's base dir {} is not a directory.", directory.string()));
    }

    if (std::filesystem::is_empty(directory)) {
        throw RequestHandlerException(fmt::format("FileRequestHandler's base dir {} is empty.", directory.string()));
    }
}

void FileRequestHandler::doHandle(RequestData &request_data) {
    std::filesystem::path full_path = getFileFullPath(request_data);
    web::http::http_response response;
    response.headers().set_content_type(recognizeContentType(full_path));
    auto body = utils::readFileContent(full_path);
    response.headers().set_content_length(body.size());
    response.set_body(std::move(body));
    response.set_status_code(web::http::status_codes::OK);
    request_data.setResponse(response);
}

std::filesystem::path FileRequestHandler::getFileFullPath(const RequestData &request_data) const {
    auto filename_to_read = request_data.path();
    if (filename_to_read == "/") {
        return BASE_DIR / MAIN_WEBSITE_FILE_PATH;
    }
    return BASE_DIR.string() + filename_to_read;
}

std::string FileRequestHandler::recognizeContentType(const std::filesystem::path &file_path) {
    static const std::unordered_map<std::string, std::string> EXTENSION_TO_CONTENT_TYPE{{".html", "text/html"},
                                                                                        {".json", "application/json"},
                                                                                        {".js",   "text/javascript"},
                                                                                        {".xml",  "text/xml"},
                                                                                        {".css",  "text/css"},
                                                                                        {".csv",  "text/csv"},
                                                                                        {".gif",  "image/gif"},
                                                                                        {".jpeg", "image/jpeg"},
                                                                                        {".png",  "image/png"},
                                                                                        {".tiff", "image/tiff"},
    };
    auto extension = file_path.extension().string();
    if (EXTENSION_TO_CONTENT_TYPE.contains(extension)) {
        return EXTENSION_TO_CONTENT_TYPE.at(extension);
    }
    return "text/plain";
}

std::set<std::string> FileRequestHandler::getHandledPaths(const std::filesystem::path &directory) {
    auto files_in_dir = getFilesInDirectory(directory);
    std::set<std::string> handled_paths{};
    for (const auto &filename: files_in_dir) {
        handled_paths.emplace("/" + filename);
    }
    handled_paths.emplace("/");
    return handled_paths;
}

