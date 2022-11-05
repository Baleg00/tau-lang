#include "crumb.h"

#include <stdio.h>
#include <stdarg.h>

#include "esc_seq.h"

static FILE* crumb_global_file = NULL;

void crumb_log(crumb_kind_t kind, location_t* loc, const char* fmt, ...)
{
  fprintf(crumb_global_file, "[" ESC_FG_BRIGHT_BLACK "%s:%zu:%zu" ESC_RESET "]\n", loc->path, loc->row + 1, loc->col + 1);

  const char *line_begin = loc->cur, *line_end = loc->cur;

  while (line_begin > loc->src && *(line_begin - 1) != '\n')
    --line_begin;

  while (*line_end != '\0' && *line_end != '\n')
    ++line_end;

  fprintf(crumb_global_file, "    %zu | %.*s%s%.*s" ESC_RESET "%.*s\n", 
    loc->row + 1, 
    loc->cur - line_begin, line_begin,
    crumb_kind_to_color(kind),
    loc->len, loc->cur,
    line_end - loc->cur - loc->len, loc->cur + loc->len);

  int offset = snprintf(NULL, 0, "    %zu | %.*s", 
    loc->row + 1, 
    loc->cur - line_begin, line_begin);

  for (int i = 0; i < offset; ++i)
    fputc(' ', crumb_global_file);

  fprintf(crumb_global_file, crumb_kind_to_color(kind));

  fputc('^', crumb_global_file);

  for (size_t i = 1; i < loc->len; ++i)
    fputc('~', crumb_global_file);

  fputc(' ', crumb_global_file);

  va_list args;
  va_start(args, fmt);

  vfprintf(crumb_global_file, fmt, args);

  va_end(args);

  fprintf(crumb_global_file, ESC_RESET "\n");
}

void crumb_set_file(FILE* file)
{
  crumb_global_file = file;
}

FILE* crumb_get_file(void)
{
  return crumb_global_file;
}

const char* crumb_kind_to_color(crumb_kind_t kind)
{
  switch (kind)
  {
  case CRUMB_WARN: return ESC_FG_YELLOW;
  case CRUMB_ERROR: return ESC_FG_RED;
  default: return "";
  }
}
