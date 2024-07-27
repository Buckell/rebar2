//
// Created by maxng on 23/07/2024.
//

#include <algorithm>
#include <ranges>
#include <rebar/semantic_analysis/operation_tree.hpp>

namespace rebar {

    void operation_tree::set_operand(std::size_t const a_index, operation_tree_node a_node) noexcept {
        // Check for parameter vector bounds and expand if needed.
        if (a_index >= m_operands.size()) {
            m_operands.resize(a_index);
        }

        m_operands.emplace(
            m_operands.cbegin() + static_cast<decltype(m_operands)::difference_type>(a_index),
            std::move(a_node)
        );
    }

    std::string operation_tree::to_string() const noexcept {
        auto const node_to_string = [](operation_tree_node const & a_node) -> std::string {
            switch (auto const node_type = static_cast<operation_tree_node_type>(a_node.index())) {
                case operation_tree_node_type::token:
                    return std::get<token>(a_node).to_string();
                case operation_tree_node_type::tree:
                    return std::get<std::unique_ptr<operation_tree>>(a_node)->to_string();
                default:
                    return "NULL";
            }
        };

        std::string list;
        std::ranges::for_each(m_operands | std::views::transform(node_to_string), [&list](auto const & s) {
            list += s + ", ";
        });

        return fmt::format(
            "{} {{ {}}}",
            operation_as_string(m_operation),
            list
        );
    }

    std::string operation_tree::to_string_pretty(std::size_t const a_indent) const noexcept {
        std::string const indent_string(a_indent, '\t');

        auto const node_to_string = [&indent_string, &a_indent](operation_tree_node const & a_node) {
            switch (auto const node_type = static_cast<operation_tree_node_type>(a_node.index())) {
                case operation_tree_node_type::token:
                    return indent_string + '\t' + std::get<token>(a_node).to_string();
                case operation_tree_node_type::tree:
                    return std::get<std::unique_ptr<operation_tree>>(a_node)->to_string_pretty(a_indent + 1);
                default:
                    return indent_string + "\tNULL";
            }
        };

        std::string list;
        std::ranges::for_each(std::views::transform(m_operands, node_to_string), [&list](auto const & s) {
            list += '\n';
            list += s + ",";
        });

        return fmt::format(
            "{}{} {{{}\n{}}}",
            indent_string,
            operation_as_string(m_operation),
            list,
            indent_string
        );
    }

}