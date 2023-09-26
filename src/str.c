/**
 * \file str.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "str.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "memtrace.h"
#include "util.h"

struct string_s 
{
  size_t cap; // Maxmimum capacity of the buffer.
  size_t len; // Length of the string. (excluding the terminating null character).
  char* buf; // Pointer to character buffer.
  bool owning; // `true` if the buffer is owned by the string, `false` otherwise.
};

string_t* string_init(void)
{
  return string_init_capacity(1);
}

string_t* string_init_cstr(char* cstr)
{
  size_t len = strlen(cstr);
  string_t* str = string_init_capacity(len + 1);

  str->len = len;
  strcpy(str->buf, cstr);
  
  return str;
}

string_t* string_init_capacity(size_t cap)
{
  string_t* str = (string_t*)malloc(sizeof(string_t));
  assert(str != NULL);

  str->buf = (char*)calloc(cap, sizeof(char));
  assert(str->buf != NULL);

  str->cap = cap;
  str->len = 0;
  str->owning = true;

  return str;
}

string_t* string_init_buffer(char* buf, size_t cap)
{
  assert(buf != NULL);
  
  string_t* str = (string_t*)malloc(sizeof(string_t));
  assert(str != NULL);

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

char* string_begin(string_t* str)
{
  return str->buf;
}

char* string_end(string_t* str)
{
  return str->buf + str->len;
}

size_t string_length(string_t* str)
{
  return str->len;
}

size_t string_capacity(string_t* str)
{
  return str->cap;
}

void string_reserve(string_t* str, size_t cap)
{
  if (str->cap >= cap)
    return;

  if (!str->owning)
  {
    log_warn("string", "Cannot expand not owned buffer.");
    return;
  }

  char* new_buf = (char*)realloc(str->buf, cap * sizeof(char));

  if (new_buf == NULL)
    log_warn("string", "Failed to reserve space for string. Requested capacity: %zu", cap);
  else
  {
    str->cap = cap;
    str->buf = new_buf;
  }
}

void string_fit(string_t* str)
{
  if (!str->owning)
  {
    log_warn("string", "Cannot shrink not owned buffer.");
    return;
  }

  char* new_buf = (char*)realloc(str->buf, str->len + 1);

  if (new_buf == NULL)
    log_warn("string", "Failed to fit string capacity.");
  else
  {
    str->cap = str->len + 1;
    str->buf = new_buf;
  }
}

void string_append(string_t* str, string_t* other)
{
  string_append_cstr(str, other->buf);
}

void string_append_cstr(string_t* str, const char* other)
{
  size_t other_len = strlen(other);

  if (str->cap < str->len + other_len + 1)
    string_reserve(str, str->len + other_len + 1);

  str->len += other_len;
  strcat(str->buf, other);
}

void string_insert(string_t* str, size_t pos, string_t* other)
{
  string_insert_cstr(str, pos, other->buf);
}

void string_insert_cstr(string_t* str, size_t pos, const char* other)
{
  assert(pos <= str->len);

  size_t other_len = strlen(other);
  size_t new_len = str->len + other_len;

  string_reserve(str, new_len + 1);

  memmove(str->buf + pos + other_len, str->buf + pos, str->len - pos + 1);
  memcpy(str->buf + pos, other, other_len);

  str->len = new_len;
}

void string_erase(string_t* str, size_t pos, size_t len)
{
  assert(pos + len <= str->len);

  memmove(str->buf + pos, str->buf + pos + len, str->len - pos - len + 1);
}

string_t* string_substr(string_t* str, size_t begin, size_t len)
{
  string_t* result = string_init_capacity(len + 1);
  result->len = len;
  strncpy(result->buf, str->buf + begin, len);
  return result;
}

int string_compare(string_t* lhs, string_t* rhs)
{
  return strcmp(lhs->buf, rhs->buf);
}

int string_compare_cstr(string_t* lhs, const char* rhs)
{
  return strcmp(lhs->buf, rhs);
}

string_t* string_escape(string_t* str)
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

  string_t* result = string_init_capacity(len + 1);

  for (char *ch = str->buf, *rch = result->buf; *ch != '\0'; ++ch, ++rch)
    switch (*ch)
    {
    case '\'': *(rch++) = '\\'; *rch = '\''; break;
    case '"':  *(rch++) = '\\'; *rch = '"'; break;
    case '\\': *(rch++) = '\\'; *rch = '\\'; break;
    case '\a': *(rch++) = '\\'; *rch = 'a'; break;
    case '\b': *(rch++) = '\\'; *rch = 'b'; break;
    case '\f': *(rch++) = '\\'; *rch = 'f'; break;
    case '\n': *(rch++) = '\\'; *rch = 'n'; break;
    case '\r': *(rch++) = '\\'; *rch = 'r'; break;
    case '\t': *(rch++) = '\\'; *rch = 't'; break;
    case '\v': *(rch++) = '\\'; *rch = 'v'; break;
    default: *rch = *ch;
    }

  return result;
}

bool string_starts_with(string_t* str, string_t* prefix)
{
  return string_starts_with_cstr(str, prefix->buf);
}

bool string_starts_with_cstr(string_t* str, const char* prefix)
{
  return strncmp(str->buf, prefix, min(str->len, strlen(prefix))) == 0;
}

bool string_ends_with(string_t* str, string_t* suffix)
{
  return string_ends_with_cstr(str, suffix->buf);
}

bool string_ends_with_cstr(string_t* str, const char* suffix)
{
  return strcmp(str->buf + (str->len - strlen(suffix)), suffix) == 0;
}

bool string_contains(string_t* str, string_t* sub)
{
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

bool string_contains_cstr(string_t* str, const char* sub)
{
  size_t sub_len = strlen(sub);

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

void string_replace(string_t* str, size_t pos, size_t len, string_t* rep)
{
  string_replace_with_csubstr(str, pos, len, rep->buf, 0);
}

void string_replace_with_substr(string_t* str, size_t pos, size_t len, string_t* rep, size_t rep_pos)
{
  string_replace_with_csubstr(str, pos, len, rep->buf, rep_pos);
}

void string_replace_with_cstr(string_t* str, size_t pos, size_t len, const char* rep)
{
  string_replace_with_csubstr(str, pos, len, rep, 0);
}

void string_replace_with_csubstr(string_t* str, size_t pos, size_t len, const char* rep, size_t rep_pos)
{
  size_t rep_len = strlen(rep);

  assert(pos < str->len);

  if (rep_len > 0)
    assert(rep_pos < rep_len);
  else
    assert(rep_pos == 0);

  size_t new_len = str->len - len + rep_len - rep_pos;

  string_reserve(str, new_len + 1);

  memmove(str->buf + pos + rep_len - rep_pos, str->buf + pos + len, str->len - pos - len + 1);
  memcpy(str->buf + pos, rep + rep_pos, rep_len - rep_pos);

  str->len = new_len;
}
