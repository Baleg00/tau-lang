/**
 * \file timer.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#include "utils/timer.h"

#ifdef _WIN32
# include <windows.h>
#else
# include <time.h>
#endif

uint64_t timer_freq(void)
{
#ifdef _WIN32
  LARGE_INTEGER f;
  QueryPerformanceFrequency(&f);
  return (uint64_t)f.QuadPart;
#elif defined(__unix) || defined(__unix__) || defined(__gnu_linux__) || defined(__linux__)
  struct timespec ts;
  clock_getres(CLOCK_MONOTONIC, &ts);
  return (uint64_t)(ts.tv_sec * 1000000ull + ts.tv_nsec);
#else
  return (uint64_t)CLOCKS_PER_SEC;
#endif
}

uint64_t timer_now(void)
{
#ifdef _WIN32
  LARGE_INTEGER t;
  QueryPerformanceCounter(&t);
  return (uint64_t)t.QuadPart;
#elif defined(__unix) || defined(__unix__) || defined(__gnu_linux__) || defined(__linux__)
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)(ts.tv_sec * 1000000ull + ts.tv_nsec);
#else
  return (uint64_t)clock();
#endif
}
