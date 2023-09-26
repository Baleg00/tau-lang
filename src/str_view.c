/**
 * \file str_view.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "str_view.h"

#include <string.h>

#include "str.h"
#include "util.h"

string_view_t string_view_init(const char* buf)
{
  return string_view_init_with_length(buf, strlen(buf));
}

string_view_t string_view_init_with_length(const char* buf, size_t len)
{
  return (string_view_t){ .buf = buf, .len = len };
}

const char* string_view_begin(string_view_t* str)
{
  return str->buf;
}

const char* string_view_end(string_view_t* str)
{
  return str->buf + str->len;
}

size_t string_view_length(string_view_t* str)
{
  return str->len;
}

string_view_t string_view_substr(string_view_t* str, size_t pos, size_t len)
{
  assert(pos + len <= str->len);

  return (string_view_t){ .buf = str->buf + pos, .len = len };
}

int string_view_compare(string_view_t* lhs, string_view_t* rhs)
{
  return strncmp(lhs->buf, rhs->buf, min(lhs->len, rhs->len));
}

int string_view_compare_cstr(string_view_t* lhs, const char* rhs)
{
  return strncmp(lhs->buf, rhs, lhs->len);
}

bool string_view_starts_with(string_view_t* str, string_view_t* prefix)
{
  if (prefix->len > str->len)
    return false;

  return strncmp(str->buf, prefix->buf, prefix->len) == 0;
}

bool string_view_starts_with_cstr(string_view_t* str, const char* prefix)
{
  size_t prefix_len = strlen(prefix);

  if (prefix_len > str->len)
    return false;

  return strncmp(str->buf, prefix, prefix_len) == 0;
}

bool string_view_ends_with(string_view_t* str, string_view_t* suffix)
{
  if (suffix->len > str->len)
    return false;

  return strncmp(str->buf + str->len - suffix->len, suffix->buf, suffix->len) == 0;
}

bool string_view_ends_with_cstr(string_view_t* str, const char* suffix)
{
  size_t suffix_len = strlen(suffix);

  if (suffix_len > str->len)
    return false;

  return strncmp(str->buf + str->len - suffix_len, suffix, suffix_len) == 0;
}

bool string_view_contains(string_view_t* str, string_view_t* sub)
{
  if (sub->len > str->len)
    return false;

  for (size_t i = 0; i <= str->len - sub->len;)
  {
    size_t j = 0;

    for (; j < sub->len; j++)
      if (str->buf[i + j] != sub->buf[j])
        break;

    if (j == sub->len)
      return true;
    else if (j == 0)
      i++;
    else
      i += j;
  }

  return false;
}

bool string_view_contains_cstr(string_view_t* str, const char* sub)
{
  size_t sub_len = strlen(sub);

  if (sub_len > str->len)
    return false;

  for (size_t i = 0; i <= str->len - sub_len;)
  {
    size_t j = 0;

    for (; j < sub_len; j++)
      if (str->buf[i + j] != sub[j])
        break;

    if (j == sub_len)
      return true;
    else if (j == 0)
      i++;
    else
      i += j;
  }

  return false;
}

size_t string_view_find(string_view_t* str, string_view_t* sub)
{
  if (sub->len > str->len)
    return str->len;

  for (size_t i = 0; i <= str->len - sub->len;)
  {
    size_t j = 0;

    for (; j < sub->len; j++)
      if (str->buf[i + j] != sub->buf[j])
        break;

    if (j == sub->len)
      return i;
    else if (j == 0)
      i++;
    else
      i += j;
  }

  return str->len;
}

size_t string_view_find_cstr(string_view_t* str, const char* sub)
{
  size_t sub_len = strlen(sub);

  if (sub_len > str->len)
    return str->len;

  for (size_t i = 0; i <= str->len - sub_len;)
  {
    size_t j = 0;

    for (; j < sub_len; j++)
      if (str->buf[i + j] != sub[j])
        break;

    if (j == sub_len)
      return i;
    else if (j == 0)
      i++;
    else
      i += j;
  }

  return str->len;
}
