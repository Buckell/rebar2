#include <rebar/rebar.hpp>

int main() {
    rebar::string_engine engine;

    auto str1 = engine.str("Hello, world!");
    auto str2 = engine.str("Hello, world!");



    return 0;
}
