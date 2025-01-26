cmake_minimum_required(VERSION 3.13)


# cpprestsdk uses one-letter-macros in public headers
# which leak everywhere and cause problems for example in templates with types named with the same letter as macros
# this definition turns off those ugly macros
add_definitions(-D_TURN_OFF_PLATFORM_STRING)
include(${CMAKE_SOURCE_DIR}/cmake/CMakeHelpers.cmake)
setup_conan()
project(url-shortener)


find_package(GTest REQUIRED)
find_package(Boost REQUIRED)
find_package(cpprestsdk REQUIRED)
find_package(libpqxx REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(modern-cpp-kafka REQUIRED)

include_directories(SYSTEM ${modern-cpp-kafka_INCLUDE_DIRS})

set(CONAN_LIBS "gtest::gtest;boost::boost;OpenSSL::SSL;OpenSSL::Crypto;cpprestsdk::cpprestsdk;libpqxx::pqxx;spdlog::spdlog;fmt::fmt;RdKafka::rdkafka++;nlohmann_json::nlohmann_json")
set(CMAKE_CXX_STANDARD 23)


# Turn on with 'cmake -Dcoverage=ON ..'.
option(coverage "Build for coverage report." OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_COMPILER_IS_GNUCXX AND coverage)
    message("Coverage report ENABLED")

    include(cmake/CodeCoverage.cmake)
    append_coverage_compiler_flags()
    setup_target_for_coverage_gcovr_html(NAME unit_tests_coverage
            EXECUTABLE cd tests/unit_tests && ctest --output-on-failure
            EXCLUDE ${CMAKE_SOURCE_DIR}/tests/* ${CMAKE_SOURCE_DIR}/apps/* )

    setup_target_for_coverage_gcovr_html(NAME integration_tests_coverage
            EXECUTABLE cd tests/integration_tests && ctest --output-on-failure
            EXCLUDE ${CMAKE_SOURCE_DIR}/tests/* ${CMAKE_SOURCE_DIR}/apps/* )

else ()
    message("Coverage report disabled")
endif ()

add_subdirectory(src)
add_subdirectory(apps)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    message("NOT RELEASE mode - enable testing")
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
else ()
    message("RELEASE mode - testing disabled")
endif ()
