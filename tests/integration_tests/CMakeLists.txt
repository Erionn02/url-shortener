package_add_test(integration_tests
        SOURCES
        HTTPServerTests.cpp
        URLShortenerFactoryTests.cpp
        DEPENDS
        url-shortener-lib
        )

target_compile_definitions(integration_tests PRIVATE WEB_FILES_DIR="${CMAKE_SOURCE_DIR}/web")
