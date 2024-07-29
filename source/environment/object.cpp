//
// Created by maxng on 28/07/2024.
//

#include <rebar/environment/object.hpp>
#include <rebar/environment/environment.hpp>

namespace rebar {

    void reference_object(object const * a_object) noexcept {
        // If type does not require referencing, return.
        if (auto const type_integer = a_object->type_integer(); type_integer < complex_type_threshold) {
            return;
        }

        switch (a_object->m_type) {
            case type::string: {
                a_object->as_internal_string()->reference();
                break;
            }
            case type::function:
                break;
            case type::table:
                break;
            case type::array:
                break;
            case type::native:
                break;
            default:
                break;
        }
    }

    void dereference_object(object const * a_object) noexcept {
        // If type does not require dereferencing, return.
        if (auto const type_integer = a_object->type_integer(); type_integer < complex_type_threshold) {
            return;
        }

        switch (a_object->m_type) {
            case type::string: {
                a_object->as_internal_string()->dereference(*static_cast<string_engine *>(a_object->m_env));
                break;
            }
            case type::function:
                break;
            case type::table:
                break;
            case type::array:
                break;
            case type::native:
                break;
            default:
                break;
        }
    }

}