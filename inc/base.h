/**
 * \file
 * 
 * \brief Project base header.
 * 
 * \details This is a "force-included" header file that is included at the
 * beginning of every source file in the project, ensuring that its contents are
 * available globally throughout the codebase. This file is used for defining
 * essential project-wide macros, constants, or configurations, or for including
 * headers that provide crucial dependencies or system-level declarations.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_BASE_H
#define TAU_BASE_H

#if defined(_MSC_VER) && !defined(__clang__)
// enumerator in switch of enum is not explicitly handled by a case label
# pragma warning(disable: 4061)

// unnamed type definition in parentheses
# pragma warning(disable: 4116)

// padding added after data member
# pragma warning(disable: 4820)

// unreachable code
# pragma warning(disable: 4702)

// Spectre mitigation
# pragma warning(disable: 5045)

// preprocessor macro is not defined
# pragma warning(disable: 4668)

// function not inlined
# pragma warning(disable: 4710)

// function selected for inline expansion
# pragma warning(disable: 4711)

// structure was padded due to alignment specifier
# pragma warning(disable: 4324)

// declaration hides previous local declaration
# pragma warning(disable: 4456)

// unsafe function pointer conversion
# pragma warning(disable: 4191)
#elif defined(__GNUC__)
// enumerator in switch of enum is not explicitly handled by a case label
# pragma GCC diagnostic ignored "-Wswitch-enum"

// padding added after data member
# pragma GCC diagnostic ignored "-Wpadded"

// declaration hides previous local declaration
# pragma GCC diagnostic ignored "-Wshadow=local"

// switch case implicit fallthrough
# pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

// conversion between pointers that have incompatible types
# pragma GCC diagnostic ignored "-Wincompatible-pointer-types"

// function is unused
# pragma GCC diagnostic ignored "-Wunused-function"
#elif defined(__clang__)
// enumerator in switch of enum is not explicitly handled by a case label
# pragma clang diagnostic ignored "-Wswitch-enum"

// padding added after data member
# pragma clang diagnostic ignored "-Wpadded"

// declaration hides previous local declaration
# pragma clang diagnostic ignored "-Wshadow-uncaptured-local"

// extension used
# pragma clang diagnostic ignored "-Wlanguage-extension-token"

// incompatible function pointer types
# pragma clang diagnostic ignored "-Wincompatible-function-pointer-types"

// unused function
# pragma clang diagnostic ignored "-Wunused-function"
#endif

#endif
