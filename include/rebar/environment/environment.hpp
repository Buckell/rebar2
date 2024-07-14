//
// Created by maxng on 7/12/2024.
//

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <rebar/string/string_engine.hpp>

namespace rebar {

    class environment : string_engine {
    public:
        environment() noexcept = default;

        environment(environment const &)     = delete;
        environment(environment &&) noexcept = default;

        environment & operator = (environment const &)     = delete;
        environment & operator = (environment &&) noexcept = default;

        using string_engine::str;

    };

}

#endif //ENVIRONMENT_HPP
