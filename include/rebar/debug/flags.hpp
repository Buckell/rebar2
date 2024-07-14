//
// Created by maxng on 7/13/2024.
//

#ifndef FLAGS_HPP
#define FLAGS_HPP

#ifdef REBAR_DEBUG_MESSAGES_VERBOSE
#define REBAR_DEBUG_STRING_REFERENCE
#endif

namespace rebar {
#ifdef REBAR_DEBUG_STRING_REFERENCE
    constexpr bool debug_string_reference_messages = true;
#else
    constexpr bool debug_string_reference_messages = false;
#endif
}

#endif //FLAGS_HPP
