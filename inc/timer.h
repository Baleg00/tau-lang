/**
 * \file
 * 
 * Timer library.
*/

#ifndef TAU_TIMER_H
#define TAU_TIMER_H

#include <stdint.h>

#include "log.h"

/**
 * \brief Returns the frequency of the timer.
 * 
 * \returns Timer frequency.
*/
uint64_t timer_freq(void);

/**
 * \brief Returns the current timer counter.
 * 
 * \returns Timer counter.
*/
uint64_t timer_now(void);

#define time_it(NAME, STMT)\
  do {\
    uint64_t time_begin_##NAME = timer_now();\
    { STMT; }\
    uint64_t time_end_##NAME = timer_now();\
    log_debug("timer", "(" #NAME ") Elapsed time: %.5lf ms", (double)(time_end_##NAME - time_begin_##NAME) / (double)timer_freq() * 1000.0);\
  } while (0);

#endif