//
// Created by maxng on 7/12/2024.
//

#ifndef STRING_ENGINE_HPP
#define STRING_ENGINE_HPP

#include <string>
#include <cstdint>
#include <memory>
#include <unordered_map>

#include <fmt/format.h>

#include <rebar/debug/flags.hpp>
#include <rebar/debug/logging.hpp>

namespace rebar {
     /*
     * Universal-Reference Strings: Identical strings are tied to a single
     * universal reference (a single instance of said string).
     */

    class string_engine;
    class string;

    /**
     * A struct to store a reference count and stored string for the string
     * engine.
     */
    struct internal_string {
        std::size_t       reference_count;
        std::string const string;

        /**
         * Increases the reference counter of the string.
         */
        inline void reference();

        /**
         * Decreases the reference counter and initiates garbage collection if
         * needed.
         * @param a_engine The engine that owns the string (to perform garbage collection).
         */
        inline void dereference(string_engine & a_engine);
    };

    using string_reference = internal_string *;

    /**
     *  A class to enforce universal-reference strings (that is, each unique
     *  string will only have one copy) and enable quick string operations with
     *  string indices (pointers).
     *
     *  Such operations that are optimized with this method are string
     *  comparisons and moving/copying.
     */
    class string_engine {
        /**
         * A map linking string plaintext to the internally stored
         * universal-reference string and a reference count.
         *
         * <code><reference_count : std::size_t, internal_string : std::unique_ptr<internal_string>></code>
         *
         * Strings are expected to be in UTF-8 encoding.
         */
        std::unordered_map<std::string_view, std::unique_ptr<internal_string>> m_strings;

    public:
        string_engine() noexcept = default;

        string_engine(string_engine const &) noexcept = default;
        string_engine(string_engine &&)      noexcept = default;

        string_engine & operator = (string_engine const &) noexcept = default;
        string_engine & operator = (string_engine &&)      noexcept = default;

        /**
         * Returns a Rebar string object of the supplied string.
         * @param a_string The string of which to generate a Rebar string object.
         * @return A Rebar object of the supplied string.
         */
        [[nodiscard]]
        string str(std::string_view a_string) noexcept;

        /**
         * Checks if a string has a universal reference in the engine (is stored in the engine).
         * @param a_string The string to check.
         * @return True if string is present in the engine, false if not.
         */
        [[nodiscard]]
        bool string_exists(std::string_view a_string) const noexcept;

        /**
         * Stores a string in the engine and returns a reference to the emplaced string.
         * @param a_string The string to store and return a reference.
         * @return A view of the emplaced string.
         * @note Does not overwrite any existing entries with the same string.
         */
        string_reference emplace_string(std::string a_string) noexcept;

        /**
         * Erases a string from the engine if it exists in storage.
         * @param a_string The string to erase from the engine.
         */
        void erase_string(std::string_view a_string) noexcept;
    };

    // ###################################### INLINE DEFINITIONS ######################################

    inline void internal_string::reference() {
        ++reference_count;

        // Debug string reference message.
        if constexpr (debug_string_reference_messages) {
            debug_log(fmt::format("String referenced. (Total references: {}) (\"{}\")", reference_count, string));
        }
    }

    inline void internal_string::dereference(string_engine & a_engine) {
        if (--reference_count == 0) {
            if constexpr (debug_string_reference_messages) {
                debug_log(fmt::format("String dereferenced and erased. (Total references: {}) (\"{}\")", reference_count, string));
            }

            a_engine.erase_string(string);
        } else if constexpr (debug_string_reference_messages) {
            debug_log(fmt::format("String dereferenced. (Total references: {}) (\"{}\")", reference_count, string));
        }
    }

}

#endif //STRING_ENGINE_HPP
