//
// Created by maxng on 23/07/2024.
//

#ifndef SEMANTIC_UNIT_H
#define SEMANTIC_UNIT_H

#include <rebar/semantic_analysis/operation_tree.hpp>

namespace rebar {

    class semantic_unit {
        operation_tree m_base_scope;

    public:
        semantic_unit() noexcept = default;

        semantic_unit(semantic_unit const &) noexcept = default;
        semantic_unit(semantic_unit &&)      noexcept = default;

        semantic_unit & operator = (semantic_unit const &) noexcept = default;
        semantic_unit & operator = (semantic_unit &&)      noexcept = default;

        inline void apply_base_scope(operation_tree a_operation_tree) noexcept;

        inline operation_tree & base_scope() noexcept;

        [[nodiscard]]
        inline operation_tree const & base_scope() const noexcept;
    };

    // ###################################### INLINE DEFINITIONS ######################################

    void semantic_unit::apply_base_scope(operation_tree a_operation_tree) noexcept {
        m_base_scope = std::move(a_operation_tree);
    }

    operation_tree & semantic_unit::base_scope() noexcept {
        return m_base_scope;
    }

    operation_tree const & semantic_unit::base_scope() const noexcept {
        return m_base_scope;
    }

}

#endif //SEMANTIC_UNIT_H
