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

        bitwise_and,
        bitwise_and_assignment,
        bitwise_or,
        bitwise_or_assignment,
        bitwise_xor,
        bitwise_xor_assignment,

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

        call,
        index,
        length,
    };

    /**
     * An enumeration representing the association of an operator, either
     * left or right.
     */
    enum class association {
        left,
        right,
    };

    /**
     * A class linking a symbol to an operation, including association,
     * precedence, and other information.
     */
    struct operation_info {
        symbol base_symbol;
        operation mapped_operation;
        association operator_association;
        std::size_t precedence;
    };

}

#endif //OPERATORS_H
