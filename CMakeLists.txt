cmake_minimum_required(VERSION 3.28)
project(rebar2playground)

set(CMAKE_CXX_STANDARD 20)

FILE(
    GLOB_RECURSE
    REBAR_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/source/*.cpp
)

add_executable(rebar2playground testing/main.cpp ${REBAR_SOURCE_FILES})

target_include_directories(
    rebar2playground PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281 # 10.2.1
)
FetchContent_MakeAvailable(fmt)
target_link_libraries(rebar2playground fmt::fmt)

# DEBUG MESSAGES
target_compile_definitions(
    rebar2playground PUBLIC
    REBAR_DEBUG_MESSAGES_VERBOSE
)
