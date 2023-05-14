#include "crumb.h"

#include "util.h"
#include "esc_seq.h"

#include "file.h"

#include "location.h"

/** Global crumb file stream. */
static FILE* crumb_file_g = NULL;

void crumb_log_message(crumb_kind_t kind, crumb_item_message_t* item)
{
  fprintf(crumb_file_g, "\n    ");
  vfprintf(crumb_file_g, item->msg, item->msg_args);
  fputc('\n', crumb_file_g);
}

void crumb_log_snippet(crumb_kind_t kind, crumb_item_snippet_t* item)
{
  location_t* loc = item->loc;

  { // Print message header: [<filename>:<row>:<column>]> <title>
    char filename_buf[255];
    file_name(loc->path, filename_buf, sizeof(filename_buf));

    fprintf(crumb_file_g, "\n[" ESC_FG_BRIGHT_BLACK "%s:%zu:%zu" ESC_RESET "]> %s", 
      filename_buf, loc->row + 1, loc->col + 1,
      crumb_kind_to_color(kind));

    vfprintf(crumb_file_g, item->title, item->title_args);

    fprintf(crumb_file_g, ESC_RESET "\n");
  }

  { // Print snippet and message
    // Find line begin and end
    // `line_begin` points to the first character of the line
    // `line_end` points to the character after the last character of the line
    const char *line_begin = loc->cur, *line_end = loc->cur;

    while (line_begin > loc->src && *(line_begin - 1) != '\n')
      --line_begin;

    while (*line_end != '\0' && *line_end != '\n')
      ++line_end;

    // Print the referenced line: <row> | <text>
    fprintf(crumb_file_g, "    %s%zu" ESC_RESET " | %.*s%s%.*s" ESC_RESET "%.*s\n",
      crumb_kind_to_color(kind), // Row number color
      loc->row + 1, // Row number
      loc->cur - line_begin, line_begin, // From line begin to referenced part
      crumb_kind_to_color(kind), // Highlight color
      loc->len, loc->cur, // Referenced part
      line_end - loc->cur - loc->len, loc->cur + loc->len); // Rest of line

    { // Print underline
      int offset = snprintf(NULL, 0, "    %zu | %.*s", 
        loc->row + 1, 
        loc->cur - line_begin, line_begin);

      for (int i = 0; i < offset; ++i)
        fputc(' ', crumb_file_g);

      fprintf(crumb_file_g, crumb_kind_to_color(kind));

      fputc('^', crumb_file_g);

      for (size_t i = 1; i < loc->len; ++i)
        fputc('~', crumb_file_g);

      fputc(' ', crumb_file_g);
    }

    { // Print format string with arguments
      vfprintf(crumb_file_g, item->msg, item->msg_args);
      fprintf(crumb_file_g, ESC_RESET "\n");
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

void crumb_set_file(FILE* file)
{
  crumb_file_g = file;
}

FILE* crumb_get_file(void)
{
  return crumb_file_g;
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
