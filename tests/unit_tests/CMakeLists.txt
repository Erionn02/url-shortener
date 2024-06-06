package_add_test(unit_tests
        SOURCES
        FileRequestHandlerTests.cpp
        APIVersionHandlerTests.cpp
        URLRedirectHandlerTests.cpp
        URLShortenerHandlerTests.cpp
        NotFoundHandlerTests.cpp
        DEPENDS
        url-shortener-lib
        )

target_include_directories(unit_tests PRIVATE ../utils)
target_compile_definitions(unit_tests PRIVATE WEB_FILES_DIR="${CMAKE_SOURCE_DIR}/web")
