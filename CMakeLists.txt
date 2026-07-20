cmake_minimum_required(VERSION 3.22)
project(app CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(fmt REQUIRED)
find_package(Framework REQUIRED)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE Framework::framework_core)
target_compile_definitions(app PRIVATE
    BOOST_ASIO_SEPARATE_COMPILATION
    BOOST_BEAST_SEPARATE_COMPILATION
    BOOST_MYSQL_SEPARATE_COMPILATION
)

option(ENABLE_TESTS "Enable tests" OFF)
if(ENABLE_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(test_endpoint tests/test_endpoint.cc)
    target_link_libraries(test_endpoint PRIVATE
        Framework::framework_core
        GTest::gtest
        GTest::gtest_main
    )
    target_compile_definitions(test_endpoint PRIVATE
        BOOST_ASIO_SEPARATE_COMPILATION
        BOOST_BEAST_SEPARATE_COMPILATION
        BOOST_MYSQL_SEPARATE_COMPILATION
    )
    include(GoogleTest)
    gtest_discover_tests(test_endpoint)
endif()
