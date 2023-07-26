/**
 * \file crumb.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "crumb.h"

#include "esc_seq.h"
#include "file.h"
#include "util.h"

static FILE* g_crumb_stream = NULL;

void crumb_log_message(crumb_kind_t kind, crumb_item_message_t* item)
{
  unused(kind);

  fprintf(g_crumb_stream, "\n    ");
  vfprintf(g_crumb_stream, item->msg, item->msg_args);
  fputc('\n', g_crumb_stream);
}

void crumb_log_snippet(crumb_kind_t kind, crumb_item_snippet_t* item)
{
  location_t* loc = item->loc;

  { // Print message header: [<filename>:<row>:<column>]> <title>
    char filename_buf[255];
    file_name(location_get_path(loc), filename_buf, sizeof(filename_buf));

    fprintf(g_crumb_stream, "\n[" ESC_FG_BRIGHT_BLACK "%s:%zu:%zu" ESC_RESET "]> %s", 
      filename_buf, location_get_row(loc) + 1, location_get_col(loc) + 1,
      crumb_kind_to_color(kind));

    vfprintf(g_crumb_stream, item->title, item->title_args);

    fprintf(g_crumb_stream, ESC_RESET "\n");
  }

  { // Print snippet and message
    // Find line begin and end
    // `line_begin` points to the first character of the line
    // `line_end` points to the character after the last character of the line
    const char *line_begin = location_get_ptr(loc), *line_end = location_get_ptr(loc);

    while (line_begin > location_get_src(loc) && *(line_begin - 1) != '\n')
      --line_begin;

    while (*line_end != '\0' && *line_end != '\n')
      ++line_end;

    // Print the referenced line: <row> | <text>
    fprintf(g_crumb_stream, "    %s%zu" ESC_RESET " | %.*s%s%.*s" ESC_RESET "%.*s\n",
      crumb_kind_to_color(kind), // Row number color
      location_get_row(loc) + 1, // Row number
      (int)(location_get_ptr(loc) - line_begin), line_begin, // From line begin to referenced part
      crumb_kind_to_color(kind), // Highlight color
      (int)location_get_len(loc), location_get_ptr(loc), // Referenced part
      (int)(line_end - location_get_ptr(loc) - location_get_len(loc)), location_get_ptr(loc) + location_get_len(loc)); // Rest of line

    { // Print underline
      int offset = snprintf(NULL, 0, "    %zu | %.*s", 
        location_get_row(loc) + 1, 
        (int)(location_get_ptr(loc) - line_begin), line_begin);

      for (int i = 0; i < offset; ++i)
        fputc(' ', g_crumb_stream);

      fprintf(g_crumb_stream, crumb_kind_to_color(kind));

      fputc('^', g_crumb_stream);

      for (size_t i = 1; i < location_get_len(loc); ++i)
        fputc('~', g_crumb_stream);

      fputc(' ', g_crumb_stream);
    }

    { // Print format string with arguments
      vfprintf(g_crumb_stream, item->msg, item->msg_args);
      fprintf(g_crumb_stream, ESC_RESET "\n");
    }
  }
}

void crumb_log(crumb_kind_t kind, size_t count, ...)
{
  va_list args;
  va_start(args, count);

  for (; count > 0; --count)
  {
    crumb_item_t item = va_arg(args, crumb_item_t);

    switch (item.kind)
    {
    case CRUMB_ITEM_MESSAGE: crumb_log_message(kind, &item.msg); break;
    case CRUMB_ITEM_SNIPPET: crumb_log_snippet(kind, &item.snip); break;
    default: unreachable();
    }
  }

  va_end(args);
}

void crumb_set_stream(FILE* file)
{
  g_crumb_stream = file;
}

FILE* crumb_get_stream(void)
{
  return g_crumb_stream;
}

const char* crumb_kind_to_color(crumb_kind_t kind)
{
  switch (kind)
  {
  case CRUMB_WARN:  return ESC_FG_YELLOW;
  case CRUMB_ERROR: return ESC_FG_RED;
  default: unreachable();
  }

  return NULL;
}

const char* crumb_kind_to_string(crumb_kind_t kind)
{
  switch (kind)
  {
  case CRUMB_WARN:  return "WARNING";
  case CRUMB_ERROR: return "ERROR";
  default: unreachable();
  }

  return NULL;
}
