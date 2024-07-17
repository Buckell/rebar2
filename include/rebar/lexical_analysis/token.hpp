//
// Created by maxng on 7/13/2024.
//

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <variant>

#include <rebar/environment/types.hpp>
#include <rebar/lexical_analysis/symbol.hpp>
#include <rebar/string/string.hpp>

namespace rebar {

    /// Type of token stored in a rebar::token class.
    enum class token_type {
        null    = 0,

        identifier = 1, ///< Represents a stored rebar::string.
        integer    = 2, ///< Represents a stored rebar::integer.
        number     = 3, ///< Represents a stored rebar::number.
        string     = 4, ///< Represents a stored rebar::string.
        symbol     = 5, ///< Represents a stored rebar::symbol.
    };

    /// Internal token data type (variant).
    using token_data = std::variant<integer, number, string, symbol>;

    /**
     * Test if a type is a token data type (storable in a token).
     * @tparam t_type The type to test.
     */
    template <typename t_type>
    constexpr bool is_token_data_type_v =
        std::is_same_v<t_type, integer> ||
        std::is_same_v<t_type, number>  ||
        std::is_same_v<t_type, string>  ||
        std::is_same_v<t_type, symbol>;

    /**
     * Find the token_type enumeration value of a token data type.
     * @tparam t_type The type of which to find the token_type enumeration
     *                value.
     */
    template <typename t_type>
    constexpr token_type token_data_type_v =
        std::is_same_v<t_type, integer> ? token_type::integer :
        std::is_same_v<t_type, number>  ? token_type::number  :
        std::is_same_v<t_type, string>  ? token_type::string  :
        std::is_same_v<t_type, symbol>  ? token_type::symbol  :
        token_type::null; // This value should never occur.

    template <typename t_type>
    concept token_data_type = requires
    {
        is_token_data_type_v<t_type>;
    };

    /**
     * A class to represent a lexical token in analyzed code.
     */
    class token {
        token_type m_type;
        token_data m_data;

    public:
        /**
         * Construct a token with the provided data.
         * @tparam t_data The type of the data to store.
         * @param a_data  The data of the token to store.
         * @param a_type  The type of the data to store (automatically
         *                assumed from data type, but can be overriden.)
         */
        template <token_data_type t_data>
        explicit token(t_data const & a_data, token_type a_type = token_data_type_v<t_data>) noexcept requires(!std::is_same_v<t_data, uint64_t>);

        /**
         * Construct an integer token.
         * @tparam t_integer The type of the provided integer.
         * @param a_integer The value of the integer to store as a token.
         */
        template <std::integral t_integer>
        explicit token(t_integer a_integer) noexcept requires(!is_token_data_type_v<t_integer>);

        /**
         * Construct a number (floating point) token.
         * @tparam t_number The type of the provided number (floating point).
         * @param a_number The value of the number (floating point) to store.
         */
        template <std::floating_point t_number>
        explicit token(t_number a_number) noexcept requires(!is_token_data_type_v<t_number>);

        token(token const &) noexcept = default;
        token(token &&)      noexcept = default;

        token & operator = (token const &) noexcept = default;
        token & operator = (token &&)      noexcept = default;

        [[nodiscard]]
        inline bool operator == (token const & a_token) const noexcept;

        /**
         * Determines if the token is of the type specified.
         * @param a_type The type against which to test the token.
         * @return True if the token matches the provided type, false if not.
         */
        [[nodiscard]]
        inline bool is_type(token_type a_type) const noexcept;

        [[nodiscard]]
        inline bool is_identifier() const noexcept;

        [[nodiscard]]
        inline bool is_integer() const noexcept;

        [[nodiscard]]
        inline bool is_number() const noexcept;

        [[nodiscard]]
        inline bool is_string() const noexcept;

        [[nodiscard]]
        inline bool is_symbol() const noexcept;

        /**
         * Get the raw value of the token.
         * @tparam t_data The type of the data to get.
         * @return The stored data of the token.
         */
        template <token_data_type t_data>
        [[nodiscard]]
        t_data const & get() const noexcept;

        [[nodiscard]]
        inline integer get_integer() const noexcept;

        [[nodiscard]]
        inline number get_number() const noexcept;

        [[nodiscard]]
        inline string const & get_string() const noexcept;

        [[nodiscard]]
        inline symbol get_symbol() const noexcept;

        [[nodiscard]]
        std::string to_string() const noexcept;
    };

    // ###################################### INLINE DEFINITIONS ######################################

    template <token_data_type t_data>
    token::token(t_data const & a_data, token_type const a_type) noexcept requires(!std::is_same_v<t_data, uint64_t>) :
        m_type(a_type),
        m_data(a_data)
    {}

    template <std::integral t_integer>
    token::token(t_integer const a_integer) noexcept requires (!is_token_data_type_v<t_integer>) :
        token(static_cast<integer>(a_integer))
    {}

    template <std::floating_point t_number>
    token::token(t_number const a_number) noexcept requires (!is_token_data_type_v<t_number>) :
        token(static_cast<number>(a_number))
    {}

    bool token::operator == (token const & a_token) const noexcept {
        if (m_type != a_token.m_type) {
            return false;
        }

        switch (m_type) {
            case token_type::integer:
                return get_integer() == a_token.get_integer();
            case token_type::number:
                return get_number() == a_token.get_number();
            case token_type::string:
            case token_type::identifier:
                return get_string() == a_token.get_string();
            case token_type::symbol:
                return get_symbol() == a_token.get_symbol();
            default:
                return false;
        }
    }

    bool token::is_type(token_type const a_type) const noexcept {
        return m_type == a_type;
    }

    bool token::is_identifier() const noexcept {
        return is_type(token_type::identifier);
    }

    bool token::is_integer() const noexcept {
        return is_type(token_type::integer);
    }

    bool token::is_number() const noexcept {
        return is_type(token_type::number);
    }

    bool token::is_string() const noexcept {
        return is_type(token_type::string);
    }

    bool token::is_symbol() const noexcept {
        return is_type(token_type::symbol);
    }

    template <token_data_type t_data>
    t_data const & token::get() const noexcept {
        return std::get<t_data>(m_data);
    }

    integer token::get_integer() const noexcept {
        return get<integer>();
    }

    number token::get_number() const noexcept {
        return get<number>();
    }

    string const & token::get_string() const noexcept {
        return get<string>();
    }

    symbol token::get_symbol() const noexcept {
        return get<symbol>();
    }

}

#endif //TOKEN_HPP
