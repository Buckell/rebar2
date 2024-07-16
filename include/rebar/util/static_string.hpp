//
// Created by maxng on 7/16/2024.
//

#ifndef STATIC_STRING_HPP
#define STATIC_STRING_HPP

#include <algorithm>
#include <cstdint>

namespace rebar {

    template <std::size_t v_string_size>
    struct static_string {
        static constexpr std::size_t size = v_string_size;
        char value[size];

        consteval static_string(const char(&a_string)[v_string_size]) : // NOLINT(*-explicit-constructor)
            value()                                                     // Conversion intentional.
        {
            std::copy_n(a_string, size, value);
        }
    };

}

#endif //STATIC_STRING_HPP
