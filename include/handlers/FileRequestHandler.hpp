#pragma once

#include "IRequestHandler.hpp"

#include <filesystem>

class FileRequestHandler : public IRequestHandler {
public:
    FileRequestHandler(std::filesystem::path base_dir, std::string main_website_filename);

private:
    void doHandle(RequestData &request_data) override;
    void validateDirectory(const std::filesystem::path &directory);
    std::set<std::string> getFilesInDirectory(const std::filesystem::path &directory);
    std::set<std::string> getHandledPaths(const std::filesystem::path &directory);
    std::string readFileContent(const std::filesystem::path &file_path);
    std::string recognizeContentType(const std::filesystem::path &file_path);
    std::filesystem::path getFileFullPath(const RequestData &request_data) const;

    const std::filesystem::path BASE_DIR;
    const std::filesystem::path MAIN_WEBSITE_FILE_PATH;
};


