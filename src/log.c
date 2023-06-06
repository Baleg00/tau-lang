#include "log.h"

#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "util.h"
#include "esc_seq.h"
#include "file.h"

static log_level_t log_global_level = LOG_LEVEL_TRACE;
static FILE* log_global_file = NULL;
static bool log_global_verbose = false;

void log_log(log_level_t lvl, const char* file, int line, const char* func, const char* name, const char* fmt, ...)
{
  unused(func);

  if (lvl < log_global_level || log_global_file == NULL)
    return;

  if (log_global_verbose)
  {
    time_t tm = time(NULL);
    
    char time_buf[9];
    size_t len = strftime(time_buf, sizeof(time_buf), "%H:%M:%S", localtime(&tm));
    time_buf[len] = '\0';

    fprintf(log_global_file, ESC_FG_BRIGHT_BLACK "%s:%d %s " ESC_RESET,
      file, line, time_buf);
  }

  fprintf(log_global_file, "[%s%s:%s" ESC_RESET "]> ", 
    log_level_to_color(lvl), log_level_to_string(lvl), name);

  va_list args;
  va_start(args, fmt);

  vfprintf(log_global_file, fmt, args);

  va_end(args);

  fputc('\n', log_global_file);
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
  log_global_level = lvl;
}

log_level_t log_get_level(void)
{
  return log_global_level;
}

void log_set_file(FILE* file)
{
  log_global_file = file;
}

FILE* log_get_file(void)
{
  return log_global_file;
}

void log_set_verbose(bool value)
{
  log_global_verbose = value;
}

bool log_get_verbose(void)
{
  return log_global_verbose;
}
