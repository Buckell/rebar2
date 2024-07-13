cmake_minimum_required(VERSION 3.28)
project(rebar2playground)

set(CMAKE_CXX_STANDARD 20)

add_executable(rebar2playground testing/main.cpp)

target_include_directories(
    rebar2playground PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)