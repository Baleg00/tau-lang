/**
 * \file
 * 
 * Library for printing detailed warning and error logs.
*/

#ifndef TAU_CRUMB_H
#define TAU_CRUMB_H

#include <stdio.h>
#include <stdbool.h>

#include "location.h"

/** Indicates the crumb log kind. */
typedef enum crumb_kind_e
{
  CRUMB_WARN, // Undesired code or behaviour.
  CRUMB_ERROR // Illegal code or behaviour.
} crumb_kind_t;

/**
 * Prints a crumb log with the specified parameters.
 * 
 * \param[in] kind Crumb log kind.
 * \param[in] loc Location in source file to be referenced.
 * \param[in] fmt Message format string.
 * \param[in] ...  Parameters displayed in the message.
*/
void crumb_log(crumb_kind_t kind, location_t* loc, const char* fmt, ...);

/** Sets the crumb file stream. */
void crumb_set_file(FILE* file);

/** Returns the crumb file stream. */
FILE* crumb_get_file(void);

/** Returns the color escape sequence of the crumb log kind. */
const char* crumb_kind_to_color(crumb_kind_t kind);

/** Logs a warning crumb message. */
#define crumb_warn(...)   crumb_log(CRUMB_WARN , __VA_ARGS__)

/** Logs an error crumb message. */
#define crumb_error(...)  crumb_log(CRUMB_ERROR, __VA_ARGS__)

#endif