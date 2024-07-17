#include <iostream>
#include <rebar/rebar.hpp>

int main() {
    rebar::environment env;

    auto str = env.str("Hello");

    auto t = rebar::token(str);

    std::cout << t.get_string() << std::endl;

    rebar::string_engine se;

    rebar::lexical_analyzer la(se);

    rebar::lexical_unit lu("Hello 34234324 truen \"32423 324324  dfasdf adsf adsf ewec \\\" 4\"");
    la.perform_analysis(lu);

    std::ranges::for_each(lu.tokens(), [](auto const & a_token) {
        std::cout << a_token.to_string() << '\n';
    });

    return 0;
}
