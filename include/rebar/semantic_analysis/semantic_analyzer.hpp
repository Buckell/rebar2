//
// Created by maxng on 23/07/2024.
//

#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <functional>

#include <rebar/lexical_analysis/lexical_unit.hpp>
#include <rebar/semantic_analysis/operators.hpp>
#include <rebar/semantic_analysis/semantic_unit.hpp>

namespace rebar {

    /**
     * A class that analyzes and parses a list of tokens into an operation
     * tree.
     */
    class semantic_analyzer {
        operator_registry m_operator_registry;

        /// Scope level increase symbols for scoped algorithms.
        std::vector<symbol> m_scope_increase_symbols = {
            symbol::brace_left,
            symbol::bracket_left,
            symbol::parenthesis_left,
            symbol::carrot_left,
        };

        /// Scope level decrease symbols for scoped algorithms.
        std::vector<symbol> m_scope_decrease_symbols = {
            symbol::brace_right,
            symbol::bracket_right,
            symbol::parenthesis_right,
            symbol::carrot_right,
        };

    public:
        inline explicit semantic_analyzer(operator_registry a_operator_registry = default_operator_registry()) noexcept;

        semantic_analyzer(semantic_analyzer const &) noexcept = default;
        semantic_analyzer(semantic_analyzer &&)      noexcept = default;

        semantic_analyzer & operator = (semantic_analyzer const &) noexcept = default;
        semantic_analyzer & operator = (semantic_analyzer&&)       noexcept = default;

        /**
         * Perform semantic analysis on a group of tokens.
         * @param a_semantic_unit The semantic unit to use for results and constraints.
         * @param a_lexical_unit The lexical unit to use as a token input.
         */
        void perform_analysis(semantic_unit & a_semantic_unit, lexical_unit const & a_lexical_unit) const;

    private:
        operation_tree      parse_block_scope(semantic_unit & a_semantic_unit, std::span<token const> a_tokens) const;
        operation_tree_node parse_expression(semantic_unit & a_semantic_unit, std::span<token const> a_tokens) const;

        /**
         * Find the first token matching the predicate function on the same
         * scope level.
         * @tparam t_predicate Type of predicate function.
         * @param a_range The range of tokens to scan.
         * @param a_predicate Predicate against which to check.
         * @return The iterator matched by the predicate or the ending iterator
         *         if nothing is found.
         */
        template <std::ranges::range t_range, std::predicate<token const &> t_predicate>
        std::span<token const>::iterator tokens_scoped_increment_find(
            t_range &&     a_range,
            t_predicate && a_predicate
        ) const noexcept;

        /**
         * Find the last token matching the predicate function on the same
         * scope level.
         * @tparam t_predicate Type of predicate function.
         * @param a_range The range of tokens to scan.
         * @param a_predicate Predicate against which to check.
         * @return The iterator matched by the predicate or the ending iterator
         *         if nothing is found.
         */
        template <std::ranges::range t_range, std::predicate<token const &> t_predicate>
        std::span<token const>::iterator tokens_scoped_increment_find_last(
            t_range &&     a_range,
            t_predicate && a_predicate
        ) const noexcept requires(std::is_same_v<std::decay_t<std::ranges::range_value_t<t_range>>, token>);

        /**
         * Filter all tokens matching predicate at the same scope level.
         * @tparam t_predicate Type of predicate function.
         * @param a_range The range of tokens to scan.
         * @param a_predicate Predicate against which to check.
         * @return The filter view of all tokens matching the predicate.
         */
        template <std::ranges::range t_range, std::predicate<token const &> t_predicate>
        auto tokens_scoped_increment_filter(
            t_range &&     a_range,
            t_predicate && a_predicate
        ) const noexcept;
    };

    // ###################################### INLINE DEFINITIONS ######################################

    semantic_analyzer::semantic_analyzer(operator_registry a_operator_registry) noexcept :
        m_operator_registry(std::move(a_operator_registry))
    {}

    template <std::ranges::range t_range, std::predicate<token const &> t_predicate>
    std::span<token const>::iterator semantic_analyzer::tokens_scoped_increment_find(
        t_range &&     a_range,
        t_predicate && a_predicate
    ) const noexcept {
        std::int64_t scope_level = 0;

        return std::ranges::find_if(
            a_range,
            [this, a_predicate, &scope_level](auto const & current_token) {
                if (current_token.is_symbol()) {
                    // Determine if token is an increase/decrease symbol.
                    if (
                        symbol const token_symbol = current_token.get_symbol();
                        std::ranges::find(m_scope_increase_symbols, token_symbol) != m_scope_increase_symbols.end()
                    ) {
                        // Return true if symbol matches an increase token and
                        // if the scope level is at zero or will be at zero.
                        return (scope_level++ == 0 || scope_level == 0) && std::invoke(a_predicate, current_token);
                    }
                    else if (std::ranges::find(m_scope_decrease_symbols, token_symbol) != m_scope_decrease_symbols.end()) {
                        // Return true if symbol matches a decrease token and
                        // if the scope level is at zero or will be at zero.
                        return (scope_level-- == 0 || scope_level == 0) && std::invoke(a_predicate, current_token);
                    }
                }

                return scope_level == 0 && std::invoke(a_predicate, current_token);
            }
        );

        // auto token_it = a_begin;
        // std::int64_t scope_level = 0;
        //
        // do {
        //     auto const & current_token = *token_it;
        //
        //     if (current_token.is_symbol()) {
        //         if (
        //             symbol const token_symbol = current_token.get_symbol();
        //             std::ranges::find(m_scope_increase_symbols, token_symbol) != m_scope_increase_symbols.end()
        //         ) {
        //             ++scope_level;
        //         }
        //         else if (std::ranges::find(m_scope_decrease_symbols, token_symbol) != m_scope_decrease_symbols.end()) {
        //             --scope_level;
        //         }
        //     }
        //
        //     if (scope_level == 0 && a_predicate(current_token)) {
        //         // ReSharper disable once CppDFALocalValueEscapesFunction
        //         return token_it;
        //     }
        // }
        // while (++token_it != a_end);
        //
        // // ReSharper disable once CppDFALocalValueEscapesFunction
        // return a_end;
    }

    template <std::ranges::range t_range, std::predicate<token const &> t_predicate>
    std::span<token const>::iterator semantic_analyzer::tokens_scoped_increment_find_last(
        t_range&&     a_range,
        t_predicate&& a_predicate
    ) const noexcept requires(std::is_same_v<std::decay_t<std::ranges::range_value_t<t_range>>, token>) {
        // (
        //     std::ranges::find_if(
        //         a_tokens | std::views::reverse,
        //         token_matches_operator_token
        //     ) + 1
        // ).base();

        std::int64_t scope_level = 0;

        return (
            std::ranges::find_if(
                a_range | std::views::reverse,
                [this, a_predicate, &scope_level](auto const & current_token) {
                    if (current_token.is_symbol()) {
                        // Determine if token is an increase/decrease symbol.
                        if (
                            symbol const token_symbol = current_token.get_symbol();
                            std::ranges::find(m_scope_increase_symbols, token_symbol) != m_scope_increase_symbols.end()
                        ) {
                            // Return true if symbol matches an increase token and
                            // if the scope level is at zero or will be at zero.
                            return (scope_level++ == 0 || scope_level == 0) && std::invoke(a_predicate, current_token);
                        }
                        else if (std::ranges::find(m_scope_decrease_symbols, token_symbol) != m_scope_decrease_symbols.end()) {
                            // Return true if symbol matches a decrease token and
                            // if the scope level is at zero or will be at zero.
                            return (scope_level-- == 0 || scope_level == 0) && std::invoke(a_predicate, current_token);
                        }
                    }

                    return scope_level == 0 && std::invoke(a_predicate, current_token);
                }
            ) + 1
        ).base();
    }

    template <std::ranges::range t_range, std::predicate<token const &> t_predicate>
    auto semantic_analyzer::tokens_scoped_increment_filter(
        t_range &&     a_range,
        t_predicate && a_predicate
    ) const noexcept {
        return a_range | std::views::filter([this, a_predicate](token const & current_token) {
            static std::int64_t scope_level = 0;

            if (current_token.is_symbol()) {
                // Determine if token is an increase/decrease symbol.

                if (
                    symbol const token_symbol = current_token.get_symbol();
                    std::ranges::find(m_scope_increase_symbols, token_symbol) != m_scope_increase_symbols.end()
                ) {
                    // Return true if symbol matches an increase token and
                    // if the scope level is at zero or will be at zero.
                    return (scope_level++ == 0 || scope_level == 0) && std::invoke(a_predicate, current_token);
                }
                else if (std::ranges::find(m_scope_decrease_symbols, token_symbol) != m_scope_decrease_symbols.end()) {
                    // Return true if symbol matches a decrease token and
                    // if the scope level is at zero or will be at zero.
                    return (scope_level-- == 0 || scope_level == 0) && std::invoke(a_predicate, current_token);
                }
            }

            return scope_level == 0 && std::invoke(a_predicate, current_token);
        });
    }

}

#endif //SEMANTIC_ANALYZER_H
