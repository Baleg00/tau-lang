/**
 * \file
 * 
 * General utilities.
*/

#ifndef TAU_UTIL_H
#define TAU_UTIL_H

#include <stdlib.h>
#include <stdint.h>

#include "log.h"

#ifdef max
# undef max
#endif

#define max(A, B) ((A) > (B) ? (A) : (B))

#ifdef min
# undef min
#endif

#define min(A, B) ((A) < (B) ? (A) : (B))

#ifdef countof
# undef countof
#endif

#define countof(ARRAY) (sizeof((ARRAY)) / sizeof((ARRAY)[0]))

#ifdef unused
# undef unused
#endif

#define unused(X) ((void)(1 ? 0 : ((void)(X), 0)))

#ifdef fallthrough
# undef fallthrough
#endif

#define fallthrough() do {} while (0)

typedef uint64_t hash_t;

static inline hash_t fnv1a_hash(const uint8_t* data, size_t size)
{
  hash_t h = 0xcbf29ce484222325ULL;

  for (size_t i = 0; i < size; ++i)
  {
    h ^= data[i];
    h *= 0x00000100000001B3ULL;
  }

  return h;
}

#define hash(DATA) (fnv1a_hash((const uint8_t*)(DATA), sizeof(*(DATA))))
#define hash_sized(DATA, SIZE) (fnv1a_hash((const uint8_t*)(DATA), (SIZE)))

#ifdef TAU_DEBUG
/** Causes a breakpoint and prompts the user to run a debugger. */
# define debugbreak() __debugbreak()

/** Marks a code path unreachable by control flow. */
# define unreachable()\
  do {\
    log_fatal("unreachable", "%s:%d", __FILE__, __LINE__);\
    debugbreak();\
    exit(EXIT_FAILURE);\
  } while (0);

/** Causes program termination if the condition is false. */
# define assert(COND)\
  if (!(COND)) {\
    log_fatal("assert", "%s:%d Assertion failed: %s", __FILE__, __LINE__, #COND);\
    debugbreak();\
    exit(EXIT_FAILURE);\
  }
#else
/** Causes a breakpoint and prompts the user to run a debugger. */
# define debugbreak() ((void)0)

/** Marks a code path unreachable by control flow. */
# define unreachable() ((void)0)

/** Causes program termination if the condition is false. */
# define assert(COND) ((void)(COND))
#endif

#endif