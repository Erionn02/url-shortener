include(CMakePrintHelpers)

add_lib(url-shortener-lib
        SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/HTTPServer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RequestData.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IRequestHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/URLShortenerFactory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/FileRequestHandler.cpp
        )

add_lib(template_lib_one
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/TestClass.cpp
        )

add_lib(template_lib_two
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/TestClass2.cpp
        )
