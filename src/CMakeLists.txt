include(CMakePrintHelpers)

add_lib(url-shortener-lib
        SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/HTTPServer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RequestData.cpp
        handlers/IRequestHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/URLShortenerFactory.cpp
        handlers/FileRequestHandler.cpp
        handlers/URLShortenerHandler.cpp
        handlers/URLRedirectHandler.cpp
        handlers/APIVersionHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PostgresDBManager.cpp
        )
