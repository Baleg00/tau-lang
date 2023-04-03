/**
 * \file
 * 
 * General utilities.
*/

#ifndef TAU_UTIL_H
#define TAU_UTIL_H

#include <stdlib.h>

#include "log.h"

#ifdef max
# undef max
#endif

#define max(A, B) ((A) > (B) ? (A) : (B))

#ifdef min
# undef min
#endif

#define min(A, B) ((A) < (B) ? (A) : (B))

#ifdef _DEBUG
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
# define debugbreak()

/** Marks a code path unreachable by control flow. */
# define unreachable()

/** Causes program termination if the condition is false. */
# define assert(COND) ((void)(COND))
#endif

#endif