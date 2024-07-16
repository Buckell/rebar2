//
// Created by maxng on 7/16/2024.
//

#ifndef ESCAPE_SEQUENCE_HPP
#define ESCAPE_SEQUENCE_HPP

#include <utility>
#include <cstdint>

#include <rebar/util/static_string.hpp>

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
     */
    template <static_string v_replacement>
    struct simple_replacement {
        static std::pair<std::string_view, std::size_t> value(std::string_view) {
            return { v_replacement.view(), 1 };
        }
    };

    template <static_string v_replacement>
    constexpr auto simple_replacement_v = simple_replacement<v_replacement>::value;

    inline escape_sequence_map default_escape_sequence_map() {
        return {
            // Ignore errors on following lines if present.
            // (CLion doesn't appreciate template weirdness.)
            { '"',  simple_replacement_v<"\""> },
            { '\\', simple_replacement_v<"\\"> },
            { 'n',  simple_replacement_v<"\n"> },
            { 't',  simple_replacement_v<"\t"> },
        };
    }

}

#endif //ESCAPE_SEQUENCE_HPP
