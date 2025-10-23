/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#include "utils/timer.h"

#include "utils/os_detect.h"

#if TAU_OS_WINDOWS
# include <windows.h>
#else
# include <time.h>
#endif

uint64_t tau_timer_freq(void)
{
#if TAU_OS_WINDOWS
  LARGE_INTEGER f;
  QueryPerformanceFrequency(&f);
  return (uint64_t)f.QuadPart;
#elif TAU_OS_UNIX
  struct timespec ts;
  clock_getres(CLOCK_MONOTONIC, &ts);
  return (uint64_t)(ts.tv_sec * 1000000ull + ts.tv_nsec);
#else
  return (uint64_t)CLOCKS_PER_SEC;
#endif
}

uint64_t tau_timer_now(void)
{
#if TAU_OS_WINDOWS
  LARGE_INTEGER t;
  QueryPerformanceCounter(&t);
  return (uint64_t)t.QuadPart;
#elif TAU_OS_UNIX
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)(ts.tv_sec * 1000000ull + ts.tv_nsec);
#else
  return (uint64_t)clock();
#endif
}
