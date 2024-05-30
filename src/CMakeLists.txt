include(CMakePrintHelpers)

add_lib(url-shortener-lib
        SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/HTTPServer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RequestData.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IRequestHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/URLShortenerFactory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/FileRequestHandler.cpp
        )
