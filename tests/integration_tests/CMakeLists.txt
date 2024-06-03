package_add_test(integration_tests
        SOURCES
        URLShortenerServerTests.cpp
        URLShortenerFactoryTests.cpp
        DEPENDS
        url-shortener-lib
        )

target_include_directories(integration_tests PRIVATE ../utils)
target_compile_definitions(integration_tests PRIVATE WEB_FILES_DIR="${CMAKE_SOURCE_DIR}/web")
