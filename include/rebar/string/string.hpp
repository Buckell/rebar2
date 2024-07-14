//
// Created by maxng on 7/13/2024.
//

#ifndef STRING_HPP
#define STRING_HPP

#include <rebar/string/string_engine.hpp>

namespace rebar {

    class string_engine;

    class string {
        string_engine* m_engine;
        string_reference m_container;

    public:
        string(string_engine& a_engine, string_reference a_container) noexcept;

        ~string() noexcept;

        /**
         * Constructs a copy of the provided string. Creates a new reference.
         */
        string(string const &) noexcept;

        /**
         * Moves the provided string. Does not create a new reference.
         * @note The provided string will be invalid once the move is complete.
         *       Using this string will result in undefined behavior.
         */
        string(string &&) noexcept;

        /**
         * Assigns a copy of the provided string. Creates a new reference.
         * @return The assigned string.
         */
        string & operator = (string const &) noexcept;

        /**
         * Moves the provided string. Does not create a new reference.
         * @return The assigned string.
         * @note The provided string will be invalid once the move is complete.
         *       Using this string will result in undefined behavior.
         */
        string & operator = (string &&) noexcept;
    };

}

#endif //STRING_HPP
