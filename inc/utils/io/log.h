/**
 * \file log.h
 * 
 * \brief Logging library interface.
 * 
 * \details The logging library is a utility that allows applications to record
 * and store information about their runtime behavior. It provides a flexible
 * and configurable way to capture events, messages, and errors occurring
 * during program execution. It supports different log levels, such as debug,
 * info, warning, and error, allowing developers to control the type and
 * verbosity of the logged information. These logs can be written to files or
 * the console. The logging library helps in troubleshooting, performance
 * analysis, and understanding the flow of the application by providing a
 * centralized and organized view of its runtime activities.
 * 
 * \copyright Copyright (c) Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#ifndef TAU_LOG_H
#define TAU_LOG_H

#include <stdio.h>
#include <stdbool.h>

/**
 * \brief Logs a message with the TRACE log level.
 *
 * \param[in] ... The message to log, formatted as a printf-style format string.
 */
#define log_trace(...) log_log(LOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * \brief Logs a message with the DEBUG log level.
 *
 * \param[in] ... The message to log, formatted as a printf-style format string.
 */
#define log_debug(...) log_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * \brief Logs a message with the INFO log level.
 *
 * \param[in] ... The message to log, formatted as a printf-style format string.
 */
#define log_info(...)  log_log(LOG_LEVEL_INFO , __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * \brief Logs a message with the WARN log level.
 *
 * \param[in] ... The message to log, formatted as a printf-style format string.
 */
#define log_warn(...)  log_log(LOG_LEVEL_WARN , __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * \brief Logs a message with the ERROR log level.
 *
 * \param[in] ... The message to log, formatted as a printf-style format string.
 */
#define log_error(...) log_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * \brief Logs a message with the FATAL log level.
 *
 * \param[in] ... The message to log, formatted as a printf-style format string.
 */
#define log_fatal(...) log_log(LOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * \brief Enumeration of log levels.
 */
typedef enum log_level_e
{
  LOG_LEVEL_TRACE, // Low-level, highly detailed behaviour.
  LOG_LEVEL_DEBUG, // Diagnostic messages.
  LOG_LEVEL_INFO,  // Normal behaviour, informative messages.
  LOG_LEVEL_WARN,  // Abnormal or unexpected behaviour.
  LOG_LEVEL_ERROR, // Severe issue or failure.
  LOG_LEVEL_FATAL, // Critical failure and program termination.
} log_level_t;

/**
 * \brief Logs a message with the specified log level.
 *
 * \param[in] lvl The log level of the message.
 * \param[in] file The source file where the log message originates.
 * \param[in] line The line number in the source file where the log message originates.
 * \param[in] func The function name where the log message originates.
 * \param[in] name The name associated with the log message.
 * \param[in] fmt The format string of the log message.
 * \param[in] ... Additional arguments for the format string.
 */
void log_log(log_level_t lvl, const char* file, int line, const char* func, const char* name, const char* fmt, ...);

/**
 * \brief Converts a log level to its corresponding string representation.
 *
 * \param[in] lvl The log level to convert.
 * \returns Pointer to the string representation of the log level.
 */
const char* log_level_to_string(log_level_t lvl);

/**
 * \brief Converts a log level to its corresponding color representation.
 *
 * \param[in] lvl The log level to convert.
 * \returns Pointer to the color representation of the log level.
 */
const char* log_level_to_color(log_level_t lvl);

/**
 * \brief Sets the log level for filtering log messages.
 *
 * \param[in] lvl The log level to set.
 */
void log_set_level(log_level_t lvl);

/**
 * \brief Retrieves the current log level.
 *
 * \returns The current log level.
 */
log_level_t log_get_level(void);

/**
 * \brief Sets the output stream for logging messages.
 *
 * \param[in] stream The output stream to set.
 */
void log_set_stream(FILE* stream);

/**
 * \brief Retrieves the current output stream for logging messages.
 *
 * \returns The current output stream.
 */
FILE* log_get_stream(void);

/**
 * \brief Sets the verbosity mode for logging messages.
 *
 * \param[in] value The verbosity mode to set.
 */
void log_set_verbose(bool value);

/**
 * \brief Retrieves the current verbosity mode for logging messages.
 *
 * \returns The current verbosity mode.
 */
bool log_get_verbose(void);

#endif