/**
 * \file
 * 
 * \copyright Copyright (c) Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#include "utils/io/log.h"

#include "utils/common.h"
#include "utils/esc_seq.h"

static tau_log_level_t g_log_level = TAU_LOG_LEVEL_TRACE;
static FILE* g_log_stream = NULL;
static bool g_log_verbose = false;

void tau_log_log(tau_log_level_t lvl, const char* file, int line, const char* TAU_UNUSED(func), const char* name, const char* fmt, ...)
{
  if (lvl < g_log_level || g_log_stream == NULL)
    return;

  if (g_log_verbose)
  {
    time_t tm = time(NULL);

    char time_buf[9];
    size_t len = strftime(time_buf, sizeof(time_buf), "%H:%M:%S", localtime(&tm));
    time_buf[len] = '\0';

    fprintf(g_log_stream, TAU_ESC_FG_BRIGHT_BLACK "%s:%d %s " TAU_ESC_RESET,
      file, line, time_buf);
  }

  fprintf(g_log_stream, "[%s%s:%s" TAU_ESC_RESET "]> ",
    tau_log_level_to_color(lvl), tau_log_level_to_string(lvl), name);

  va_list args;
  va_start(args, fmt);

  vfprintf(g_log_stream, fmt, args);

  va_end(args);

  fputc('\n', g_log_stream);
}

const char* tau_log_level_to_string(tau_log_level_t lvl)
{
  switch (lvl)
  {
  case TAU_LOG_LEVEL_TRACE: return "TRACE";
  case TAU_LOG_LEVEL_DEBUG: return "DEBUG";
  case TAU_LOG_LEVEL_INFO:  return "INFO";
  case TAU_LOG_LEVEL_WARN:  return "WARN";
  case TAU_LOG_LEVEL_ERROR: return "ERROR";
  case TAU_LOG_LEVEL_FATAL: return "FATAL";
  default: return "";
  }
}

const char* tau_log_level_to_color(tau_log_level_t lvl)
{
  switch (lvl)
  {
  case TAU_LOG_LEVEL_TRACE: return TAU_ESC_FG_BRIGHT_BLACK;
  case TAU_LOG_LEVEL_DEBUG: return TAU_ESC_FG_CYAN;
  case TAU_LOG_LEVEL_INFO:  return TAU_ESC_FG_WHITE;
  case TAU_LOG_LEVEL_WARN:  return TAU_ESC_FG_YELLOW;
  case TAU_LOG_LEVEL_ERROR: return TAU_ESC_FG_RED;
  case TAU_LOG_LEVEL_FATAL: return TAU_ESC_BG_RED;
  default: return "";
  }
}

void tau_log_set_level(tau_log_level_t lvl)
{
  g_log_level = lvl;
}

tau_log_level_t tau_log_get_level(void)
{
  return g_log_level;
}

void tau_log_set_stream(FILE* stream)
{
  g_log_stream = stream;
}

FILE* tau_log_get_stream(void)
{
  return g_log_stream;
}

void tau_log_set_verbose(bool value)
{
  g_log_verbose = value;
}

bool tau_log_get_verbose(void)
{
  return g_log_verbose;
}
