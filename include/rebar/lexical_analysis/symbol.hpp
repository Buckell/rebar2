//
// Created by maxng on 7/13/2024.
//

#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <array>
#include <unordered_map>
#include <string_view>

namespace rebar {
    /**
     * An enumeration representing all of the special characters and other
     * symbols that can be present within a piece of plaintext code, including
     * keywords and other reserved sequences.
     */
    enum class symbol {
        tilda,                   // ~
        exclamation,             // !
        exclamation_equals,      // !=
        at,                      // @
        pound,                   // #
        dollar,                  // $
        percent,                 // %
        carrot,                  // ^
        carrot_equals,           // ^=
        ampersand,               // &
        ampersand_equals,        // &=
        double_ampersand,        // &&
        double_ampersand_equals, // &&=
        star,                    // *
        star_equals,             // *=
        parenthesis_left,        // (
        parenthesis_right,       // )
        bracket_left,            // [
        bracket_right,           // ]
        brace_left,              // {
        brace_right,             // }
        minus,                   // -
        minus_equals,            // -=
        plus,                    // +
        plus_equals,             // +=
        equals,                  // =
        double_equals,           // ==
        slash,                   // /
        slash_equals,            // /=
        colon,                   // :
        semicolon,               // ;
        comma,                   // ,
        carrot_left,             // <
        carrot_right,            // >
        pipe,                    // |
        pipe_equals,             // |=
        double_pipe,             // ||
        double_pipe_equals,      // ||=
        boolean_true,            // true
        boolean_false,           // false
    };

    /**
     * A map of plaintext symbols to enumeration values.
     * Enumeration values are paired with a boolean that marks whether the
     * symbol is a non-special character symbol (if true and the symbol is
     * found, but begins an identifier, the symbol will not be parsed, and
     * will be included in the identifier).
     */
    using symbol_map = std::unordered_map<std::string_view, std::pair<symbol, bool>>;

    /**
     * Generate the default Rebar symbol map.
     * @return The default Rebar symbol map.
     */
    inline symbol_map default_symbol_map() {
        return {
            { "~",     { symbol::tilda,                   false, } },
            { "!",     { symbol::exclamation,             false, } },
            { "!=",    { symbol::exclamation_equals,      false, } },
            { "@",     { symbol::at,                      false, } },
            { "#",     { symbol::pound,                   false, } },
            { "$",     { symbol::dollar,                  false, } },
            { "%",     { symbol::percent,                 false, } },
            { "^",     { symbol::carrot,                  false, } },
            { "^=",    { symbol::carrot_equals,           false, } },
            { "&",     { symbol::ampersand,               false, } },
            { "&=",    { symbol::ampersand_equals,        false, } },
            { "&&",    { symbol::double_ampersand,        false, } },
            { "&&=",   { symbol::double_ampersand_equals, false, } },
            { "*",     { symbol::star,                    false, } },
            { "*=",    { symbol::star_equals,             false, } },
            { "(",     { symbol::parenthesis_left,        false, } },
            { ")",     { symbol::parenthesis_right,       false, } },
            { "[",     { symbol::bracket_left,            false, } },
            { "]",     { symbol::bracket_right,           false, } },
            { "{",     { symbol::brace_left,              false, } },
            { "}",     { symbol::brace_right,             false, } },
            { "-",     { symbol::minus,                   false, } },
            { "-=",    { symbol::minus_equals,            false, } },
            { "+",     { symbol::plus,                    false, } },
            { "+=",    { symbol::plus_equals,             false, } },
            { "=",     { symbol::equals,                  false, } },
            { "==",    { symbol::double_equals,           false, } },
            { "/",     { symbol::slash,                   false, } },
            { "/=",    { symbol::slash_equals,            false, } },
            { ":",     { symbol::colon,                   false, } },
            { ";",     { symbol::semicolon,               false, } },
            { ",",     { symbol::comma,                   false, } },
            { "<",     { symbol::carrot_left,             false, } },
            { ">",     { symbol::carrot_right,            false, } },
            { "|",     { symbol::pipe,                    false, } },
            { "|=",    { symbol::pipe_equals,             false, } },
            { "||",    { symbol::double_pipe,             false, } },
            { "||=",   { symbol::double_pipe_equals,      false, } },
            { "true",  { symbol::boolean_true,            true,  } },
            { "false", { symbol::boolean_false,           true,  } },
        };
    }

    constexpr std::string_view symbol_as_string(symbol const a_symbol) noexcept {
        using namespace std::string_view_literals;

        constexpr std::array symbol_strings{
            "tilda",
            "exclamation",
            "exclamation_equals",
            "at",
            "pound",
            "dollar",
            "percent",
            "carrot",
            "carrot_equals",
            "ampersand",
            "ampersand_equals",
            "double_ampersand",
            "double_ampersand_equals",
            "star",
            "star_equals",
            "parenthesis_left",
            "paranthesis_right",
            "bracket_left",
            "bracket_right",
            "brace_left",
            "brace_right",
            "minus",
            "minus_equals",
            "plus",
            "plus_equals",
            "equals",
            "double_equal",
            "slash",
            "slash_equals",
            "colon",
            "semicolon",
            "comma",
            "carrot_left",
            "carrot_right",
            "pipe",
            "pipe_equals",
            "double_pipe",
            "double_pipe_equals",
            "boolean_true",
            "boolean_false",
        };

        return symbol_strings[static_cast<size_t>(a_symbol)];
    }

}

#endif //SYMBOL_HPP
