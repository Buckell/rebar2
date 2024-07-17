//
// Created by maxng on 7/15/2024.
//

#ifndef LEXICAL_UNIT_HPP
#define LEXICAL_UNIT_HPP

#include <span>
#include <string>
#include <vector>

#include <rebar/lexical_analysis/token.hpp>

namespace rebar {
    /**
     * A class to store lexical analysis data, output, and other related
     * information.
     */
    class lexical_unit {
        std::string              m_plaintext;
        std::vector<token>       m_tokens;
        std::vector<std::size_t> m_token_positions;

    public:
        /**
         * Construct a lexical unit for use with a lexical analyzer.
         * @param a_plaintext The plaintext of the unit.
         */
        explicit inline lexical_unit(std::string a_plaintext) noexcept;

        [[nodiscard]]
        inline std::string_view plaintext() const noexcept;

        [[nodiscard]]
        inline std::span<token const> tokens() const noexcept;

        /**
         * Add a token to the result of lexical analysis.
         * @param a_token The token to add.
         * @param a_token_position The position of the token to add.
         */
        inline void push_token(token a_token, std::size_t a_token_position) noexcept;
    };

    // ###################################### INLINE DEFINITIONS ######################################

    lexical_unit::lexical_unit(std::string a_plaintext) noexcept :
        m_plaintext(std::move(a_plaintext))
    {}

    std::string_view lexical_unit::plaintext() const noexcept {
        return m_plaintext;
    }

    inline std::span<token const> lexical_unit::tokens() const noexcept {
        return m_tokens;
    }

    void lexical_unit::push_token(token a_token, std::size_t const a_token_position) noexcept {
        m_tokens.emplace_back(std::move(a_token));
        m_token_positions.push_back(a_token_position);
    }

}

#endif //LEXICAL_UNIT_HPP
