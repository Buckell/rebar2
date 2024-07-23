//
// Created by maxng on 23/07/2024.
//

#include <rebar/semantic_analysis/operation_tree.hpp>

namespace rebar {

    void operation_tree::set_operand(std::size_t const a_index, operation_tree_node a_node) noexcept {
        // Check for parameter vector bounds and expand if needed.
        if (a_index >= m_operands.size()) {
            m_operands.resize(a_index + 1);
        }

        m_operands.emplace(
            m_operands.cbegin() + static_cast<decltype(m_operands)::difference_type>(a_index),
            std::move(a_node)
        );
    }

}