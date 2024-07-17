//
// Created by maxng on 7/16/2024.
//

#include <cstdint>
#include <string>

#include <fmt/format.h>

#include <rebar/lexical_analysis/token.hpp>

namespace rebar {

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
                return fmt::format("SYMBOL ({})", static_cast<std::size_t>(get_symbol()));
        }

        return "";
    }

}