/**
 * \file
 * 
 * \brief Common utility macros.
 * 
 * \details Utility macros provide convenient and reusable code snippets in the
 * form of shorthand notations. They encapsulate complex expressions, repetitive
 * code patterns, or commonly used computations.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#ifndef TAU_COMMON_H
#define TAU_COMMON_H

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "utils/compiler_detect.h"
#include "utils/extern_c.h"
#include "utils/os_detect.h"
#include "utils/io/log.h"

#ifdef MAX
# undef MAX
#endif

/**
 * \brief Returns the maximum value between two numbers.
 * 
 * \param[in] A The first number.
 * \param[in] B The second number.
 * \returns The maximum value.
 */
#define MAX(A, B) ((A) > (B) ? (A) : (B))

#ifdef MIN
# undef MIN
#endif

/**
 * \brief Returns the minimum value between two numbers.
 * 
 * \param[in] A The first number.
 * \param[in] B The second number.
 * \returns The minimum value.
 */
#define MIN(A, B) ((A) < (B) ? (A) : (B))

#ifdef COUNTOF
# undef COUNTOF
#endif

/**
 * \brief Returns the number of elements in an array.
 * 
 * \param[in] ARRAY The array.
 * \returns The number of elements in the array.
 */
#define COUNTOF(ARRAY) (sizeof((ARRAY)) / sizeof((ARRAY)[0]))

#if defined(_MSC_VER) && !defined(__clang__)
/**
 * \brief A type whose alignment requirement is at least as strict
 * as that of every scalar type.
 *
 * \details MSVC does not conform to the C standard and does not
 * provide a definition for `max_align_t` so we make our own.
 */
typedef union
{
  int8_t i8;
  int16_t i16;
  int32_t i32;
  int64_t i64;
  intmax_t imax;
  float f;
  double d;
  long double ld;
  void* p;
} max_align_t;
#endif

#ifdef OFFSETOF
# undef OFFSETOF
#endif

#ifdef offsetof
/**
 * \brief Returns the offset of a member within a structure.
 *
 * \param[in] TYPE The type of the structure.
 * \param[in] MEMBER The member within the structure.
 * \returns The offset of the member.
 */
# define OFFSETOF(TYPE, MEMBER) offsetof(TYPE, MEMBER)
#else
/**
 * \brief Returns the offset of a member within a structure.
 * 
 * \param[in] TYPE The type of the structure.
 * \param[in] MEMBER The member within the structure.
 * \returns The offset of the member.
 */
# define OFFSETOF(TYPE, MEMBER) (size_t)(&((TYPE*)NULL)->MEMBER)
#endif

#ifdef ALIGNOF
# undef ALIGNOF
#endif

#if (defined(_MSC_VER) && defined(__alignof_is_defined)) || defined(__clang__) || defined(__GNUC__)
/**
 * \brief Returns the alignment requirement of a type.
 *
 * \param[in] TYPE The type.
 * \returns The alignment requirement of the type.
 */
# define ALIGNOF(TYPE) alignof(TYPE)
#else
/**
 * \brief Returns the alignment requirement of a type.
 * 
 * \param[in] TYPE The type.
 * \returns The alignment requirement of the type.
 */
# define ALIGNOF(TYPE) OFFSETOF(struct { char c; TYPE t; }, t)
#endif

#ifdef UNUSED
# undef UNUSED
#endif

#if defined(_MSC_VER) && !defined(__clang__)
/**
 * \brief Marks a parameter as unused to suppress warnings.
 *
 * \param[in] X The parameter's identifier.
 */
# define UNUSED(X) _Pragma("warning(suppress:4100)") unused_##X
#elif defined(__clang__) || defined(__GNUC__)
/**
 * \brief Marks a parameter as unused to suppress warnings.
 *
 * \param[in] X The parameter's identifier.
 */
# define UNUSED(X) unused_##X __attribute__((__unused__))
#else
/**
 * \brief Marks a parameter as unused to suppress warnings.
 *
 * \param[in] X The parameter's identifier.
 */
# define UNUSED(X)
#endif

#ifdef NOOP
# undef NOOP
#endif

/**
 * \brief Creates a statement which does nothing.
 */
#define NOOP() do {} while (0)

#ifdef NORETURN
# undef NORETURN
#endif

#ifdef _MSC_VER
/**
 * \brief Tells the compiler that a function cannot return.
 */
# define NORETURN __declspec(noreturn)
#elif defined(__clang__) || defined(__GNUC__)
/**
 * \brief Tells the compiler that a function cannot return.
 */
# define NORETURN __attribute__((noreturn))
#else
/**
 * \brief Tells the compiler that a function cannot return.
 */
# define NORETURN
#endif

#ifdef CLEAROBJ
# undef CLEAROBJ
#endif

/**
 * \brief Fill the memory of an object with zeros.
 * 
 * \param[in,out] OBJ Pointer to the object to be cleared.
*/
#define CLEAROBJ(OBJ) ((void)memset((OBJ), 0, sizeof(*(OBJ))))

#ifdef TAU_DEBUG
# ifdef _MSC_VER
/**
 * \brief Breaks into a debugger for debugging purposes.
 */
#   define DEBUGBREAK() __debugbreak()
# else
/**
 * \brief Breaks into a debugger for debugging purposes.
 */
#   define DEBUGBREAK() ((void)0)
# endif

/**
 * \brief Marks a code path as unreachable and triggers a fatal error.
 */
# define UNREACHABLE()\
  do {\
    log_fatal("unreachable", "%s:%d", __FILE__, __LINE__);\
    DEBUGBREAK();\
    exit(EXIT_FAILURE);\
  } while (0)

/**
 * \brief Asserts a condition and triggers a fatal error if the condition is
 * `false`.
 * 
 * \param[in] COND The condition to check.
 */
# define ASSERT(COND)\
  do {\
    if (!(COND)) {\
      log_fatal("assert", "%s:%d Assertion failed: %s", __FILE__, __LINE__, #COND);\
      DEBUGBREAK();\
      exit(EXIT_FAILURE);\
    }\
  } while (0)
#else
/**
 * \brief Breaks into a debugger for debugging purposes.
 */
# define DEBUGBREAK() ((void)0)

/**
 * \brief Marks a code path as unreachable and triggers a fatal error.
 */
# define UNREACHABLE() ((void)0)

/**
 * \brief Asserts a condition and triggers a fatal error if the condition is
 * `false`.
 * 
 * \param[in] COND The condition to check.
 */
# define ASSERT(COND) ((void)(COND))
#endif

#endif
