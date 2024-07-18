//
// Created by maxng on 7/16/2024.
//

#include <cstdint>
#include <string>

#include <fmt/format.h>

#include <rebar/lexical_analysis/token.hpp>

namespace rebar {

    bool token::operator == (token const & a_token) const noexcept {
        if (m_type != a_token.m_type) {
            return false;
        }

        switch (m_type) {
            case token_type::integer:
                return get_integer() == a_token.get_integer();
            case token_type::number:
                return get_number() == a_token.get_number();
            case token_type::string:
            case token_type::identifier:
                return get_string() == a_token.get_string();
            case token_type::symbol:
                return get_symbol() == a_token.get_symbol();
            default:
                return false;
        }
    }

    std::string token::to_string() const noexcept {
        switch (m_type) {
            case token_type::null:
                return "NULL";
            case token_type::identifier:
                return fmt::format("IDENTIFIER ({})", get_string().view());
            case token_type::integer:
                return fmt::format("INTEGER ({})", get_integer());
            case token_type::number:
                return fmt::format("NUMBER ({})", get_number());
            case token_type::string:
                return fmt::format("STRING (\"{}\")", get_string().view());
            case token_type::symbol:
                return fmt::format("SYMBOL ({})", symbol_as_string(get_symbol()));
        }

        return "";
    }

}