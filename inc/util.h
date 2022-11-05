#ifndef TAU_UTIL_H
#define TAU_UTIL_H

#include <stdlib.h>

#include "log.h"

#define unreachable()\
  do {\
    log_fatal("unreachable", "%s:%d", __FILE__, __LINE__);\
    __debugbreak();\
    exit(EXIT_FAILURE);\
  } while (0);

#define assert(COND)\
  do {\
    if (!(COND)) {\
      log_fatal("assert", "%s:%d Assertion failed: %s", __FILE__, __LINE__, #COND);\
      __debugbreak();\
      exit(EXIT_FAILURE);\
    }\
  } while (0);

#endif