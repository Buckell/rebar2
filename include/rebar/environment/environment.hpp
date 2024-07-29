//
// Created by maxng on 7/12/2024.
//

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <rebar/environment/object.hpp>
#include <rebar/lexical_analysis/lexical_analyzer.hpp>
#include <rebar/semantic_analysis/semantic_analyzer.hpp>
#include <rebar/string/string_engine.hpp>

namespace rebar {

    class environment : string_engine, lexical_analyzer, semantic_analyzer {
    public:
        inline environment() noexcept;

        environment(environment const &)     = delete;
        environment(environment &&) noexcept = default;

        environment & operator = (environment const &)     = delete;
        environment & operator = (environment &&) noexcept = default;

        using string_engine::str;

        using lexical_analyzer::perform_analysis;
        using lexical_analyzer::process_string;

        using semantic_analyzer::perform_analysis;

    private:
        friend void rebar::reference_object(object const * a_object) noexcept;
        friend void rebar::dereference_object(object const * a_object) noexcept;
    };

    // ###################################### INLINE DEFINITIONS ######################################

    environment::environment() noexcept :
        lexical_analyzer(dynamic_cast<string_engine &>(*this))
    {}


}

#endif //ENVIRONMENT_HPP
