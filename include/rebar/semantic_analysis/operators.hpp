//
// Created by maxng on 23/07/2024.
//

#ifndef OPERATORS_H
#define OPERATORS_H

#include <rebar/lexical_analysis/symbol.hpp>

namespace rebar {

    /**
     * An enumeration for Rebar operations/operators.
     */
    enum class operation {
        null = 0,

        function,
        scope,
        conditional,
        preconditional_loop,
        postconditional_loop,
        conditional_initialization_loop,

        assignment,

        addition,
        addition_assignment,
        subtraction,
        subtraction_assignment,
        multiplication,
        multiplication_assignment,
        division,
        division_assignment,
        exponentiation,
        exponentiation_assignment,
        modulo,
        modulo_assignment,

        bitwise_not,
        bitwise_and,
        bitwise_and_assignment,
        bitwise_or,
        bitwise_or_assignment,
        bitwise_xor,
        bitwise_xor_assignment,

        logical_not,
        logical_and,
        logical_and_assignment,
        logical_or,
        logical_or_assignment,
        logical_xor,
        logical_xor_assignment,

        equality,
        equality_assignment,

        lesser,
        lesser_assignment,

        lesser_equality,
        lesser_equality_assignment,

        greater,
        greater_assignment,

        greater_equality,
        greater_equality_assignment,

        prefix_increment,
        postfix_increment,

        prefix_decrement,
        postfix_decrement,

        ternary,

        call,
        index,
        dot_index,
        length,
    };

    constexpr std::string_view operation_as_string(operation const a_operation) noexcept {
        using namespace std::string_view_literals;

        constexpr std::array operation_strings{
            "null",

            "function",
            "scope",
            "conditional",
            "preconditional_loop",
            "postconditional_loop",
            "conditional_initialization_loop",

            "assignment",

            "addition",
            "addition_assignment",
            "subtraction",
            "subtraction_assignment",
            "multiplication",
            "multiplication_assignment",
            "division",
            "division_assignment",
            "exponentiation",
            "exponentiation_assignment",
            "modulo",
            "modulo_assignment",

            "bitwise_not",
            "bitwise_and",
            "bitwise_and_assignment",
            "bitwise_or",
            "bitwise_or_assignment",
            "bitwise_xor",
            "bitwise_xor_assignment",

            "logical_not",
            "logical_and",
            "logical_and_assignment",
            "logical_or",
            "logical_or_assignment",
            "logical_xor",
            "logical_xor_assignment",

            "equality",
            "equality_assignment",

            "lesser",
            "lesser_assignment",

            "lesser_equality",
            "lesser_equality_assignment",

            "greater",
            "greater_assignment",

            "greater_equality",
            "greater_equality_assignment",

            "prefix_increment",
            "postfix_increment",

            "prefix_decrement",
            "postfix_decrement",

            "ternary",

            "call",
            "index",
            "dot_index",
            "length",
        };

        return operation_strings[static_cast<size_t>(a_operation)];
    }

    /**
     * An enumeration representing the association of an operator, either
     * left or right.
     */
    enum class operator_association {
        left,
        right,
    };

    /**
     * An enumeration representing how many parameters each operator takes
     * and in which form they take them.
     */
    enum class operator_type {
        unary,
        binary,
        trinary,
        binary_enclose,
    };

    /**
     * A class linking a symbol to an operation, including association,
     * precedence, and other information.
     *
     * When type is trinary or binary_enclose, the secondary identifier symbol
     * is used to mark either the beginning of the third parameter or the end
     * of the second parameter, respectively.
     */
    struct operator_info {
        symbol               identifier;
        symbol               secondary;
        operation            mapped_operation;
        operator_type        type;
        operator_association association;
        std::size_t          precedence;
    };

    using operator_registry = std::vector<operator_info>;

    /**
     * Generate the default Rebar operator registry.
     * @return Default Rebar operator registry.
     */
    inline operator_registry default_operator_registry() {
        return {
            {
                .identifier       = symbol::equals,
                .mapped_operation = operation::assignment,
                .type             = operator_type::binary,
                .association      = operator_association::right,
                .precedence       = 1,
            },
            {
                .identifier       = symbol::question,
                .secondary        = symbol::colon,
                .mapped_operation = operation::ternary,
                .type             = operator_type::trinary,
                .association      = operator_association::right,
                .precedence       = 3,
            },
            {
                .identifier       = symbol::plus,
                .mapped_operation = operation::addition,
                .type             = operator_type::binary,
                .association      = operator_association::left,
                .precedence       = 6,
            },
            {
                .identifier       = symbol::star,
                .mapped_operation = operation::multiplication,
                .type             = operator_type::binary,
                .association      = operator_association::left,
                .precedence       = 7,
            },
            {
                .identifier       = symbol::exclamation,
                .mapped_operation = operation::logical_not,
                .type             = operator_type::unary,
                .association      = operator_association::right,
                .precedence       = 10,
            },
            {
                .identifier       = symbol::double_plus,
                .mapped_operation = operation::prefix_increment,
                .type             = operator_type::unary,
                .association      = operator_association::right,
                .precedence       = 11,
            },
            {
                .identifier       = symbol::double_plus,
                .mapped_operation = operation::postfix_increment,
                .type             = operator_type::unary,
                .association      = operator_association::left,
                .precedence       = 11,
            },
            {
                .identifier       = symbol::bracket_left,
                .secondary        = symbol::bracket_right,
                .mapped_operation = operation::index,
                .type             = operator_type::binary_enclose,
                .association      = operator_association::left,
                .precedence       = 12,
            },
            {
                .identifier       = symbol::period,
                .mapped_operation = operation::dot_index,
                .type             = operator_type::binary,
                .association      = operator_association::left,
                .precedence       = 14,
            },
        };
    }

}

#endif //OPERATORS_H
