#include <iostream>
#include <rebar/rebar.hpp>

int main() {
    rebar::environment env;

    auto str = env.str("Hello");

    auto t = rebar::token(3232ull);

    std::cout << t.get_integer() << std::endl;

    return 0;
}
