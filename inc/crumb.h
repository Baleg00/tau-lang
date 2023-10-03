/**
 * \file crumb.h
 * 
 * \brief Compiler message library interface.
 * 
 * \details The crumb library provides functions to display detailed messages
 * referencing locations in the source code making it possible to create
 * informative error and warning messages for the user during compilation.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_CRUMB_H
#define TAU_CRUMB_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "location.h"

/**
 * \brief Enumeration of crumb kinds.
 */
typedef enum crumb_kind_e
{
  CRUMB_WARN, // Undesired code or behaviour.
  CRUMB_ERROR // Illegal code or behaviour.
} crumb_kind_t;

/**
 * \brief Enumeraion of crumb item kinds.
 */
typedef enum crumb_item_kind_e
{
  CRUMB_ITEM_MESSAGE, // Plain text.
  CRUMB_ITEM_SNIPPET, // Source code snippet.
} crumb_item_kind_t;

/**
 * \brief Crumb message item.
*/
typedef struct crumb_item_message_t
{
  const char* msg; // Message format string.
  va_list msg_args; // Message arguments.
} crumb_item_message_t;

/**
 * \brief Crumb snippet item.
 */
typedef struct crumb_item_snippet_t
{
  location_t* loc; // Source code location.
  const char* title; // Title format string.
  va_list title_args; // Title arguments.
  const char* msg; // Message format string.
  va_list msg_args; // Message arguments.
} crumb_item_snippet_t;

/**
 * \brief Crumb item.
 */
typedef struct crumb_item_t
{
  crumb_item_kind_t kind; // Item kind.

  union
  {
    crumb_item_message_t msg; // Message item.
    crumb_item_snippet_t snip; // Snippet item.
  };
} crumb_item_t;

/**
 * \brief Initializes a crumb message item.
 * 
 * \param[in] MSG Message format string to be displayed.
 * \param[in] MSG_ARGS Variadic argument list for message.
*/
#define crumb_message(MSG, MSG_ARGS)\
  ((crumb_item_t){\
    .kind = CRUMB_ITEM_MESSAGE,\
    .msg = (crumb_item_message_t){\
      .msg = (MSG),\
      .msg_args = (MSG_ARGS)\
    }\
  })

/**
 * \brief Initializes a crumb snippet item.
 * 
 * \param[in] LOC Source code location to be referenced.
 * \param[in] TITLE Title format string to be displayed.
 * \param[in] TITLE_ARGS Variadic argument list for title.
 * \param[in] MSG Message format string to be displayed.
 * \param[in] MSG_ARGS Variadic argument list for message.
*/
#define crumb_snippet(LOC, TITLE, TITLE_ARGS, MSG, MSG_ARGS)\
  ((crumb_item_t){\
    .kind = CRUMB_ITEM_SNIPPET,\
    .snip = (crumb_item_snippet_t){\
      .loc = (LOC),\
      .title = (TITLE),\
      .title_args = (TITLE_ARGS),\
      .msg = (MSG),\
      .msg_args = (MSG_ARGS)\
    }\
  })

/**
 * \brief Logs crumb messages to the output stream.
 * 
 * \param kind Crumb message kind.
 * \param count Number of crumb items.
 * \param ... Exactly `count` crumb items.
 */
void crumb_log(crumb_kind_t kind, size_t count, ...);

/**
 * \brief Sets the crumb output stream.
 * 
 * \param[in] stream Pointer to the output stream.
 */
void crumb_set_stream(FILE* stream);

/**
 * \brief Gets the crumb output stream.
 * 
 * \returns Pointer to the output stream.
 */
FILE* crumb_get_stream(void);

/**
 * \brief Converts the crumb kind to its corresponding ANSI color string.
 * 
 * \param[in] kind The crumb kind to convert.
 * \returns The ANSI color string of the crumb kind.
 */
const char* crumb_kind_to_color(crumb_kind_t kind);

/**
 * \brief Converts the crumb kind to its corresponding string representation.
 * 
 * \param[in] kind The crumb kind to convert.
 * \returns The string representation of the crumb kind.
 */
const char* crumb_kind_to_string(crumb_kind_t kind);

/**
 * \brief Logs a crumb warning message to the output stream.
 */
#define crumb_warn(...)   crumb_log(CRUMB_WARN , __VA_ARGS__)

/**
 * \brief Logs an crumb error message to the output stream.
 */
#define crumb_error(...)  crumb_log(CRUMB_ERROR, __VA_ARGS__)

#endif