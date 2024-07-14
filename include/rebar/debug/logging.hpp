//
// Created by maxng on 7/13/2024.
//

#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <fmt/format.h>

namespace rebar {

    inline void debug_log(std::string_view a_string) {
        fmt::println("[REBAR - DEBUG]: {}", a_string);
    }

}

#endif //LOGGING_HPP
