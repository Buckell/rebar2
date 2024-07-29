//
// Created by maxng on 27/07/2024.
//

#ifndef EQUAL_TO_HPP
#define EQUAL_TO_HPP

namespace rebar {

    /// Predicate for being equal to another value regardless of type.
    template <typename t_contained>
    struct equal_to {
        t_contained const contained;

        explicit equal_to(t_contained a_contained) noexcept :
            contained(std::move(a_contained))
        {}

        [[nodiscard]]
        bool operator () (auto const & a_rhs) const noexcept {
            return a_rhs == contained;
        }
    };

    /// Predicate for being equal to another value (reference) regardless of type.
    template <typename t_contained, typename t_compare>
    struct equal_to_reference {
        t_contained const & contained_reference;

        explicit equal_to_reference(t_contained const & a_contained) noexcept :
            contained_reference(a_contained)
        {}

        [[nodiscard]]
        bool operator () (t_compare const & a_rhs) const noexcept {
            return a_rhs == contained_reference;
        }
    };

    template <typename t_contained>
    struct not_equal_to {
        t_contained const contained;

        explicit not_equal_to(t_contained a_contained) noexcept :
            contained(std::move(a_contained))
        {}

        [[nodiscard]]
        bool operator () (auto const & a_rhs) const noexcept {
            return a_rhs != contained;
        }
    };

    template <typename t_contained, typename t_compare>
    struct not_equal_to_reference {
        t_contained const & contained_reference;

        explicit not_equal_to_reference(t_contained const & a_contained) noexcept :
            contained_reference(a_contained)
        {}

        [[nodiscard]]
        bool operator () (t_compare const & a_rhs) const noexcept {
            return a_rhs != contained_reference;
        }
    };

}

#endif //EQUAL_TO_HPP
