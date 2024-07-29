//
// Created by maxng on 28/07/2024.
//

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <rebar/environment/types.hpp>
#include <rebar/string/string.hpp>

namespace rebar {

    class environment;
    class object;

    using object_data = std::uint64_t;

    void reference_object(object const * a_object) noexcept;
    void dereference_object(object const * a_object) noexcept;

    class object {
        /// If object is of type string, this value is of type string_engine*,
        /// otherwise, it is either nullptr or environment*.
        void mutable * m_env;

        type m_type;
        object_data m_data;

    public:
        inline object() noexcept;

        inline explicit object(boolean a_boolean) noexcept;

        inline explicit object(integer a_integer) noexcept;

        template <std::integral t_integer>
        explicit object(t_integer a_integer) noexcept;

        inline explicit object(number a_number) noexcept;

        inline explicit object(string const & a_string) noexcept;
        inline explicit object(string && a_string) noexcept;

        inline ~object() noexcept;

        inline object(object const & a_object) noexcept;
        inline object(object && a_object)      noexcept;

        inline object & operator = (boolean a_boolean) noexcept;

        inline object & operator = (integer a_integer) noexcept;

        template <std::integral t_integer>
        object & operator = (t_integer a_integer) noexcept;

        inline object & operator = (number a_number) noexcept;

        inline object & operator = (string const & a_string) noexcept;
        inline object & operator = (string && a_string) noexcept;

        inline object & operator = (object const & a_object) noexcept;
        inline object & operator = (object && a_object) noexcept;

        [[nodiscard]]
        inline type object_type() const noexcept;

        [[nodiscard]]
        inline bool is_type(type a_type) const noexcept;

        [[nodiscard]]
        inline bool is_null() const noexcept;

        [[nodiscard]]
        inline bool is_integer() const noexcept;

        [[nodiscard]]
        inline bool is_number() const noexcept;

        [[nodiscard]]
        inline bool is_string() const noexcept;

        [[nodiscard]]
        inline bool is_function() const noexcept;

        [[nodiscard]]
        inline bool is_table() const noexcept;

        [[nodiscard]]
        inline bool is_array() const noexcept;

        [[nodiscard]]
        inline bool is_native() const noexcept;

    private:
        [[nodiscard]]
        inline std::size_t type_integer() const noexcept;

        [[nodiscard]]
        inline internal_string * as_internal_string() const noexcept;

        friend void rebar::reference_object(object const * a_object) noexcept;
        friend void rebar::dereference_object(object const * a_object) noexcept;
    };

    // ###################################### INLINE DEFINITIONS ######################################

    object::object() noexcept :
        m_env(nullptr),
        m_type(type::null),
        m_data(0)
    {}

    object::object(boolean const a_boolean) noexcept :
        m_env(nullptr),
        m_type(type::boolean),
        m_data(std::bit_cast<object_data>(a_boolean))
    {}

    object::object(integer const a_integer) noexcept :
        m_env(nullptr),
        m_type(type::integer),
        m_data(std::bit_cast<object_data>(a_integer))
    {}

    template <std::integral t_integer>
    object::object(t_integer a_integer) noexcept :
        m_env(nullptr),
        m_type(type::integer),
        m_data(std::bit_cast<object_data>(static_cast<integer>(a_integer)))
    {}

    object::object(number const a_number) noexcept :
        m_env(nullptr),
        m_type(type::number),
        m_data(std::bit_cast<object_data>(a_number))
    {}

    object::object(string const & a_string) noexcept :
        m_env(a_string.m_engine),
        m_type(type::string),
        m_data(std::bit_cast<object_data>(a_string.m_container))
    {
        as_internal_string()->reference();
    }

    object::object(string && a_string) noexcept :
        m_env(a_string.m_engine),
        m_type(type::string),
        m_data(std::bit_cast<object_data>(a_string.m_container))
    {
        a_string.m_container = nullptr;
    }

    object::~object() noexcept {
        dereference_object(this);
    }

    object::object(object const & a_object) noexcept :
        m_env(a_object.m_env),
        m_type(a_object.m_type),
        m_data(a_object.m_data)
    {
        reference_object(this);
    }

    object::object(object && a_object) noexcept :
        m_env(a_object.m_env),
        m_type(a_object.m_type),
        m_data(a_object.m_data)
    {
        // If transferred object is complex, make old object null.
        if (type_integer() >= complex_type_threshold) {
            a_object.m_type = type::null;
            a_object.m_data = 0;
        }
    }

    object & object::operator = (boolean const a_boolean) noexcept {
        m_type = type::boolean;
        m_data = std::bit_cast<object_data>(a_boolean);

        return *this;
    }

    object & object::operator = (integer const a_integer) noexcept {
        m_type = type::integer;
        m_data = std::bit_cast<object_data>(a_integer);

        return *this;
    }

    template <std::integral t_integer>
    object & object::operator = (t_integer a_integer) noexcept {
        m_type = type::integer;
        m_data = std::bit_cast<object_data>(static_cast<integer>(a_integer));

        return *this;
    }

    object & object::operator = (number const a_number) noexcept {
        m_type = type::number;
        m_data = std::bit_cast<object_data>(a_number);

        return *this;
    }

    object & object::operator = (string const & a_string) noexcept {
        m_env = a_string.m_engine;
        m_type = type::string;
        m_data = std::bit_cast<object_data>(a_string.m_container);

        return *this;
    }

    object & object::operator = (string && a_string) noexcept {
        m_env = a_string.m_engine;
        m_type = type::string;
        m_data = std::bit_cast<object_data>(a_string.m_container);

        a_string.m_container = nullptr;

        return *this;
    }

    object & object::operator = (object const & a_object) noexcept {
        if (this == &a_object) {
            return *this;
        }

        m_env = a_object.m_env;
        m_type = a_object.m_type;
        m_data = a_object.m_data;

        reference_object(this);

        return *this;
    }

    object & object::operator = (object && a_object) noexcept {
        m_env = a_object.m_env;
        m_type = a_object.m_type;
        m_data = a_object.m_data;

        // If transferred object is complex, make old object null.
        if (type_integer() >= complex_type_threshold) {
            a_object.m_type = type::null;
            a_object.m_data = 0;
        }

        return *this;
    }

    type object::object_type() const noexcept {
        return m_type;
    }

    bool object::is_type(type const a_type) const noexcept {
        return m_type == a_type;
    }

    bool object::is_null() const noexcept {
        return is_type(type::null);
    }

    bool object::is_integer() const noexcept {
        return is_type(type::integer);
    }

    bool object::is_number() const noexcept {
        return is_type(type::number);
    }

    bool object::is_string() const noexcept {
        return is_type(type::string);
    }

    bool object::is_function() const noexcept {
        return is_type(type::function);
    }

    bool object::is_table() const noexcept {
        return is_type(type::table);
    }

    bool object::is_array() const noexcept {
        return is_type(type::array);
    }

    bool object::is_native() const noexcept {
        return is_type(type::native);
    }

    std::size_t object::type_integer() const noexcept {
        return static_cast<std::size_t>(m_type);
    }

    internal_string * object::as_internal_string() const noexcept {
        return std::bit_cast<internal_string *>(m_data);
    }

}

#endif //OBJECT_HPP
