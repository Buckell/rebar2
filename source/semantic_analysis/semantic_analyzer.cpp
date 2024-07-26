//
// Created by maxng on 24/07/2024.
//

#include <algorithm>
#include <ranges>
#include <rebar/semantic_analysis/semantic_analyzer.hpp>

namespace rebar {

    void semantic_analyzer::perform_analysis(semantic_unit & a_semantic_unit, lexical_unit const & a_lexical_unit) const {
        a_semantic_unit.apply_base_scope(parse_block_scope(a_semantic_unit, a_lexical_unit.tokens()));
    }

    operation_tree semantic_analyzer::parse_block_scope(semantic_unit & a_semantic_unit, std::span<token const> a_tokens) const {
        operation_tree base_scope(operation::scope);

        auto token_it = a_tokens.begin();

        while (token_it != a_tokens.end()) {
            // Testing for non-simple statement constructions.

            // Statement parsing.
            auto const statement_begin = token_it;

            auto const statement_end = tokens_scoped_increment_find(
                statement_begin,
                a_tokens.end(),
                [](auto const & a_token) {
                    return a_token == symbol::semicolon;
                }
            );

            token_it = statement_end + 1;

            base_scope.push_operand(
                parse_expression(
                    a_semantic_unit,
                    { statement_begin, statement_end }
                )
            );
        }

        return base_scope;
    }

    operation_tree_node semantic_analyzer::parse_expression(semantic_unit & a_semantic_unit, std::span<token const> a_tokens) const { // NOLINT(*-no-recursion)
        // If expression is surrounded in parenthesis, parse inner content.
        while (*a_tokens.begin() == symbol::parenthesis_left && *a_tokens.end() == symbol::parenthesis_right) {
            a_tokens = { a_tokens.begin() + 1, a_tokens.end() - 1 };
        }

        if (a_tokens.size() == 1) {
            return a_tokens[0];
        }

        auto expression_tree = std::make_unique<operation_tree>();

        auto symbol_tokens = std::ranges::views::filter(a_tokens, &token::is_symbol);

        if (symbol_tokens.empty()) {
            // TODO: Throw error.
        }

        // Projection function for token precedence.
        auto const operator_token_precedence = [this](auto const & current_token) {
            const auto it = std::ranges::find(
                m_operator_registry,
                current_token.get_symbol(),
                &operator_info::identifier
            );

            return it != m_operator_registry.cend() ? it->precedence : 0;
        };

        // Find lowest precedence operator.
        auto const & lowest_operator_token = std::ranges::min(
            symbol_tokens,
            {},
            operator_token_precedence
        );

        auto const info_matches_operator_token = [&lowest_operator_token](auto const & op_info) {
            return op_info.identifier == lowest_operator_token.get_symbol();
        };

        // Find all operators that match lowest precedence symbol.
        auto op_infos = std::ranges::views::filter(m_operator_registry, info_matches_operator_token);

        auto const token_matches_operator_token = [&lowest_operator_token](auto const & current_token) {
            return current_token.is_symbol() && current_token.get_symbol() == lowest_operator_token.get_symbol();
        };

        // Find last operator token matching symbol.
        auto op_it = tokens_scoped_increment_find_last(a_tokens.begin(), a_tokens.end(), token_matches_operator_token);
        // (
        //     std::ranges::find_if(
        //         a_tokens | std::views::reverse,
        //         token_matches_operator_token
        //     ) + 1
        // ).base();

        if (op_it == a_tokens.begin()) {
            const auto prefix_operator = std::ranges::find_if(op_infos, [](auto const & op_info) {
                return op_info.type == operator_type::unary && op_info.association == operator_association::right;
            });

            if (prefix_operator != op_infos.end()) {
                expression_tree->set_op(prefix_operator->mapped_operation);

                expression_tree->set_operand(
                    0,
                    parse_expression(
                        a_semantic_unit,
                        { a_tokens.begin() + 1, a_tokens.end() }
                    )
                );
            } else {
                // TODO: Throw error.
            }
        }
        else if (op_it == a_tokens.end() - 1) {
            const auto postfix_operator = std::ranges::find_if(op_infos, [](auto const & op_info) {
                return op_info.type == operator_type::unary && op_info.association == operator_association::left;
            });

            if (postfix_operator != op_infos.end()) {
                expression_tree->set_op(postfix_operator->mapped_operation);

                expression_tree->set_operand(
                    0,
                    parse_expression(
                        a_semantic_unit,
                        { a_tokens.begin(), a_tokens.end() - 1 }
                    )
                );
            } else {
                // TODO: Throw error.
            }
        }
        else {
            auto const matched_op = *op_infos.begin();

            // Match first operator in series if right associated.
            if (matched_op.association == operator_association::right) {
                op_it = tokens_scoped_increment_find(a_tokens.begin(), a_tokens.end(), token_matches_operator_token);
            }

            if (matched_op.type == operator_type::binary) {
                expression_tree->set_op(matched_op.mapped_operation);

                expression_tree->set_operand(
                    0,
                    parse_expression(
                        a_semantic_unit,
                        { a_tokens.begin(), op_it }
                    )
                );

                expression_tree->set_operand(
                    1,
                    parse_expression(
                        a_semantic_unit,
                        { op_it + 1, a_tokens.end() }
                    )
                );
            }
        }

        return std::move(expression_tree);
    }


}
