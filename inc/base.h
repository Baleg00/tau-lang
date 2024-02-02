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

#ifdef _MSC_VER
// enumerator in switch of enum is not explicitly handled by a case label
# pragma warning(disable: 4061)

// unreferenced formal parameter
# pragma warning(disable: 4100)

// unnamed type definition in parentheses
# pragma warning(disable: 4116)

// padding added after data member
# pragma warning(disable: 4820)

// unreachable code
# pragma warning(disable: 4702)

// unreachable code
# pragma warning(disable: 5045)

// preprocessor macro is not defined
# pragma warning(disable: 4668)
#endif

#endif