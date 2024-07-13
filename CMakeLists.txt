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