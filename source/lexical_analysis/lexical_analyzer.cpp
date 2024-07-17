//
// Created by maxng on 7/16/2024.
//

#include <rebar/lexical_analysis/lexical_analyzer.hpp>

namespace rebar {

    void lexical_analyzer::perform_analysis(lexical_unit & a_lexical_unit) const {
        std::string_view const plaintext = a_lexical_unit.plaintext();
        auto plaintext_it = plaintext.cbegin();

        // Test if a value is within a range of values (inclusive).
        auto const in_range = [](auto const value, auto const min, auto const max) {
            return value >= min && value <= max;
        };

        // Keeps incrementing the plaintext iterator until the predicate
        // function returns true. Returns final iterator.
        auto const plaintext_increment_find = [&plaintext_it, &plaintext](auto const predicate) noexcept {
            while (++plaintext_it != plaintext.cend()) {
                if (predicate(*plaintext_it)) {
                    return plaintext_it;
                }
            }

            return plaintext.cend();
        };

        // Find the index into the plaintext to which the specified iterator
        // corresponds.
        auto const get_iterator_plaintext_index = [&plaintext](auto const iterator) noexcept -> std::size_t {
            return std::distance(plaintext.cbegin(), iterator);
        };

        // Main analysis loop.
        while (plaintext_it != plaintext.cend()) {
            unsigned char const current_char = *plaintext_it;

            // Skip spaces, tabs, and other whitespace/non-display characters.
            if (current_char == ' ' || current_char == '\t' || current_char == '\n' || current_char == '\r') {
                ++plaintext_it;
                continue;
            }

            // Test for conditions of a string.
            if (current_char == '"') {
                auto const string_begin = plaintext_it;

                // Whether the parsed string contains an escape sequence or
                // not.
                bool contains_escape_sequence = false;

                // Find end of string.
                auto const string_end = plaintext_increment_find(
                    [&plaintext_it, &contains_escape_sequence](auto const position_char) noexcept {
                        // End string parsing with ending quotation marks.
                        if (position_char == '\"') [[unlikely]] {
                            return true;
                        }

                        // Skip past escape sequence.
                        if (position_char == '\\') [[unlikely]] {
                            // TODO: Optimize escape sequence handling by storing relative escape sequence locations and forwarding to process_string.
                            // (The array of locations can also replace the explicit boolean flag. - !locations.empty())
                            // (Make explicit positions optional, retaining the ability to scan for them.)

                            contains_escape_sequence = true;
                            ++plaintext_it;
                        }

                        return false;
                    }
                );

                // Advance iterator past sentinel/ending quotation marks.
                ++plaintext_it;

                // Add 1 to string_begin to avoid capturing quotation marks.
                auto const raw_string = std::string_view(string_begin + 1, string_end);
                // Replace escape sequences if present.
                auto const final_string = m_string_engine->str(
                    contains_escape_sequence ?
                    process_string(raw_string) :
                    raw_string
                );
                auto const plaintext_position = get_iterator_plaintext_index(string_begin);

                // Add token to analysis result.
                a_lexical_unit.push_token(
                    token(final_string, token_type::string),
                    plaintext_position
                );

                continue;
            }
            // End string parsing.

            // Test for conditions of integer/number.
            if (std::isdigit(current_char)) {
                auto const number_begin = plaintext_it;

                // Store if parsed number has a decimal (number, floating
                // point).
                bool floating_point = false;

                // Store if separating apostrophes are used.
                bool separating_characters = false;

                // Find end of number string.
                auto const number_end = plaintext_increment_find(
                    [&floating_point, &separating_characters](unsigned char const position_char) {
                        if (position_char == '.') [[unlikely]] {
                            floating_point = true;
                            return false;
                        }

                        if (position_char == '\'') [[unlikely]] {
                            separating_characters = true;
                            return false;
                        }

                        return !std::isdigit(position_char) && position_char != '\'';
                    }
                );

                std::string_view const raw_number_string(number_begin, number_end);
                auto const plaintext_position = get_iterator_plaintext_index(number_begin);

                if (floating_point) {
                    a_lexical_unit.push_token(
                        token(parse_number(raw_number_string)),
                        plaintext_position
                    );
                } else {
                    a_lexical_unit.push_token(
                        token(parse_integer(raw_number_string)),
                        plaintext_position
                    );
                }
            }
            // End integer/number parsing.

            // Test for different symbols.
            std::pair<symbol, bool> const * symbol_pair = nullptr;
            auto symbol_end = plaintext_it + 1;

            // TODO: Optimize symbol searching with search trees.

            // Search for longest symbol match.
            for (std::size_t length = 0; length < m_max_symbol_length; ++length) {
                auto const current_end_it = plaintext_it + length + 1;

                if (auto const symbol_it = m_symbol_map.find(std::string_view(plaintext_it, current_end_it)); symbol_it != m_symbol_map.cend()) {
                    symbol_pair = &(symbol_it->second);
                    symbol_end = current_end_it;
                }
            }

            // Test that symbol has been found.
            if (symbol_pair != nullptr) {
                // Test that symbol is not interrupting an identifier.
                if (auto const following_char = *symbol_end; !symbol_pair->second || !(std::isalnum(following_char) || following_char == '_')) {
                    auto const plaintext_position = get_iterator_plaintext_index(plaintext_it);

                    a_lexical_unit.push_token(
                        token(symbol_pair->first),
                        plaintext_position
                    );

                    // Set next scanning position.
                    plaintext_it = symbol_end;

                    continue;
                }
            }
            // End symbol testing.

            // Test for conditions of an identifier token.
            if (std::isalpha(current_char) || current_char == '_') {
                auto const identifier_begin = plaintext_it;

                // Find end of identifier.
                auto const identifier_end = plaintext_increment_find(
                    [](unsigned char const position_char) noexcept {
                        return !std::isalnum(position_char) && position_char != '_';
                    }
                );

                // Compile identifier and its plaintext position.
                auto const identifier = m_string_engine->str(std::string_view(identifier_begin, identifier_end));
                auto const plaintext_position = get_iterator_plaintext_index(identifier_begin);

                // Add token to analysis result.
                a_lexical_unit.push_token(
                    token(identifier, token_type::identifier),
                    plaintext_position
                );

                continue;
            }
            // End identifier parsing.
        }
    }

    std::string lexical_analyzer::process_string(std::string_view const a_raw_string) const {
        std::string final_string;

        // Reserve as much space as original string to avoid reallocation.
        // New string is unlikely to exceed this amount.
        final_string.reserve(a_raw_string.size());

        auto string_it = a_raw_string.begin();

        // Store beginning of string part to add to final string between
        // escape sequences.
        auto string_part_begin = string_it;

        while (string_it != a_raw_string.cend() && string_it + 1 != a_raw_string.cend()) {
            auto const current_char = *string_it;

            // Test for escape sequence.
            if (current_char == '\\') {
                // Add preceding string part to the final string.
                final_string += std::string_view(string_part_begin, string_it);

                // Advance iterator and read leading escape sequence character.
                auto const lead_character = *(++string_it);

                // TODO: Add proper escape sequence error handling on invalid sequences.
                auto const handler = m_escape_sequence_map.at(lead_character);

                // Get replacement and sequence length by passing entire rest
                // of the string following the backslash.
                auto [replacement, sequence_length] = handler(std::string_view(string_it, a_raw_string.cend()));

                final_string += replacement;

                // Advance iterator past escape sequence.
                string_it += static_cast<int64_t>(sequence_length);

                // Set new string part beginning iterator.
                string_part_begin = string_it;

                continue;
            }

            ++string_it;
        }

        // Add trailing string part to final string.
        final_string += std::string_view(string_part_begin, string_it + 1);

        // False positive: address escape.
        // ReSharper disable once CppDFALocalValueEscapesFunction
        return final_string;
    }

    number lexical_analyzer::parse_number(std::string_view const a_raw_string, bool const a_has_separators) {
        std::string transformed_string;

        // Filter out separators if present.
        if (a_has_separators) {
            transformed_string.resize(a_raw_string.size());

            std::size_t convert_index = 0;

            // Scan raw string and place all non-separating characters into the
            // transformed string.
            std::ranges::for_each(a_raw_string, [&convert_index, &transformed_string](char const a_char) {
                if (a_char != '\'') [[likely]] {
                    transformed_string[convert_index++] = a_char;
                }
            });

            // Resize string to match transformed size.
            transformed_string.resize(convert_index);
        } else {
            transformed_string = a_raw_string;
        }

        return std::stod(transformed_string);
    }

    integer lexical_analyzer::parse_integer(std::string_view const a_raw_string, bool const a_has_separators) {
        std::string transformed_string;

        // Filter out separators if present.
        if (a_has_separators) {
            transformed_string.resize(a_raw_string.size());

            std::size_t convert_index = 0;

            // Scan raw string and place all non-separating characters into the
            // transformed string.
            std::ranges::for_each(a_raw_string, [&convert_index, &transformed_string](char const a_char) {
                if (a_char != '\'') [[likely]] {
                    transformed_string[convert_index++] = a_char;
                }
            });

            // Resize string to match transformed size.
            transformed_string.resize(convert_index);
        } else {
            transformed_string = a_raw_string;
        }

        return std::stoll(transformed_string);
    }

}
