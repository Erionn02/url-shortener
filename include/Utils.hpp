#pragma once

#include <string>
#include <filesystem>

namespace utils {
    std::string readFileContent(const std::filesystem::path &file_path);
    std::string generateRandomString(std::size_t length);
}

