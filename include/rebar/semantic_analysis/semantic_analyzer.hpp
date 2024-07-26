//
// Created by maxng on 23/07/2024.
//

#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

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
         * Find the next token matching the predicate function on the same
         * scope level.
         * @tparam t_predicate Type of predicate function.
         * @param a_begin Beginning token iterator.
         * @param a_end Ending token iterator.
         * @param a_predicate Predicate against which to check.
         * @return The iterator matched by the predicate or the ending iterator
         *         if nothing is found.
         */
        template <std::predicate<token const &> t_predicate>
        static std::span<token const>::iterator tokens_scoped_increment_find(
            std::span<token const>::iterator a_begin,
            std::span<token const>::iterator a_end,
            t_predicate                      a_predicate
        ) noexcept;
    };

    // ###################################### INLINE DEFINITIONS ######################################

    semantic_analyzer::semantic_analyzer(operator_registry a_operator_registry) noexcept :
        m_operator_registry(std::move(a_operator_registry))
    {}

    template<std::predicate<token const &> t_predicate>
    std::span<token const>::iterator semantic_analyzer::tokens_scoped_increment_find(
        std::span<token const>::iterator const a_begin,
        std::span<token const>::iterator const a_end,
        t_predicate a_predicate
    ) noexcept {
        auto token_it = a_begin;
        std::size_t scope_level = 0;

        while (++token_it != a_end) {
            auto const & current_token = *token_it;

            if (scope_level == 0 && a_predicate(current_token)) {
                return token_it;
            }

            if (
                current_token == symbol::brace_left ||
                current_token == symbol::bracket_left ||
                current_token == symbol::parenthesis_left
            ) {
                ++scope_level;
            }
            else if (
                current_token == symbol::brace_right ||
                current_token == symbol::bracket_right ||
                current_token == symbol::parenthesis_right
            ) {
                --scope_level;
            }
        }

        return a_end;
    }

}

#endif //SEMANTIC_ANALYZER_H
