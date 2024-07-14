//
// Created by maxng on 7/13/2024.
//

#ifndef TYPES_HPP
#define TYPES_HPP

namespace rebar {

    /**
     * An enumeration of Rebar object types.
     *
     * Simple Type: A type that requires no explicit garbage collection. Once
     * the object is no longer in use, it can simply be forgotten.
     *
     * Complex Type: A type that requires additional garbage collection. It
     * must be explicitly referenced and dereferenced. When the reference count
     * reaches zero, explicit garbage collection will occur.
     *
     * Simply Comparable: A type that can be compared (equality) by testing raw
     * equality of the data fields of the Rebar objects. These types require
     * no additional actions to determine equality.
     *
     * Complexly Comparable: A type that requires additional computation to
     * determine equality past comparing object data fields.
     */
    enum class type {
        /*
         * Order of types is very important. Integer thresholds determine
         * cleanup actions and garbage collection. Type order assists with
         * code optimization.
         */

        // Simple Type, Simply Comparable:
        null    = 0, ///< Null type: No contained data.
        boolean = 1, ///< Boolean type: Binary true/false value.
        integer = 2, ///< Integer type: Signed integer.
        number  = 3, ///< Number type: Floating point value.

        // Complex Type, Simply Comparable:
        string  = 4, ///< String type: Represents a string of characters.

        // Complex Type, Complexly Comparable:
        table   = 5, /**< Table type: Represents a key/value map of objects to
                      *               objects.
                      */
        array   = 6, ///< Array type: Represents an array of objects.
    };

    /**
     * Type integer threshold for complex types. All types with an integer
     * equal to or greater than this value are complex types.
     */
    constexpr std::uint64_t complex_type_threshold = static_cast<std::uint64_t>(type::string);

    /**
     * Type integer threshold for complexly comparable types. All types with
     * an integer equal to or greater than this value are complexly comparable.
     */
    constexpr std::uint64_t complexly_comparable_threshold = static_cast<std::uint64_t>(type::table);

    /**
     * Rebar integer type.
     * Signed 64-bit integer.
     */
    using integer = std::int64_t;

    // Make sure double is eight bytes wide.
    static_assert(sizeof(double) == 8);

    /**
     * Rebar number type.
     * 64-bit double-precision floating point.
     */
    using number = double;

    /**
     * Rebar boolean type.
     * 64-bit unsigned integer (bool).
     */
    using boolean = std::uint64_t;

    /// Rebar boolean true value.
    constexpr boolean _false = 0;

    /// Rebar boolean false value.
    constexpr boolean _true  = 1;

}

#endif //TYPES_HPP
