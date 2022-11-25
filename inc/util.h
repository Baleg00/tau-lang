#ifndef TAU_UTIL_H
#define TAU_UTIL_H

#include <stdlib.h>

#include "log.h"

#ifdef _DEBUG
#define debugbreak() __debugbreak()
#else
#define debugbreak()
#endif

#define unreachable()\
  do {\
    log_fatal("unreachable", "%s:%d", __FILE__, __LINE__);\
    debugbreak();\
    exit(EXIT_FAILURE);\
  } while (0);

#define assert(COND)\
  if (!(COND)) {\
    log_fatal("assert", "%s:%d Assertion failed: %s", __FILE__, __LINE__, #COND);\
    debugbreak();\
    exit(EXIT_FAILURE);\
  }

#endif