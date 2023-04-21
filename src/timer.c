#include "timer.h"

#ifdef _WIN32

#include <windows.h>
#include <profileapi.h>

uint64_t timer_freq(void)
{
  LARGE_INTEGER f;
  QueryPerformanceFrequency(&f);
  return (uint64_t)f.QuadPart;
}

uint64_t timer_now(void)
{
  LARGE_INTEGER t;
  QueryPerformanceCounter(&t);
  return (uint64_t)t.QuadPart;
}

#else

#include <time.h>

uint64_t timer_freq(void)
{
  return (uint64_t)CLOCKS_PER_SEC;
}

uint64_t timer_now(void)
{
  return (uint64_t)clock();
}

#endif
