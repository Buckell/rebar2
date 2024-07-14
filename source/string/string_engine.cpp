//
// Created by maxng on 7/13/2024.
//

#include <rebar/string/string_engine.hpp>
#include <rebar/string/string.hpp>

namespace rebar {

    string string_engine::str(std::string_view const a_string) noexcept {
        // Return reference if already existing.
        if (auto const it = m_strings.find(a_string); it != m_strings.cend()) {
            return { *this, it->second.get() };
        }

        return { *this, emplace_string(std::string(a_string)) };
    }

    bool string_engine::string_exists(std::string_view const a_string) const noexcept {
        return m_strings.contains(a_string);
    }

    string_reference string_engine::emplace_string(std::string a_string) noexcept {
        // Construct pointer and store view reference.
        auto string_pointer = std::make_unique<internal_string>(0ull, std::move(a_string));
        std::string_view string_reference = string_pointer->string;

        // Map view to the storage location.
        const auto [it, insertion] = m_strings.emplace(string_reference, std::move(string_pointer));

        return it->second.get();
    }

    void string_engine::erase_string(std::string_view const a_string) noexcept {
        // Erase if string exists.
        if (auto const it = m_strings.find(a_string); it != m_strings.cend()) {
            m_strings.erase(it);
        }
    }

}