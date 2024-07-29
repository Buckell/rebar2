//
// Created by maxng on 24/07/2024.
//

#include <algorithm>
#include <ranges>

#include <rebar/semantic_analysis/semantic_analyzer.hpp>
#include <rebar/util/equal_to.hpp>
#include <rebar/util/print.hpp>

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
                std::span(statement_begin, a_tokens.end()),
                [](auto const & a_token) {
                    return a_token == symbol::semicolon;
                }
            );

            token_it = statement_end == a_tokens.end() ? statement_end : statement_end + 1;

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
        while (
            *a_tokens.begin() == symbol::parenthesis_left &&
            tokens_scoped_increment_find(
                a_tokens,
                equal_to(symbol::parenthesis_right)
            ) == a_tokens.end() - 1
        ) {
            a_tokens = { a_tokens.begin() + 1, a_tokens.end() - 1 };
        }

        if (a_tokens.size() == 1) {
            return a_tokens[0];
        }

        auto expression_tree = std::make_unique<operation_tree>();

        // Projection function for token precedence.
        auto const operator_token_precedence = [this](auto const & current_token) {
            const auto it = std::ranges::find(
                m_operator_registry,
                current_token.get_symbol(),
                &operator_info::identifier
            );

            return it != m_operator_registry.cend() ? it->precedence : 9999;
        };

        auto lowest_operator_symbol = symbol::null;
        std::size_t lowest_precedence = std::numeric_limits<std::size_t>::max();

        // Find lowest precedence operator symbol.
        for (auto const & current_token : tokens_scoped_increment_filter(a_tokens, &token::is_symbol)) {
            if (
                auto const precedence = operator_token_precedence(current_token);
                precedence <= lowest_precedence
            ) {
                lowest_precedence = precedence;
                lowest_operator_symbol = current_token.get_symbol();
            }
        }

        // Predicate for matching operator info to the matched symbol.
        auto const info_matches_operator_symbol = [lowest_operator_symbol](auto const & op_info) {
            return op_info.identifier == lowest_operator_symbol;
        };

        // Find all operators that match lowest precedence symbol.
        auto op_infos = std::ranges::views::filter(m_operator_registry, info_matches_operator_symbol);

        // Find last operator token matching symbol.
        auto op_it = tokens_scoped_increment_find_last(a_tokens, equal_to(lowest_operator_symbol));

        if (op_it == a_tokens.begin() || (*a_tokens.begin() == lowest_operator_symbol && lowest_operator_symbol != symbol::parenthesis_left)) {
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
                op_it = tokens_scoped_increment_find(a_tokens, equal_to(lowest_operator_symbol));
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
            else if (matched_op.type == operator_type::binary_enclose) {
                auto const end_op_it = tokens_scoped_increment_find(
                    std::span(op_it, a_tokens.end()),
                    equal_to(matched_op.secondary)
                );

                expression_tree->set_op(matched_op.mapped_operation);

                if (matched_op.association == operator_association::left) {
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
                            { op_it + 1, end_op_it }
                        )
                    );
                }
                else if (matched_op.association == operator_association::right) {
                    expression_tree->set_operand(
                        0,
                        parse_expression(
                            a_semantic_unit,
                            { end_op_it + 1, a_tokens.end() }
                        )
                    );

                    expression_tree->set_operand(
                        1,
                        parse_expression(
                            a_semantic_unit,
                            { op_it + 1, end_op_it }
                        )
                    );
                }
            }
            else if (matched_op.type == operator_type::variadic) {
                expression_tree->set_op(matched_op.mapped_operation);

                auto expression_begin = a_tokens.begin();
                auto expression_end   = a_tokens.begin();

                do {
                    expression_end = tokens_scoped_increment_find(
                        std::span(expression_begin, a_tokens.end()),
                        equal_to(lowest_operator_symbol)
                    );

                    expression_tree->push_operand(
                        parse_expression(
                            a_semantic_unit,
                            { expression_begin, expression_end }
                        )
                    );

                    expression_begin = expression_end + 1;
                }
                while (expression_end != a_tokens.end());
            }
        }

        return std::move(expression_tree);
    }


}
