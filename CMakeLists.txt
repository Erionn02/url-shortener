cmake_minimum_required(VERSION 3.13)
project(url-shortener)
set(CMAKE_CXX_STANDARD 23)

set(CONAN_SYSTEM_INCLUDES ON)
include(${CMAKE_SOURCE_DIR}/cmake/CMakeHelpers.cmake)

setup_conan()

# cpprestsdk is stupid library and uses one-letter-macros in public headers
# which leak everywhere and cause problems for example in templates with the types named with the same letter
# this definition turns off those ugly macros
add_definitions(-D_TURN_OFF_PLATFORM_STRING)

# Turn on with 'cmake -Dcoverage=ON ..'.
option(coverage "Build for coverage report." OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_COMPILER_IS_GNUCXX AND coverage)
    message("Coverage report ENABLED")

    include(cmake/CodeCoverage.cmake)
    append_coverage_compiler_flags()
    setup_target_for_coverage_gcovr_html(NAME unit_tests_coverage
            EXECUTABLE cd tests/unit_tests && ctest --output-on-failure
            EXCLUDE ${CMAKE_SOURCE_DIR}/tests/* ${CMAKE_SOURCE_DIR}/apps/* ${CMAKE_SOURCE_DIR}/src/main.cpp)

    setup_target_for_coverage_gcovr_html(NAME integration_tests_coverage
            EXECUTABLE cd tests/integration_tests && ctest --output-on-failure
            EXCLUDE ${CMAKE_SOURCE_DIR}/tests/* ${CMAKE_SOURCE_DIR}/apps/* ${CMAKE_SOURCE_DIR}/src/main.cpp)

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
