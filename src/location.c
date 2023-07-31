/**
 * \file location.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "location.h"

#include "memtrace.h"
#include "util.h"

struct location_s
{
  const char* path; // Path to the source file.
  const char* src; // Pointer to beginning of the source string.
  const char* ptr; // Pointer to marked location in the source string.
  size_t row; // The row number in the source file.
  size_t col; // The column number in the row.
  size_t len; // The character length of marked location.
};

location_t* location_init(const char* path, const char* src, const char* ptr, size_t row, size_t col, size_t len)
{
  location_t* loc = (location_t*)malloc(sizeof(location_t));
  assert(loc != NULL);

  loc->path = path;
  loc->src = src;
  loc->ptr = ptr;
  loc->row = row;
  loc->col = col;
  loc->len = len;

  return loc;
}

void location_free(location_t* loc)
{
  free(loc);
}

void location_json_dump(location_t* loc, FILE* stream)
{
  fprintf(stream,
    "{\"path\":\"%s\",\"row\":\"%zu\",\"col\":\"%zu\",\"pos\":\"%zu\",\"len\":\"%zu\"}",
    loc->path, loc->row, loc->col, (size_t)(loc->ptr - loc->src), loc->len);
}

const char* location_get_path(location_t* loc)
{
  return loc->path;
}

void location_set_path(location_t* loc, const char* path)
{
  loc->path = path;
}

const char* location_get_src(location_t* loc)
{
  return loc->src;
}

void location_set_src(location_t* loc, const char* src)
{
  loc->src = src;
}

const char* location_get_ptr(location_t* loc)
{
  return loc->ptr;
}

void location_set_ptr(location_t* loc, const char* ptr)
{
  loc->ptr = ptr;
}

size_t location_get_row(location_t* loc)
{
  return loc->row;
}

void location_set_row(location_t* loc, size_t row)
{
  loc->row = row;
}

size_t location_get_col(location_t* loc)
{
  return loc->col;
}

void location_set_col(location_t* loc, size_t col)
{
  loc->col = col;
}

size_t location_get_len(location_t* loc)
{
  return loc->len;
}

void location_set_len(location_t* loc, size_t len)
{
  loc->len = len;
}
