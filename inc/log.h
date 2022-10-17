#ifndef TAU_LOG_H
#define TAU_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

typedef enum log_level_e
{
  LOG_LEVEL_TRACE,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_FATAL,
} log_level_t;

void log_log(log_level_t lvl, const char* file, int line, const char* fmt, ...);

const char* log_level_to_string(log_level_t lvl);
const char* log_level_to_color(log_level_t lvl);

void log_set_level(log_level_t lvl);
log_level_t log_get_level(void);

void log_set_file(FILE* file);
FILE* log_get_file(void);

void log_set_verbose(bool value);
bool log_get_verbose(void);

#define log_trace(...)  log_log(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...)  log_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)   log_log(LOG_LEVEL_INFO , __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)   log_log(LOG_LEVEL_WARN , __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...)  log_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...)  log_log(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#endif