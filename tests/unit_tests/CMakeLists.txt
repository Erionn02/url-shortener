package_add_test(unit_tests
        SOURCES
        FileRequestHandlerTests.cpp
        APIVersionHandlerTest.cpp
        URLRedirectHandlerTests.cpp
        URLShortenerHandlerTests.cpp
        DEPENDS
        url-shortener-lib
        )

target_include_directories(unit_tests PRIVATE ../mocks)
target_compile_definitions(unit_tests PRIVATE WEB_FILES_DIR="${CMAKE_SOURCE_DIR}/tests/asset_dir")
