/**
 * \file
 * 
 * Library for printing detailed warning and error logs.
*/

#ifndef TAU_CRUMB_H
#define TAU_CRUMB_H

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

#include "location.h"

/** Indicates the crumb log kind. */
typedef enum crumb_kind_e
{
  CRUMB_WARN, // Undesired code or behaviour.
  CRUMB_ERROR // Illegal code or behaviour.
} crumb_kind_t;

/** Indicates the kind of a crumb log item. */
typedef enum crumb_item_kind_e
{
  CRUMB_ITEM_MESSAGE, // Plain text.
  CRUMB_ITEM_SNIPPET, // Source code snippet.
} crumb_item_kind_t;

/** Represents a crumb log message. */
typedef struct crumb_item_message_s
{
  const char* msg; // Message format string.
  va_list msg_args; // Message arguments if any.
} crumb_item_message_t;

/** Represents a crumb log snippet. */
typedef struct crumb_item_snippet_s
{
  location_t* loc; // Source code location.
  const char* title; // Title format string.
  va_list title_args; // Title arguments if any.
  const char* msg; // Message format string.
  va_list msg_args; // Message arguments if any.
} crumb_item_snippet_t;

/** Type large enough to hold any item kind. */
typedef struct crumb_item_s
{
  crumb_item_kind_t kind;

  union
  {
    crumb_item_message_t msg;
    crumb_item_snippet_t snip;
  };
} crumb_item_t;

#define crumb_message(MSG, MSG_ARGS) ((crumb_item_t){ .kind = CRUMB_ITEM_MESSAGE, .msg = (crumb_item_message_t){ .msg = (MSG), .msg_args = (MSG_ARGS) }})
#define crumb_snippet(LOC, TITLE, TITLE_ARGS, MSG, MSG_ARGS) ((crumb_item_t){ .kind = CRUMB_ITEM_SNIPPET, .snip = (crumb_item_snippet_t){ .loc = (LOC), .title = (TITLE), .title_args = (TITLE_ARGS), .msg = (MSG), .msg_args = (MSG_ARGS) }})

void crumb_log(crumb_kind_t kind, size_t count, ...);

/** Sets the crumb file stream. */
void crumb_set_file(FILE* file);

/** Returns the crumb file stream. */
FILE* crumb_get_file(void);

/** Returns the color escape sequence of the crumb log kind. */
const char* crumb_kind_to_color(crumb_kind_t kind);

/** Returns the string representation of the crumb log kind. */
const char* crumb_kind_to_string(crumb_kind_t kind);

/** Logs a warning crumb message. */
#define crumb_warn(...)   crumb_log(CRUMB_WARN , __VA_ARGS__)

/** Logs an error crumb message. */
#define crumb_error(...)  crumb_log(CRUMB_ERROR, __VA_ARGS__)

#endif