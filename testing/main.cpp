#include <iostream>
#include <rebar/rebar.hpp>

int main() {
    rebar::string_engine engine;

    auto str1 = engine.str("Hello, world!");
    auto str2 = engine.str("Hello, world!");

    std::cout << (str1 == str2) << std::endl;

    return 0;
}
