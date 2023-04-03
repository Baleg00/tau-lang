#include "str.h"

#include <stdlib.h>
#include <string.h>

#include "memtrace.h"
#include "util.h"
#include "log.h"

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
  str->user_buf = false;
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
  str->user_buf = true;
  return str;
}

void string_free(string_t* str)
{
  if (!str->user_buf)
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

  if (str->user_buf)
  {
    log_fatal("string", "Cannot expand user provided buffer.");
    exit(EXIT_FAILURE);
  }

  char* new_buf = (char*)realloc(str->buf, cap * sizeof(char));

  if (new_buf == NULL)
    log_error("string", "Failed to reserve space for string. Requested capacity: %zu", cap);
  else
  {
    str->cap = cap;
    str->buf = new_buf;
  }
}

void string_fit(string_t* str)
{
  if (str->user_buf)
  {
    log_error("string", "Cannot shrink user provided buffer.");
    return;
  }

  char* new_buf = (char*)realloc(str->buf, str->len + 1);

  if (new_buf == NULL)
    log_error("string", "Failed to fit string capacity.");
  else
  {
    str->cap = str->len + 1;
    str->buf = new_buf;
  }
}

void string_append(string_t* str, string_t* other)
{
  if (str->cap < str->len + other->len + 1)
    string_reserve(str, str->len + other->len + 1);

  str->len += other->len;
  strcat(str->buf, other->buf);
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
