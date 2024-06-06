#include "Utils.hpp"

#include <fstream>
#include <random>

std::string utils::readFileContent(const std::filesystem::path &file_path) {
    std::ifstream file{file_path};
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string utils::generateRandomString(std::size_t length)  {
    static auto &chrs = "0123456789"
                        "abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    static thread_local std::mt19937 rg{std::random_device{}()};
    static thread_local std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string random_str;
    random_str.reserve(length);
    while (length--) {
        random_str += chrs[pick(rg)];
    }

    return random_str;
}