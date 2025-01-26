include(CMakePrintHelpers)

add_lib(url-shortener-lib
        SOURCES
        HTTPServer.cpp
        KafkaReportingHttpServer.cpp
        RequestData.cpp
        URLShortenerFactory.cpp
        PostgresDBManager.cpp
        handlers/IRequestHandler.cpp
        handlers/FileRequestHandler.cpp
        handlers/URLShortenerHandler.cpp
        handlers/URLRedirectHandler.cpp
        handlers/APIVersionHandler.cpp
        handlers/NotFoundHandler.cpp
        Utils.cpp
        )
