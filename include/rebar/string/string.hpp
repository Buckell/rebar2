//
// Created by maxng on 7/13/2024.
//

#ifndef STRING_HPP
#define STRING_HPP

#include <iostream>

#include <rebar/string/string_engine.hpp>

namespace rebar {

    class string_engine;

    class string {
        string_engine *  m_engine;
        string_reference m_container;

    public:
        /// Null string constructor.
        inline string() noexcept;

        inline string(string_engine& a_engine, string_reference a_container) noexcept;

        inline ~string() noexcept;

        /**
         * Constructs a copy of the provided string. Creates a new reference.
         */
        inline string(string const &) noexcept;

        /**
         * Moves the provided string. Does not create a new reference.
         * @note The provided string will be invalid once the move is complete.
         *       Using this string will result in undefined behavior.
         */
        inline string(string &&) noexcept;

        /**
         * Assigns a copy of the provided string. Creates a new reference.
         * @return The assigned string.
         */
        inline string & operator = (string const &) noexcept;

        /**
         * Moves the provided string. Does not create a new reference.
         * @return The assigned string.
         * @note The provided string will be invalid once the move is complete.
         *       Using this string will result in undefined behavior.
         */
        inline string & operator = (string &&) noexcept;

        [[nodiscard]]
        inline bool operator == (string const & a_string) const noexcept;

        [[nodiscard]]
        inline std::string_view view() const noexcept;

        [[nodiscard]]
        inline string_engine & parent_engine() const noexcept;

        [[nodiscard]]
        inline string_reference reference() const noexcept;

    private:
        friend class object;
    };

    // IO stream interoperability.
    inline std::ostream & operator << (std::ostream & a_stream, string const & a_string) noexcept {
        return a_stream << a_string.view();
    }

    // ###################################### INLINE DEFINITIONS ######################################

    string::string() noexcept :
        m_engine(nullptr),
        m_container(nullptr)
    {}

    string::string(string_engine & a_engine, string_reference const a_container) noexcept : // NOLINT(*-misplaced-const)
        m_engine(&a_engine),
        m_container(a_container)
    {
        m_container->reference();
    }

    string::~string() {
        // Do nothing if string is null.
        if (m_container == nullptr) {
            return;
        }

        m_container->dereference(*m_engine);
    }

    string::string(string const & m_string) noexcept :
        string(*m_string.m_engine, m_string.m_container)
    {}

    string::string(string && m_string) noexcept :
        m_engine(m_string.m_engine),
        m_container(m_string.m_container)
    {
        m_string.m_container = nullptr;
    }

    string & string::operator = (string const & a_string) noexcept {
        if (this == &a_string) {
            return *this;
        }

        m_engine = a_string.m_engine;
        m_container = a_string.m_container;

        m_container->reference();

        return *this;
    }

    string & string::operator = (string && a_string) noexcept {
        m_engine = a_string.m_engine;
        m_container = a_string.m_container;

        a_string.m_container = nullptr;

        return *this;
    }

    bool string::operator == (string const & a_string) const noexcept {
        return m_container == a_string.m_container;
    }

    std::string_view string::view() const noexcept {
        return m_container->string;
    }

    inline string_engine & string::parent_engine() const noexcept {
        return *m_engine;
    }

    inline string_reference string::reference() const noexcept {
        return m_container;
    }

}

#endif //STRING_HPP
