/**
 * \file
 * 
 * Logging library.
*/

#ifndef TAU_LOG_H
#define TAU_LOG_H

#include <stdio.h>
#include <stdbool.h>

#include "typedefs.h"

/**
 * Prints a log message with the specified parameters.
 * 
 * \param[in] lvl Log level.
 * \param[in] file Path to source file in which the call was made.
 * \param[in] line Line number on which the call was made.
 * \param[in] func Name of function in which the call was made.
 * \param[in] name Name of the logger.
 * \param[in] fmt Message format string.
 * \param[in] ... Parameters displayed in the message.
*/
void log_log(log_level_t lvl, const char* file, int line, const char* func, const char* name, const char* fmt, ...);

/** Returns the log level name as a string. */
const char* log_level_to_string(log_level_t lvl);

/** Returns the color escape sequence of the log level. */
const char* log_level_to_color(log_level_t lvl);

/** Sets the minimum log level. */
void log_set_level(log_level_t lvl);

/** Returns the minimum log level. */
log_level_t log_get_level(void);

/** Sets the log file stream. */
void log_set_file(FILE* file);

/** Returns the log file stream. */
FILE* log_get_file(void);

/** Enables or disables verbose logging. */
void log_set_verbose(bool value);

/** Returns true if verbose logging is enabled, false otherwise. */
bool log_get_verbose(void);

/** Logs a trace level message. */
#define log_trace(...)  log_log(LOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)

/** Logs a debug level message. */
#define log_debug(...)  log_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

/** Logs an info level message. */
#define log_info(...)   log_log(LOG_LEVEL_INFO , __FILE__, __LINE__, __func__, __VA_ARGS__)

/** Logs a warning level message. */
#define log_warn(...)   log_log(LOG_LEVEL_WARN , __FILE__, __LINE__, __func__, __VA_ARGS__)

/** Logs an error level message. */
#define log_error(...)  log_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

/** Logs a fatal level message. */
#define log_fatal(...)  log_log(LOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif