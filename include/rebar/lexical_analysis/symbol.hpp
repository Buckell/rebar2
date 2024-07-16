//
// Created by maxng on 7/13/2024.
//

#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <unordered_map>

namespace rebar {
    /**
     * An enumeration representing all of the special characters and other
     * symbols that can be present within a piece of plaintext code, including
     * keywords and other reserved sequences.
     */
    enum class symbol {
        tilda, // ~
        exclamation, // !
        exclamation_equals, // !=
        at, // @
        pound, // #
        dollar, // $
        percent, // %
        carrot, // ^
        carrot_equals, // ^=
        ampersand, // &
        ampersand_equals, // &=
        double_ampersand, // &&
        double_ampersand_equals, // &&=
        star, // *
        star_equals, // *=
        parenthesis_left, // (
        paranthesis_right, // )
        bracket_left, // [
        bracket_right, // ]
        brace_left, // {
        brace_right, // }
        minus, // -
        minus_equals, // -=
        plus, // +
        plus_equals, // +=
        equals, // =
        double_equals, // ==
        slash, // /
        slash_equals, // /=
        colon, // :
        semicolon, // ;
        comma, // ,
        carrot_left, // <
        carrot_right, // >
        pipe, // |
        pipe_equals, // |=
        double_pipe, // ||
        double_pipe_equals, // ||=
    };

    /// A map of plaintext symbols to enumeration values.
    using symbol_map = std::unordered_map<std::string_view, symbol>;

    /**
     * Generate the default Rebar symbol map.
     * @return The default Rebar symbol map.
     */
    inline symbol_map default_symbol_map() {
        return {
            { "~",   symbol::tilda,                   },
            { "!",   symbol::exclamation,             },
            { "!=",  symbol::exclamation_equals,      },
            { "@",   symbol::at,                      },
            { "#",   symbol::pound,                   },
            { "$",   symbol::dollar,                  },
            { "%",   symbol::percent,                 },
            { "^",   symbol::carrot,                  },
            { "^=",  symbol::carrot_equals,           },
            { "&",   symbol::ampersand,               },
            { "&=",  symbol::ampersand_equals,        },
            { "&&",  symbol::double_ampersand,        },
            { "&&=", symbol::double_ampersand_equals, },
            { "*",   symbol::star,                    },
            { "*=",  symbol::star_equals,             },
            { "(",   symbol::parenthesis_left,        },
            { ")",   symbol::paranthesis_right,       },
            { "[",   symbol::bracket_left,            },
            { "]",   symbol::bracket_right,           },
            { "{",   symbol::brace_left,              },
            { "}",   symbol::brace_right,             },
            { "-",   symbol::minus,                   },
            { "-=",  symbol::minus_equals,            },
            { "+",   symbol::plus,                    },
            { "+=",  symbol::plus_equals,             },
            { "=",   symbol::equals,                  },
            { "==",  symbol::double_equals,           },
            { "/",   symbol::slash,                   },
            { "/=",  symbol::slash_equals,            },
            { ":",   symbol::colon,                   },
            { ";",   symbol::semicolon,               },
            { ",",   symbol::comma,                   },
            { "<",   symbol::carrot_left,             },
            { ">",   symbol::carrot_right,            },
            { "|",   symbol::pipe,                    },
            { "|=",  symbol::pipe_equals,             },
            { "||",  symbol::double_pipe,             },
            { "||=", symbol::double_pipe_equals,      },
        };
    }

}

#endif //SYMBOL_HPP
