/**
 * \file
 * 
 * \brief Timer library interface.
 * 
 * \details The timer library provides functionality to measure and track the
 * elapsed time of code execution. It includes functions to retrieve the
 * frequency of the system timer and to obtain the current timestamp. This
 * allows developers to profile and optimize their code by identifying
 * performance bottlenecks and measuring the impact of optimizations.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#ifndef TAU_TIMER_H
#define TAU_TIMER_H

#include <stdint.h>

#include "utils/esc_seq.h"
#include "utils/io/log.h"

/**
 * \brief Measures the execution time of a given statement and logs the elapsed
 * time in milliseconds.
 *
 * \param[in] NAME The name for identifying the measurement.
 * \param[in] STMT The statement to be executed and timed.
 */
#define time_it(NAME, STMT)\
  do {\
    uint64_t time_it_begin = timer_now();\
    { STMT; }\
    uint64_t time_it_end = timer_now();\
    log_debug("timer", "[" ESC_FG_BRIGHT_BLACK NAME ESC_RESET "] Elapsed time: %.6g ms", (double)(time_it_end - time_it_begin) / (double)timer_freq() * 1000.0);\
  } while (0)\

/**
 * \brief Returns the frequency of the timer in ticks per second.
 * 
 * \returns The frequency of the timer.
 */
uint64_t timer_freq(void);

/**
 * \brief Returns the current value of the timer in ticks.
 * 
 * \returns The current value of the timer.
 */
uint64_t timer_now(void);

#endif