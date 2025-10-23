/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/str_view.h"

#include "utils/common.h"

tau_string_view_t tau_string_view_init(const char* buf)
{
  return tau_string_view_init_with_length(buf, strlen(buf));
}

tau_string_view_t tau_string_view_init_with_length(const char* buf, size_t len)
{
  return (tau_string_view_t){ .buf = buf, .len = len };
}

const char* tau_string_view_begin(tau_string_view_t str)
{
  return str.buf;
}

const char* tau_string_view_end(tau_string_view_t str)
{
  return str.buf + str.len;
}

size_t tau_string_view_length(tau_string_view_t str)
{
  return str.len;
}

tau_string_view_t tau_string_view_substr(tau_string_view_t str, size_t pos, size_t len)
{
  TAU_ASSERT(pos + len <= str.len);

  return (tau_string_view_t){ .buf = str.buf + pos, .len = len };
}

int tau_string_view_compare(tau_string_view_t lhs, tau_string_view_t rhs)
{
  return strncmp(lhs.buf, rhs.buf, TAU_MIN(lhs.len, rhs.len));
}

int tau_string_view_compare_cstr(tau_string_view_t lhs, const char* rhs)
{
  return strncmp(lhs.buf, rhs, lhs.len);
}

bool tau_string_view_starts_with(tau_string_view_t str, tau_string_view_t prefix)
{
  if (prefix.len > str.len)
    return false;

  return strncmp(str.buf, prefix.buf, prefix.len) == 0;
}

bool tau_string_view_starts_with_cstr(tau_string_view_t str, const char* prefix)
{
  size_t prefix_len = strlen(prefix);

  if (prefix_len > str.len)
    return false;

  return strncmp(str.buf, prefix, prefix_len) == 0;
}

bool tau_string_view_ends_with(tau_string_view_t str, tau_string_view_t suffix)
{
  if (suffix.len > str.len)
    return false;

  return strncmp(str.buf + str.len - suffix.len, suffix.buf, suffix.len) == 0;
}

bool tau_string_view_ends_with_cstr(tau_string_view_t str, const char* suffix)
{
  size_t suffix_len = strlen(suffix);

  if (suffix_len > str.len)
    return false;

  return strncmp(str.buf + str.len - suffix_len, suffix, suffix_len) == 0;
}

bool tau_string_view_contains(tau_string_view_t str, tau_string_view_t sub)
{
  if (sub.len > str.len)
    return false;

  for (size_t i = 0; i <= str.len - sub.len;)
  {
    size_t j = 0;

    for (; j < sub.len; j++)
      if (str.buf[i + j] != sub.buf[j])
        break;

    if (j == sub.len)
      return true;

    if (j == 0)
      i++;
    else
      i += j;
  }

  return false;
}

bool tau_string_view_contains_cstr(tau_string_view_t str, const char* sub)
{
  size_t sub_len = strlen(sub);

  if (sub_len > str.len)
    return false;

  for (size_t i = 0; i <= str.len - sub_len;)
  {
    size_t j = 0;

    for (; j < sub_len; j++)
      if (str.buf[i + j] != sub[j])
        break;

    if (j == sub_len)
      return true;

    if (j == 0)
      i++;
    else
      i += j;
  }

  return false;
}

size_t tau_string_view_find(tau_string_view_t str, tau_string_view_t sub)
{
  if (sub.len > str.len)
    return str.len;

  for (size_t i = 0; i <= str.len - sub.len;)
  {
    size_t j = 0;

    for (; j < sub.len; j++)
      if (str.buf[i + j] != sub.buf[j])
        break;

    if (j == sub.len)
      return i;

    if (j == 0)
      i++;
    else
      i += j;
  }

  return str.len;
}

size_t tau_string_view_find_cstr(tau_string_view_t str, const char* sub)
{
  size_t sub_len = strlen(sub);

  if (sub_len > str.len)
    return str.len;

  for (size_t i = 0; i <= str.len - sub_len;)
  {
    size_t j = 0;

    for (; j < sub_len; j++)
      if (str.buf[i + j] != sub[j])
        break;

    if (j == sub_len)
      return i;

    if (j == 0)
      i++;
    else
      i += j;
  }

  return str.len;
}

int tau_string_view_printf(FILE* stream, tau_string_view_t fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  char* fmt_buf = (char*)malloc(fmt.len + 1);
  memcpy(fmt_buf, fmt.buf, fmt.len);
  fmt_buf[fmt.len] = '\0';

  int result = vfprintf(stream, fmt_buf, args);

  free(fmt_buf);

  va_end(args);

  return result;
}

int tau_string_view_print_escaped(FILE* stream, tau_string_view_t str)
{
  int result = 0;

  for (size_t i = 0; i < str.len; i++)
    switch (str.buf[i])
    {
    case '\'': result += fprintf(stream, "\\'" ); break;
    case '"':  result += fprintf(stream, "\\\""); break;
    case '\\': result += fprintf(stream, "\\\\"); break;
    case '\a': result += fprintf(stream, "\\a" ); break;
    case '\b': result += fprintf(stream, "\\b" ); break;
    case '\f': result += fprintf(stream, "\\f" ); break;
    case '\n': result += fprintf(stream, "\\n" ); break;
    case '\r': result += fprintf(stream, "\\r" ); break;
    case '\t': result += fprintf(stream, "\\t" ); break;
    case '\v': result += fprintf(stream, "\\v" ); break;
    default: result++; fputc(str.buf[i], stream);
    }

  return result;
}
