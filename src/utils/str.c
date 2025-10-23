/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/str.h"

#include "utils/common.h"

struct tau_string_t
{
  size_t cap; // Maxmimum capacity of the buffer.
  size_t len; // Length of the string (excluding the terminating null character).
  char* buf; // Pointer to character buffer.
  bool owning; // `true` if the buffer is owned by the string, `false` otherwise.
};

tau_string_t* tau_string_init(void)
{
  return tau_string_init_with_capacity(1);
}

tau_string_t* tau_string_init_with_cstr(const char* cstr)
{
  return tau_string_init_with_cstr_and_length(cstr, strlen(cstr));
}

tau_string_t* tau_string_init_with_cstr_and_length(const char* cstr, size_t len)
{
  tau_string_t* str = tau_string_init_with_capacity(len + 1);

  memcpy(str->buf, cstr, len);
  str->buf[len] = '\0';
  str->len = len;

  return str;
}

tau_string_t* tau_string_init_with_capacity(size_t cap)
{
  tau_string_t* str = (tau_string_t*)malloc(sizeof(tau_string_t));
  TAU_ASSERT(str != NULL);

  str->buf = (char*)calloc(cap, sizeof(char));
  TAU_ASSERT(str->buf != NULL);

  str->cap = cap;
  str->len = 0;
  str->owning = true;

  return str;
}

tau_string_t* tau_string_init_with_buffer(char* buf, size_t cap)
{
  TAU_ASSERT(buf != NULL);

  tau_string_t* str = (tau_string_t*)malloc(sizeof(tau_string_t));
  TAU_ASSERT(str != NULL);

  str->buf = buf;
  str->cap = cap;
  str->len = 0;
  str->owning = false;

  return str;
}

void tau_string_free(tau_string_t* str)
{
  if (str->owning)
    free(str->buf);

  free(str);
}

char* tau_string_begin(const tau_string_t* str)
{
  return str->buf;
}

char* tau_string_end(const tau_string_t* str)
{
  return str->buf + str->len;
}

size_t tau_string_length(const tau_string_t* str)
{
  return str->len;
}

size_t tau_string_capacity(const tau_string_t* str)
{
  return str->cap;
}

void tau_string_reserve(tau_string_t* str, size_t cap)
{
  if (str->cap >= cap)
    return;

  TAU_ASSERT(str->owning);

  char* new_buf = (char*)realloc(str->buf, cap * sizeof(char));
  TAU_ASSERT(new_buf != NULL);

  str->cap = cap;
  str->buf = new_buf;
}

void tau_string_fit(tau_string_t* str)
{
  if (!str->owning)
    return;

  char* new_buf = (char*)realloc(str->buf, str->len + 1);
  TAU_ASSERT(new_buf != NULL);

  str->cap = str->len + 1;
  str->buf = new_buf;
}

int tau_string_printf(FILE* restrict stream, const tau_string_t* restrict fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  int result = vfprintf(stream, fmt->buf, args);

  va_end(args);

  return result;
}

int tau_string_print_escaped(FILE* restrict stream, const tau_string_t* restrict str)
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

void tau_string_append(tau_string_t* restrict str, const tau_string_t* restrict other)
{
  tau_string_append_cstr(str, other->buf);
}

void tau_string_append_cstr(tau_string_t* restrict str, const char* restrict other)
{
  size_t other_len = strlen(other);

  if (str->cap < str->len + other_len + 1)
    tau_string_reserve(str, str->len + other_len + 1);

  str->len += other_len;
  strcat(str->buf, other);
}

void tau_string_insert(tau_string_t* restrict str, size_t pos, const tau_string_t* restrict other)
{
  tau_string_insert_cstr(str, pos, other->buf);
}

void tau_string_insert_cstr(tau_string_t* restrict str, size_t pos, const char* restrict other)
{
  TAU_ASSERT(pos <= str->len);

  size_t other_len = strlen(other);
  size_t new_len = str->len + other_len;

  tau_string_reserve(str, new_len + 1);

  memmove(str->buf + pos + other_len, str->buf + pos, str->len - pos + 1);
  memcpy(str->buf + pos, other, other_len);

  str->len = new_len;
}

void tau_string_erase(tau_string_t* str, size_t pos, size_t len)
{
  TAU_ASSERT(pos + len <= str->len);

  memmove(str->buf + pos, str->buf + pos + len, str->len - pos - len + 1);
}

void tau_string_clear(tau_string_t* str)
{
  str->buf[0] = '\0';
  str->len = 0;
}

tau_string_t* tau_string_copy(const tau_string_t* str)
{
  return tau_string_init_with_cstr(str->buf);
}

tau_string_t* tau_string_substr(const tau_string_t* str, size_t begin, size_t len)
{
  tau_string_t* result = tau_string_init_with_capacity(len + 1);

  result->len = len;
  strncpy(result->buf, str->buf + begin, len);

  return result;
}

int tau_string_compare(const tau_string_t* lhs, const tau_string_t* rhs)
{
  return strcmp(lhs->buf, rhs->buf);
}

int tau_string_compare_cstr(const tau_string_t* restrict lhs, const char* restrict rhs)
{
  return strcmp(lhs->buf, rhs);
}

tau_string_t* tau_string_escape(const tau_string_t* str)
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

  tau_string_t* result = tau_string_init_with_capacity(len + 1);
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

bool tau_string_starts_with(const tau_string_t* restrict str, const tau_string_t* restrict prefix)
{
  return tau_string_starts_with_cstr(str, prefix->buf);
}

bool tau_string_starts_with_cstr(const tau_string_t* restrict str, const char* restrict prefix)
{
  return strncmp(str->buf, prefix, TAU_MIN(str->len, strlen(prefix))) == 0;
}

bool tau_string_ends_with(const tau_string_t* restrict str, const tau_string_t* restrict suffix)
{
  return tau_string_ends_with_cstr(str, suffix->buf);
}

bool tau_string_ends_with_cstr(const tau_string_t* restrict str, const char* restrict suffix)
{
  return strcmp(str->buf + (str->len - strlen(suffix)), suffix) == 0;
}

bool tau_string_contains(const tau_string_t* restrict str, const tau_string_t* restrict sub)
{
  return tau_string_contains_cstr(str, sub->buf);
}

bool tau_string_contains_cstr(const tau_string_t* restrict str, const char* restrict sub)
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

void tau_string_replace(tau_string_t* restrict str, size_t pos, size_t len, const tau_string_t* restrict rep)
{
  tau_string_replace_with_csubstr(str, pos, len, rep->buf, 0);
}

void tau_string_replace_with_substr(tau_string_t* restrict str, size_t pos, size_t len, const tau_string_t* restrict rep, size_t rep_pos)
{
  tau_string_replace_with_csubstr(str, pos, len, rep->buf, rep_pos);
}

void tau_string_replace_with_cstr(tau_string_t* restrict str, size_t pos, size_t len, const char* restrict rep)
{
  tau_string_replace_with_csubstr(str, pos, len, rep, 0);
}

void tau_string_replace_with_csubstr(tau_string_t* restrict str, size_t pos, size_t len, const char* restrict rep, size_t rep_pos)
{
  size_t rep_len = strlen(rep);

  TAU_ASSERT(pos <= str->len);
  TAU_ASSERT((rep_len > 0 && rep_pos < rep_len) || rep_pos == 0);

  size_t new_len = str->len - len + rep_len - rep_pos;

  tau_string_reserve(str, new_len + 1);

  memmove(str->buf + pos + rep_len - rep_pos, str->buf + pos + len, str->len - pos - len + 1);
  memcpy(str->buf + pos, rep + rep_pos, rep_len - rep_pos);

  str->len = new_len;
}

size_t tau_string_find(const tau_string_t* restrict str, const tau_string_t* restrict sub)
{
  return tau_string_find_cstr(str, sub->buf);
}

size_t tau_string_find_cstr(const tau_string_t* restrict str, const char* restrict sub)
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
