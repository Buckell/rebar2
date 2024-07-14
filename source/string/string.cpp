//
// Created by maxng on 7/13/2024.
//

#include <rebar/string/string.hpp>
#include <rebar/string/string_engine.hpp>

namespace rebar {

    string::string(string_engine &a_engine, string_reference const a_container) noexcept : // NOLINT(*-misplaced-const)
        m_engine(&a_engine),
        m_container(a_container)
    {
        m_container->reference();
    }

    string::~string() {
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

}