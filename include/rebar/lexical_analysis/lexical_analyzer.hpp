//
// Created by maxng on 7/14/2024.
//

#ifndef LEXICAL_ANALYZER_HPP
#define LEXICAL_ANALYZER_HPP

#include <ranges>

#include <rebar/lexical_analysis/symbol.hpp>
#include <rebar/lexical_analysis/lexical_unit.hpp>
#include <rebar/lexical_analysis/escape_sequence.hpp>

namespace rebar {

    /**
     * A class to analyze and convert plaintext to lexical tokens.
     */
    class lexical_analyzer {
        string_engine*      m_string_engine;
        symbol_map          m_symbol_map;
        escape_sequence_map m_escape_sequence_map;
        std::size_t         m_max_symbol_length;

    public:
        explicit inline lexical_analyzer(
            string_engine & a_string_engine,
            symbol_map a_symbol_map = default_symbol_map(),
            escape_sequence_map a_escape_sequence_map = default_escape_sequence_map()
        ) noexcept;

        lexical_analyzer(lexical_analyzer const &) noexcept = default;
        lexical_analyzer(lexical_analyzer &&)      noexcept = default;

        lexical_analyzer & operator = (lexical_analyzer const &) noexcept = default;
        lexical_analyzer & operator = (lexical_analyzer &&) noexcept = default;

        /**
         * Perform lexical analysis on the plaintext in the lexical unit.
         * @param a_lexical_unit The lexical unit containing the text to
         *                       examine and in which the results of the
         *                       analysis will be placed.
         */
        void perform_analysis(lexical_unit & a_lexical_unit) const;

        /**
         * Replaces all occurrences of escape sequences (\?) with their
         * matching characters.
         * @param a_raw_string The raw string with explicit escape sequences.
         * @return The processed string, with escape string replaced.
         */
        [[nodiscard]]
        std::string process_string(std::string_view a_raw_string) const;

        /**
         * Parse a string into a number.
         * @param a_raw_string The string to parse for a number.
         * @param a_has_separators Whether the number string contains
         *                         separating characters (') or not. (If set to
         *                         true, separating characters will be removed
         *                         before final parsing.)
         * @return The parsed number.
         */
        [[nodiscard]]
        static number parse_number(std::string_view a_raw_string, bool a_has_separators = true);

        /**
         * Parse a string into an integer.
         * @param a_raw_string The string to parse for an integer.
         * @param a_has_separators Whether the integer string contains
         *                         separating characters (') or not. (If set to
         *                         true, separating characters will be removed
         *                         before final parsing.)
         * @return The parsed integer.
         */
        [[nodiscard]]
        static integer parse_integer(std::string_view a_raw_string, bool a_has_separators = true);
    };

    // ###################################### INLINE DEFINITIONS ######################################

    lexical_analyzer::lexical_analyzer(
        string_engine & a_string_engine,
        symbol_map a_symbol_map,
        escape_sequence_map a_escape_sequence_map
    ) noexcept :
        m_string_engine(&a_string_engine),
        m_symbol_map(std::move(a_symbol_map)),
        m_escape_sequence_map(std::move(a_escape_sequence_map))
    {
        // Find max symbol length for use in parsing with selecting symbols.
        m_max_symbol_length = std::ranges::max(
            std::ranges::views::keys(m_symbol_map) |
            std::ranges::views::transform(
                [](auto const & a) {
                    return a.size();
                }
            )
        );
    }

}

#endif //LEXICAL_ANALYZER_HPP
