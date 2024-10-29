/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/str.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "utils/common.h"
#include "utils/io/log.h"
#include "utils/memory/memtrace.h"

struct string_t
{
  size_t cap; // Maxmimum capacity of the buffer.
  size_t len; // Length of the string (excluding the terminating null character).
  char* buf; // Pointer to character buffer.
  bool owning; // `true` if the buffer is owned by the string, `false` otherwise.
};

string_t* string_init(void)
{
  return string_init_with_capacity(1);
}

string_t* string_init_with_cstr(const char* cstr)
{
  return string_init_with_cstr_and_length(cstr, strlen(cstr));
}

string_t* string_init_with_cstr_and_length(const char* cstr, size_t len)
{
  string_t* str = string_init_with_capacity(len + 1);

  memcpy(str->buf, cstr, len);
  str->buf[len] = '\0';
  str->len = len;

  return str;
}

string_t* string_init_with_capacity(size_t cap)
{
  string_t* str = (string_t*)malloc(sizeof(string_t));
  ASSERT(str != NULL);

  str->buf = (char*)calloc(cap, sizeof(char));
  ASSERT(str->buf != NULL);

  str->cap = cap;
  str->len = 0;
  str->owning = true;

  return str;
}

string_t* string_init_with_buffer(char* buf, size_t cap)
{
  ASSERT(buf != NULL);

  string_t* str = (string_t*)malloc(sizeof(string_t));
  ASSERT(str != NULL);

  str->buf = buf;
  str->cap = cap;
  str->len = 0;
  str->owning = false;

  return str;
}

void string_free(string_t* str)
{
  if (str->owning)
    free(str->buf);

  free(str);
}

char* string_begin(const string_t* str)
{
  return str->buf;
}

char* string_end(const string_t* str)
{
  return str->buf + str->len;
}

size_t string_length(const string_t* str)
{
  return str->len;
}

size_t string_capacity(const string_t* str)
{
  return str->cap;
}

void string_reserve(string_t* str, size_t cap)
{
  if (str->cap >= cap)
    return;

  ASSERT(str->owning);

  char* new_buf = (char*)realloc(str->buf, cap * sizeof(char));
  ASSERT(new_buf != NULL);

  str->cap = cap;
  str->buf = new_buf;
}

void string_fit(string_t* str)
{
  if (!str->owning)
    return;

  char* new_buf = (char*)realloc(str->buf, str->len + 1);
  ASSERT(new_buf != NULL);

  str->cap = str->len + 1;
  str->buf = new_buf;
}

int string_printf(FILE* restrict stream, const string_t* restrict fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  int result = vfprintf(stream, fmt->buf, args);

  va_end(args);

  return result;
}

int string_print_escaped(FILE* restrict stream, const string_t* restrict str)
{
  int result = 0;

  for (size_t i = 0; i < str->len; i++)
    switch (str->buf[i])
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
    default: result++; fputc(str->buf[i], stream);
    }

  return result;
}

void string_append(string_t* restrict str, const string_t* restrict other)
{
  string_append_cstr(str, other->buf);
}

void string_append_cstr(string_t* restrict str, const char* restrict other)
{
  size_t other_len = strlen(other);

  if (str->cap < str->len + other_len + 1)
    string_reserve(str, str->len + other_len + 1);

  str->len += other_len;
  strcat(str->buf, other);
}

void string_insert(string_t* restrict str, size_t pos, const string_t* restrict other)
{
  string_insert_cstr(str, pos, other->buf);
}

void string_insert_cstr(string_t* restrict str, size_t pos, const char* restrict other)
{
  ASSERT(pos <= str->len);

  size_t other_len = strlen(other);
  size_t new_len = str->len + other_len;

  string_reserve(str, new_len + 1);

  memmove(str->buf + pos + other_len, str->buf + pos, str->len - pos + 1);
  memcpy(str->buf + pos, other, other_len);

  str->len = new_len;
}

void string_erase(string_t* str, size_t pos, size_t len)
{
  ASSERT(pos + len <= str->len);

  memmove(str->buf + pos, str->buf + pos + len, str->len - pos - len + 1);
}

void string_clear(string_t* str)
{
  str->buf[0] = '\0';
  str->len = 0;
}

string_t* string_copy(const string_t* str)
{
  return string_init_with_cstr(str->buf);
}

string_t* string_substr(const string_t* str, size_t begin, size_t len)
{
  string_t* result = string_init_with_capacity(len + 1);

  result->len = len;
  strncpy(result->buf, str->buf + begin, len);

  return result;
}

int string_compare(const string_t* lhs, const string_t* rhs)
{
  return strcmp(lhs->buf, rhs->buf);
}

int string_compare_cstr(const string_t* restrict lhs, const char* restrict rhs)
{
  return strcmp(lhs->buf, rhs);
}

string_t* string_escape(const string_t* str)
{
  size_t len = str->len;

  for (char* ch = str->buf; *ch != '\0'; ++ch)
    switch (*ch)
    {
    case '\'':
    case '"':
    case '\\':
    case '\a':
    case '\b':
    case '\f':
    case '\n':
    case '\r':
    case '\t':
    case '\v':
      ++len;
    }

  string_t* result = string_init_with_capacity(len + 1);
  result->len = len;

  for (char *ch = str->buf, *rch = result->buf; *ch != '\0'; ++ch, ++rch)
    switch (*ch)
    {
    case '\'': *rch++ = '\\'; *rch = '\''; break;
    case '"':  *rch++ = '\\'; *rch = '"'; break;
    case '\\': *rch++ = '\\'; *rch = '\\'; break;
    case '\a': *rch++ = '\\'; *rch = 'a'; break;
    case '\b': *rch++ = '\\'; *rch = 'b'; break;
    case '\f': *rch++ = '\\'; *rch = 'f'; break;
    case '\n': *rch++ = '\\'; *rch = 'n'; break;
    case '\r': *rch++ = '\\'; *rch = 'r'; break;
    case '\t': *rch++ = '\\'; *rch = 't'; break;
    case '\v': *rch++ = '\\'; *rch = 'v'; break;
    default: *rch = *ch;
    }

  return result;
}

bool string_starts_with(const string_t* restrict str, const string_t* restrict prefix)
{
  return string_starts_with_cstr(str, prefix->buf);
}

bool string_starts_with_cstr(const string_t* restrict str, const char* restrict prefix)
{
  return strncmp(str->buf, prefix, MIN(str->len, strlen(prefix))) == 0;
}

bool string_ends_with(const string_t* restrict str, const string_t* restrict suffix)
{
  return string_ends_with_cstr(str, suffix->buf);
}

bool string_ends_with_cstr(const string_t* restrict str, const char* restrict suffix)
{
  return strcmp(str->buf + (str->len - strlen(suffix)), suffix) == 0;
}

bool string_contains(const string_t* restrict str, const string_t* restrict sub)
{
  return string_contains_cstr(str, sub->buf);
}

bool string_contains_cstr(const string_t* restrict str, const char* restrict sub)
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

    if (j == 0)
      i++;
    else
      i += j;
  }

  return false;
}

void string_replace(string_t* restrict str, size_t pos, size_t len, const string_t* restrict rep)
{
  string_replace_with_csubstr(str, pos, len, rep->buf, 0);
}

void string_replace_with_substr(string_t* restrict str, size_t pos, size_t len, const string_t* restrict rep, size_t rep_pos)
{
  string_replace_with_csubstr(str, pos, len, rep->buf, rep_pos);
}

void string_replace_with_cstr(string_t* restrict str, size_t pos, size_t len, const char* restrict rep)
{
  string_replace_with_csubstr(str, pos, len, rep, 0);
}

void string_replace_with_csubstr(string_t* restrict str, size_t pos, size_t len, const char* restrict rep, size_t rep_pos)
{
  size_t rep_len = strlen(rep);

  ASSERT(pos <= str->len);
  ASSERT((rep_len > 0 && rep_pos < rep_len) || rep_pos == 0);

  size_t new_len = str->len - len + rep_len - rep_pos;

  string_reserve(str, new_len + 1);

  memmove(str->buf + pos + rep_len - rep_pos, str->buf + pos + len, str->len - pos - len + 1);
  memcpy(str->buf + pos, rep + rep_pos, rep_len - rep_pos);

  str->len = new_len;
}

size_t string_find(const string_t* restrict str, const string_t* restrict sub)
{
  return string_find_cstr(str, sub->buf);
}

size_t string_find_cstr(const string_t* restrict str, const char* restrict sub)
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

    if (j == 0)
      i++;
    else
      i += j;
  }

  return str->len;
}
