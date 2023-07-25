/**
 * \file log.c
 * 
 * \copyright Copyright (c) Róna Balázs. All rights reserved.
 * \license 
*/

#include "log.h"

#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "util.h"
#include "esc_seq.h"
#include "file.h"

static log_level_t g_log_level = LOG_LEVEL_TRACE;
static FILE* g_log_stream = NULL;
static bool g_log_verbose = false;

void log_log(log_level_t lvl, const char* file, int line, const char* func, const char* name, const char* fmt, ...)
{
  unused(func);

  if (lvl < g_log_level || g_log_stream == NULL)
    return;

  if (g_log_verbose)
  {
    time_t tm = time(NULL);
    
    char time_buf[9];
    size_t len = strftime(time_buf, sizeof(time_buf), "%H:%M:%S", localtime(&tm));
    time_buf[len] = '\0';

    fprintf(g_log_stream, ESC_FG_BRIGHT_BLACK "%s:%d %s " ESC_RESET,
      file, line, time_buf);
  }

  fprintf(g_log_stream, "[%s%s:%s" ESC_RESET "]> ", 
    log_level_to_color(lvl), log_level_to_string(lvl), name);

  va_list args;
  va_start(args, fmt);

  vfprintf(g_log_stream, fmt, args);

  va_end(args);

  fputc('\n', g_log_stream);
}

const char* log_level_to_string(log_level_t lvl)
{
  switch (lvl)
  {
  case LOG_LEVEL_TRACE: return "TRACE";
  case LOG_LEVEL_DEBUG: return "DEBUG";
  case LOG_LEVEL_INFO:  return "INFO";
  case LOG_LEVEL_WARN:  return "WARN";
  case LOG_LEVEL_ERROR: return "ERROR";
  case LOG_LEVEL_FATAL: return "FATAL";
  default: return "";
  }
}

const char* log_level_to_color(log_level_t lvl)
{
  switch (lvl)
  {
  case LOG_LEVEL_TRACE: return ESC_FG_BRIGHT_BLACK;
  case LOG_LEVEL_DEBUG: return ESC_FG_CYAN;
  case LOG_LEVEL_INFO:  return ESC_FG_WHITE;
  case LOG_LEVEL_WARN:  return ESC_FG_YELLOW;
  case LOG_LEVEL_ERROR: return ESC_FG_RED;
  case LOG_LEVEL_FATAL: return ESC_BG_RED;
  default: return "";
  }
}

void log_set_level(log_level_t lvl)
{
  g_log_level = lvl;
}

log_level_t log_get_level(void)
{
  return g_log_level;
}

void log_set_stream(FILE* stream)
{
  g_log_stream = stream;
}

FILE* log_get_stream(void)
{
  return g_log_stream;
}

void log_set_verbose(bool value)
{
  g_log_verbose = value;
}

bool log_get_verbose(void)
{
  return g_log_verbose;
}
