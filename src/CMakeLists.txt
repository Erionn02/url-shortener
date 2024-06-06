include(CMakePrintHelpers)

add_lib(url-shortener-lib
        SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/HTTPServer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RequestData.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/URLShortenerFactory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PostgresDBManager.cpp
        handlers/IRequestHandler.cpp
        handlers/FileRequestHandler.cpp
        handlers/URLShortenerHandler.cpp
        handlers/URLRedirectHandler.cpp
        handlers/APIVersionHandler.cpp
        handlers/NotFoundHandler.cpp
        Utils.cpp
        )
