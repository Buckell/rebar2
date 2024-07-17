cmake_minimum_required(VERSION 3.28)
project(rebar2)

include(FetchContent)
include(GoogleTest)

set(CMAKE_CXX_STANDARD 20)

###### FMT LIB DEPENDENCY ######
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281 # 10.2.1
)
FetchContent_MakeAvailable(fmt)
link_libraries(fmt::fmt)

###### GOOGLE TEST DEPENDENCY ######
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG        03597a01ee50ed33e9dfd640b249b4be3799d395 # 1.15.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

###### REBAR INCLUDE DIRECTORY ######
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

###### COMPILE REBAR SOURCE FILES ######
FILE(
    GLOB_RECURSE
    REBAR_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/source/*.cpp
)

add_executable(
    hot_test
    testing/hot_testing.cpp
    ${REBAR_SOURCE_FILES}
)

# HOT TEST DEBUG MESSAGES
target_compile_definitions(
    hot_test PUBLIC
    REBAR_DEBUG_MESSAGES_VERBOSE
)

###### COMPILE TEST CASES ######
FILE(
    GLOB_RECURSE
    TEST_CASES
    ${CMAKE_SOURCE_DIR}/testing/cases/*.cpp
)

###### GOOGLE TESTING ######
enable_testing()

add_executable(
    tests
    testing/test_main.cpp
    ${TEST_CASES}
    ${REBAR_SOURCE_FILES}
)

target_link_libraries(
    tests
    GTest::gtest_main
)

gtest_discover_tests(tests)