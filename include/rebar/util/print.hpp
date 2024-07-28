//
// Created by maxng on 27/07/2024.
//

#ifndef PRINT_HPP
#define PRINT_HPP

#include <functional>

#include <rebar/lexical_analysis/token.hpp>
#include <rebar/semantic_analysis/operation_tree.hpp>

namespace rebar {

    template <typename ...t_args>
    void print(t_args const & ...a_args) noexcept {
        (std::cout << ... << a_args);
    }

    template <typename ...t_args>
    void println(t_args const & ...a_args) noexcept {
        (std::cout << ... << a_args) << '\n';
    }

    template <std::convertible_to<std::span<token const>> t_range>
    void print(t_range&& a_range) noexcept {
        std::ranges::for_each(
            std::forward<t_range>(a_range),
            []<typename t_string>(t_string && a_string) {
                rebar::print<std::string_view, std::string_view>(std::forward<t_string>(a_string), ", ");
            },
            &token::to_string
        );
    }

    template <std::convertible_to<std::span<token const>> t_range>
    void println(t_range&& a_range) noexcept {
        std::ranges::for_each(
            std::forward<t_range>(a_range),
            rebar::println<std::string_view>,
            &token::to_string
        );
    }

    inline void print(operation_tree const & a_operation_tree) noexcept {
        print(a_operation_tree.to_string());
    }

    inline void println(operation_tree const & a_operation_tree) noexcept {
        println(a_operation_tree.to_string());
    }

}

#endif //PRINT_HPP

