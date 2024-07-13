//
// Created by maxng on 7/13/2024.
//

#include <rebar/string/string_engine.hpp>

namespace rebar {

    std::string_view string_engine::str(std::string_view const m_string) noexcept {
        // Return reference if already existing.
        if (auto const it = m_strings.find(m_string); it != m_strings.cend()) {
            return it->first;
        }

        return emplace_string(std::string(m_string));
    }

    bool string_engine::string_exists(std::string_view const m_string) const noexcept {
        return m_strings.contains(m_string);
    }

    std::string_view string_engine::emplace_string(std::string m_string) noexcept {
        // Construct string pointer and store view reference.
        auto string_pointer = std::make_unique<std::string>(std::move(m_string));
        std::string_view string_reference = *string_pointer;

        // Map view to the storage location.
        const auto [it, insertion] = m_strings.emplace(string_reference, std::move(string_pointer));

        return it->first;
    }

    void string_engine::erase_string(std::string_view const m_string) noexcept {
        // Erase if string exists.
        if (auto const it = m_strings.find(m_string); it != m_strings.cend()) {
            m_strings.erase(it);
        }
    }

}