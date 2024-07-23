//
// Created by maxng on 23/07/2024.
//

#ifndef OPERATION_TREE_H
#define OPERATION_TREE_H

#include <memory>
#include <variant>
#include <vector>

#include <rebar/lexical_analysis/token.hpp>
#include <rebar/semantic_analysis/operators.hpp>

namespace rebar {

    class operation_tree;

    /**
     * A node within an operation tree. Contains either another operation
     * tree or a token representing an immediate value.
     */
    using operation_tree_node = std::variant<std::nullptr_t, token, std::unique_ptr<operation_tree>>;

    enum class operation_tree_node_type {
        null  = 0,
        token = 1,
        tree  = 2,
    };

    /**
     * A class that defines an operation or construct with a list of
     * operands. Contains a principal operation and an array of nodes, with
     * each node either being another operation tree or a token (which
     * represents a literal value or identifier).
     */
    class operation_tree {
        operation m_operation;
        std::vector<operation_tree_node> m_operands;

    public:
        inline explicit operation_tree(operation a_operation) noexcept;

        operation_tree(operation_tree const &) noexcept = default;
        operation_tree(operation_tree &&)      noexcept = default;

        operation_tree & operator = (operation_tree const &) noexcept = default;
        operation_tree & operator = (operation_tree &&)      noexcept = default;

        /**
         * Retrieve the tree's operation.
         * @return The operation of the tree.
         */
        [[nodiscard]]
        inline operation op() const noexcept;

        /**
         * Set the tree's operation.
         * @param a_operation The operation to set as the tree's operation.
         */
        inline void set_op(operation a_operation) noexcept;

        /**
         * Set an operand at a specified index.
         * @param a_index The index of the operand to set.
         * @param a_node The value of the operand to set.
         */
        void set_operand(std::size_t a_index, operation_tree_node a_node) noexcept;

        /**
         * Retrieve the type of an operand.
         * @param a_index The index of the operand of which to retrieve the
         *                type.
         * @return The type of the operand at the specified index.
         */
        [[nodiscard]]
        inline operation_tree_node_type operand_type(std::size_t a_index) const noexcept;

        /**
         * Retrieve an operand as a token.
         * @param a_index The index of the token operand to retrieve.
         * @return The token at the specified index.
         */
        [[nodiscard]]
        inline token & token_operand(std::size_t a_index);

        /**
         * Retrieve an operand as a token.
         * @param a_index The index of the token operand to retrieve.
         * @return The token at the specified index.
         */
        [[nodiscard]]
        inline token const & token_operand(std::size_t a_index) const;

        /**
         * Retrieve an operand as an operation tree.
         * @param a_index The index of the tree operand to retrieve.
         * @return The operation tree at the specified index.
         */
        [[nodiscard]]
        inline operation_tree & tree_operand(std::size_t a_index);

        /**
         * Retrieve an operand as an operation tree.
         * @param a_index The index of the tree operand to retrieve.
         * @return The operation tree at the specified index.
         */
        [[nodiscard]]
        inline operation_tree const & tree_operand(std::size_t a_index) const;

    };

    // ###################################### INLINE DEFINITIONS ######################################

    operation_tree::operation_tree(operation const a_operation) noexcept :
        m_operation(a_operation)
    {}

    operation operation_tree::op() const noexcept {
        return m_operation;
    }

    void operation_tree::set_op(operation const a_operation) noexcept {
        m_operation = a_operation;
    }

    inline operation_tree_node_type operation_tree::operand_type(std::size_t const a_index) const noexcept {
        if (a_index >= m_operands.size()) {
            return operation_tree_node_type::null;
        }

        return static_cast<operation_tree_node_type>(m_operands[a_index].index());
    }

    token & operation_tree::token_operand(std::size_t const a_index) {
        return std::get<token>(m_operands.at(a_index));
    }

    token const & operation_tree::token_operand(std::size_t const a_index) const {
        return std::get<token>(m_operands.at(a_index));
    }

    operation_tree & operation_tree::tree_operand(std::size_t const a_index) {
        return *std::get<std::unique_ptr<operation_tree>>(m_operands.at(a_index));
    }

    operation_tree const & operation_tree::tree_operand(std::size_t const a_index) const {
        return *std::get<std::unique_ptr<operation_tree>>(m_operands.at(a_index));
    }

}

#endif //OPERATION_TREE_H
