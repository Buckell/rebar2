//
// Created by maxng on 7/16/2024.
//

#ifndef ESCAPE_SEQUENCE_HPP
#define ESCAPE_SEQUENCE_HPP

#include <utility>
#include <cstdint>

namespace rebar {

    /**
     * A handler for escape sequences.
     * @param a_trailing The string starting at the escape sequence (beginning
     *                   after the leading backslash.
     * @return A pair representing the replacement value and amount of
     *         characters used in the escape sequence (to determine the amount
     *         to cut from the original string).
     * @note Even though escape sequences are only supposed to result in a
     *       single character, the replacement value for an escape sequence
     *       is expressed as a string. This is to enable support for Unicode
     *       and multibyte characters.
     */
    using escape_sequence_handler = std::pair<std::string_view, std::size_t> (*)(std::string_view a_trailing);

    /**
     * A map representing the first character in an escape sequence and its
     * associated handler.
     */
    using escape_sequence_map = std::unordered_map<unsigned char, escape_sequence_handler>;

    /**
     * Generates an escape sequence handler that returns a static string. Used
     * for simple escape sequences (like converting (\") to just (") ).
     * @tparam v_replacement_length Length of replacement string.
     * @param replacement_string String to replace escape sequence.
     * @return Generated escape sequence handler.
     */
    template <std::size_t v_replacement_length>
    consteval escape_sequence_handler simple_replacement(const char (&replacement_string)[v_replacement_length]) {
        return [replacement_string](std::string_view) noexcept -> std::pair<std::string_view, std::size_t> {
            return { std::string_view(replacement_string, v_replacement_length), 1 };
        };
    }

    inline escape_sequence_map default_escape_sequence_map() {
        return {
            { '"',  simple_replacement("\"") },
            { '\\', simple_replacement("\\") },
            { 'n',  simple_replacement("\n") },
            { 't',  simple_replacement("\t") },
        };
    }

}

#endif //ESCAPE_SEQUENCE_HPP
