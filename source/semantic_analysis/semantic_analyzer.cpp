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
                statement_begin,
                a_tokens.end(),
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
        // If expression is surrounded in parenthesis, redefine tokens to inner content.
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

        // Search data.
        auto symbol_tokens = tokens_scoped_increment_filter(a_tokens, &token::is_symbol);
        std::vector<symbol> skip_table;

        // Selected operator info.
        operator_info current_operation{ .precedence = 9999 };
        auto current_operation_token_it = a_tokens.end();
        auto secondary_symbol_token_it = a_tokens.end();

        // Select next operator.
        for (auto symbol_token_it = symbol_tokens.begin(); symbol_token_it != symbol_tokens.end(); ++symbol_token_it) {
            auto const token_it = symbol_token_it.base();
            auto const & token = *token_it;
            auto const token_symbol = token.get_symbol();

            // Skip and pop and symbols in the skip table.
            if (auto const it = std::ranges::find(skip_table, token_symbol); it != skip_table.cend()) {
                skip_table.erase(it);

                // If current operation uses a second symbol and matches
                // current token, keep track.
                if (current_operation.secondary == token_symbol) {
                    secondary_symbol_token_it = token_it;
                }

                continue;
            }

            auto operator_infos = m_operator_registry | std::ranges::views::filter([current_operation, token_symbol](operator_info const & entry) {
                return token_symbol == entry.identifier && entry.precedence <= current_operation.precedence;
            });

            if (operator_infos.empty()) {
                continue;
            }

            if (
                auto const secondary_symbol_op_it = std::ranges::find_if(
                    operator_infos,
                    not_equal_to(symbol::null),
                    &operator_info::secondary
                );
                secondary_symbol_op_it != operator_infos.end()
            ) {
                auto const & secondary_symbol_op = *secondary_symbol_op_it;

                skip_table.push_back(secondary_symbol_op.secondary);

                if (token_it == a_tokens.begin() && secondary_symbol_op.association == operator_association::right) {
                    current_operation = secondary_symbol_op;
                    current_operation_token_it = token_it;

                    continue;
                }
            }

            if (token_it == a_tokens.begin()) {
                auto const prefix_unary_operator_it = std::ranges::find_if(operator_infos, [](auto const & info) {
                    return info.type == operator_type::unary && info.association == operator_association::right;
                });

                if (prefix_unary_operator_it == operator_infos.end()) {
                    // Throw error?
                    continue;
                }

                current_operation = *prefix_unary_operator_it;
                current_operation_token_it = token_it;

                break;
            }

            if (token_it == a_tokens.end() - 1) {
                auto const postfix_unary_operator_it = std::ranges::find_if(operator_infos, [](auto const & info) {
                    return info.type == operator_type::unary && info.association == operator_association::left;
                });

                if (postfix_unary_operator_it == operator_infos.end()) {
                    // Throw error?
                    continue;
                }

                current_operation = *postfix_unary_operator_it;
                current_operation_token_it = token_it;

                break;
            }

            auto const & op_info = *operator_infos.begin();

            if (op_info.precedence == current_operation.precedence && op_info.association == operator_association::right) {
                continue;
            }

            current_operation = op_info;
            current_operation_token_it = token_it;
        }

        expression_tree->set_op(current_operation.mapped_operation);

        if (current_operation.type == operator_type::unary) {
            if (current_operation.association == operator_association::left) {
                expression_tree->set_operand(
                    0,
                    parse_expression(
                        a_semantic_unit,
                        { a_tokens.begin(), current_operation_token_it }
                    )
                );
            }
            else {
                expression_tree->set_operand(
                    0,
                    parse_expression(
                        a_semantic_unit,
                        { current_operation_token_it + 1, a_tokens.end() }
                    )
                );
            }
        }
        else if (current_operation.type == operator_type::binary) {
            expression_tree->set_operand(
                0,
                parse_expression(
                    a_semantic_unit,
                    { a_tokens.begin(), current_operation_token_it }
                )
            );

            expression_tree->set_operand(
                1,
                parse_expression(
                    a_semantic_unit,
                    { current_operation_token_it + 1, a_tokens.end() }
                )
            );
        }
        else if (current_operation.type == operator_type::binary_enclose) {
            if (current_operation.association == operator_association::left) {
                expression_tree->set_operand(
                    0,
                    parse_expression(
                        a_semantic_unit,
                        { a_tokens.begin(), current_operation_token_it }
                    )
                );

                expression_tree->set_operand(
                    1,
                    parse_expression(
                        a_semantic_unit,
                        { current_operation_token_it + 1, secondary_symbol_token_it }
                    )
                );
            }
            else {
                expression_tree->set_operand(
                    0,
                    parse_expression(
                        a_semantic_unit,
                        { current_operation_token_it + 1, secondary_symbol_token_it }
                    )
                );

                expression_tree->set_operand(
                    1,
                    parse_expression(
                        a_semantic_unit,
                        { secondary_symbol_token_it + 1, a_tokens.end() }
                    )
                );
            }
        }
        else if (current_operation.type == operator_type::variadic) {
            auto expression_begin = a_tokens.begin();
            auto expression_end   = a_tokens.begin();

            // Add expressions to operator operands, delimited by operator.
            while (true) {
                expression_end = tokens_scoped_increment_find(
                    expression_begin,
                    a_tokens.end(),
                    equal_to(current_operation.identifier)
                );

                expression_tree->push_operand(
                    parse_expression(
                        a_semantic_unit,
                        { expression_begin, expression_end }
                    )
                );

                if (expression_end != a_tokens.end()) {
                    expression_begin = expression_end + 1;
                } else {
                    break;
                }
            }
        }

        // Old algorithm.
        //
        // // Projection function for token operator precedence.
        // auto const operator_token_precedence = [this](auto const & current_token) {
        //     const auto it = std::ranges::find(
        //         m_operator_registry,
        //         current_token.get_symbol(),
        //         &operator_info::identifier
        //     );
        //
        //     return it != m_operator_registry.cend() ? it->precedence : 9999;
        // };
        //
        // auto lowest_operator_symbol = symbol::null;
        //
        // // Find lowest precedence operator symbol.
        // std::size_t lowest_precedence = std::numeric_limits<std::size_t>::max();
        //
        // for (auto const & current_token : tokens_scoped_increment_filter(a_tokens, &token::is_symbol)) {
        //     if (
        //         auto const precedence = operator_token_precedence(current_token);
        //         precedence <= lowest_precedence
        //     ) {
        //         lowest_precedence = precedence;
        //         lowest_operator_symbol = current_token.get_symbol();
        //     }
        // }
        //
        // // Predicate for matching operator info to the matched symbol.
        // auto const info_matches_operator_symbol = [lowest_operator_symbol](auto const & op_info) {
        //     return op_info.identifier == lowest_operator_symbol;
        // };
        //
        // // Find all operators that match lowest precedence symbol.
        // auto op_infos = std::ranges::views::filter(m_operator_registry, info_matches_operator_symbol);
        //
        // // Find last operator token matching symbol.
        // auto op_it = tokens_scoped_increment_find_last(a_tokens, equal_to(lowest_operator_symbol));
        //
        // // Test if operator is a prefix operator.
        // if (op_it == a_tokens.begin() || (*a_tokens.begin() == lowest_operator_symbol && lowest_operator_symbol != symbol::parenthesis_left)) {
        //     // Find prefix operator entry.
        //     const auto prefix_operator = std::ranges::find_if(op_infos, [](auto const & op_info) {
        //         return op_info.type == operator_type::unary && op_info.association == operator_association::right;
        //     });
        //
        //     // Test that prefix operator exists and complete tree.
        //     if (prefix_operator != op_infos.end()) {
        //         expression_tree->set_op(prefix_operator->mapped_operation);
        //
        //         expression_tree->set_operand(
        //             0,
        //             parse_expression(
        //                 a_semantic_unit,
        //                 { a_tokens.begin() + 1, a_tokens.end() }
        //             )
        //         );
        //     } else {
        //         // TODO: Throw error.
        //     }
        // }
        // // Test if operator is a postfix operator.
        // else if (op_it == a_tokens.end() - 1) {
        //     // Find postfix operator entry.
        //     const auto postfix_operator = std::ranges::find_if(op_infos, [](auto const & op_info) {
        //         return op_info.type == operator_type::unary && op_info.association == operator_association::left;
        //     });
        //
        //     // Test that postfix operator exists and complete tree.
        //     if (postfix_operator != op_infos.end()) {
        //         expression_tree->set_op(postfix_operator->mapped_operation);
        //
        //         expression_tree->set_operand(
        //             0,
        //             parse_expression(
        //                 a_semantic_unit,
        //                 { a_tokens.begin(), a_tokens.end() - 1 }
        //             )
        //         );
        //     } else {
        //         // TODO: Throw error.
        //     }
        // }
        // // Operator is a binary, trinary, etc. operator (non-prefix/postfix).
        // else {
        //     auto const matched_op = *op_infos.begin();
        //
        //     // Match first operator in series if right associated.
        //     if (matched_op.association == operator_association::right) {
        //         op_it = tokens_scoped_increment_find(a_tokens, equal_to(lowest_operator_symbol));
        //     }
        //
        //     if (matched_op.type == operator_type::binary) {
        //         expression_tree->set_op(matched_op.mapped_operation);
        //
        //         expression_tree->set_operand(
        //             0,
        //             parse_expression(
        //                 a_semantic_unit,
        //                 { a_tokens.begin(), op_it }
        //             )
        //         );
        //
        //         expression_tree->set_operand(
        //             1,
        //             parse_expression(
        //                 a_semantic_unit,
        //                 { op_it + 1, a_tokens.end() }
        //             )
        //         );
        //     }
        //     else if (matched_op.type == operator_type::binary_enclose) {
        //         // Find closing operator.
        //         auto const end_op_it = tokens_scoped_increment_find(
        //             op_it,
        //             a_tokens.end(),
        //             equal_to(matched_op.secondary)
        //         );
        //
        //         expression_tree->set_op(matched_op.mapped_operation);
        //
        //         // Select side of expression for base depending on association.
        //         if (matched_op.association == operator_association::left) {
        //             expression_tree->set_operand(
        //                 0,
        //                 parse_expression(
        //                     a_semantic_unit,
        //                     { a_tokens.begin(), op_it }
        //                 )
        //             );
        //
        //             expression_tree->set_operand(
        //                 1,
        //                 parse_expression(
        //                     a_semantic_unit,
        //                     { op_it + 1, end_op_it }
        //                 )
        //             );
        //         }
        //         else if (matched_op.association == operator_association::right) {
        //             expression_tree->set_operand(
        //                 0,
        //                 parse_expression(
        //                     a_semantic_unit,
        //                     { end_op_it + 1, a_tokens.end() }
        //                 )
        //             );
        //
        //             expression_tree->set_operand(
        //                 1,
        //                 parse_expression(
        //                     a_semantic_unit,
        //                     { op_it + 1, end_op_it }
        //                 )
        //             );
        //         }
        //     }
        //     else if (matched_op.type == operator_type::variadic) {
        //         expression_tree->set_op(matched_op.mapped_operation);
        //
        //         auto expression_begin = a_tokens.begin();
        //         auto expression_end   = a_tokens.begin();
        //
        //         // Add expressions to operator operands, delimited by operator.
        //         while (true) {
        //             expression_end = tokens_scoped_increment_find(
        //                 expression_begin,
        //                 a_tokens.end(),
        //                 equal_to(lowest_operator_symbol)
        //             );
        //
        //             expression_tree->push_operand(
        //                 parse_expression(
        //                     a_semantic_unit,
        //                     { expression_begin, expression_end }
        //                 )
        //             );
        //
        //             if (expression_end != a_tokens.end()) {
        //                 expression_begin = expression_end + 1;
        //             } else {
        //                 break;
        //             }
        //         }
        //     }
        // }

        return std::move(expression_tree);
    }


}
