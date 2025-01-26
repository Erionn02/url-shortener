from conan import ConanFile
from conan.tools.cmake import cmake_layout


class UrlShortenerRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    default_options = {
        "librdkafka/*:ssl" : True,
        "librdkafka/*:sasl" : True
    }
    def requirements(self):
        self.requires("gtest/1.14.0")
        self.requires("cpprestsdk/2.10.19")
        self.requires("libpqxx/7.10.0")
        self.requires("fmt/10.2.1")
        self.requires("spdlog/1.14.0")
        self.requires("boost/1.83.0")
        self.requires("openssl/3.2.1")
        self.requires("nlohmann_json/3.11.3")
        self.requires("modern-cpp-kafka/2024.07.03")

    def layout(self):
        cmake_layout(self)