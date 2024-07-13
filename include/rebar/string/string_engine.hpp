//
// Created by maxng on 7/12/2024.
//

#ifndef STRING_ENGINE_HPP
#define STRING_ENGINE_HPP

#include <memory>
#include <unordered_map>

namespace rebar {

    /*
     * Universal-Reference Strings: Identical strings are tied to a single universal reference (a single instance of
     * said string).
     */

    /**
     *  A class to enforce universal-reference strings (that is, each unique
     *  string will only have one copy) and enable quick string operations with
     *  string indices (pointers).
     *
     *  Such operations that are optimized with this method are string
     *  comparisons and moving/copying.
     */
    class string_engine {
        /// UTF-8 Encoding
        std::unordered_map<std::string_view, std::unique_ptr<std::string>> m_strings;

    public:
        string_engine() noexcept = default;

        string_engine(string_engine const &) noexcept = default;
        string_engine(string_engine &&)      noexcept = default;

        string_engine & operator = (string_engine const &) noexcept = default;
        string_engine & operator = (string_engine &&)      noexcept = default;

        /**
         * Returns a universal reference to the specified string.
         * @param m_string The string of which to get a universe reference.
         * @return A universal reference to the supplied string.
         * @note Using emplace_string may be more efficient if a string is
         *       unlikely to already exist or a preexisting string reference
         *       is available to be moved.
         */
        [[nodiscard]]
        std::string_view str(std::string_view m_string) noexcept;

        /**
         * Checks if a string has a universal reference in the engine (is stored in the engine).
         * @param m_string The string to check.
         * @return True if string is present in the engine, false if not.
         */
        [[nodiscard]]
        bool string_exists(std::string_view m_string) const noexcept;

        /**
         * Stores a string in the engine and returns a reference to the emplaced string.
         * @return A view of the emplaced string.
         * @note Does not overwrite any existing entries with the same string.
         */
        std::string_view emplace_string(std::string m_string) noexcept;

        /**
         * Erases a string from the engine if it exists in storage.
         * @param m_string The string to erase from the engine.
         */
        void erase_string(std::string_view m_string) noexcept;
    };

}

#endif //STRING_ENGINE_HPP
