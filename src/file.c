/**
 * \file file.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "file.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "diagnostics.h"
#include "log.h"
#include "memtrace.h"
#include "util.h"

size_t file_read(const char* path, char* buf, size_t len)
{
  FILE* file = fopen(path, "r");
  
  if (file == NULL)
    report_error_failed_to_open_file(path);

  fseek(file, 0, SEEK_END);
  size_t flen = (size_t)ftell(file);
  rewind(file);

  if (buf != NULL)
  {
    flen = fread(buf, sizeof(char), min(len, flen), file);
    memset(buf + flen, '\0', (len - flen) * sizeof(char));
  }
  
  fclose(file);

  return flen;
}

size_t file_name(const char* path, char* buf, size_t len)
{
  char* sep = strrchr(path, FILE_DIRSEP);

  if (sep == NULL)
    sep = (char*)path - 1;

  size_t name_len = strlen(sep + 1);

  if (buf != NULL)
  {
    strncpy(buf, sep + 1, len);

    if (name_len < len)
      buf[name_len] = '\0';
  }

  return name_len;
}

size_t file_ext(const char* path, char* buf, size_t len)
{
  char* dot = strrchr(path, '.');

  if (dot == NULL || dot == path)
    return 0;

  size_t ext_len = strlen(dot + 1);

  if (buf != NULL)
  {
    strncpy(buf, dot + 1, len);

    if (ext_len < len)
      buf[ext_len] = '\0';
  }

  return ext_len;
}

size_t file_dir(const char* path, char* buf, size_t len)
{
  char* sep = strrchr(path, FILE_DIRSEP);

  if (sep == NULL)
    return 0;

  size_t dir_len = max(sep - path, 1);

  if (buf != NULL)
  {
    strncpy(buf, path, min(len, dir_len));

    if (dir_len < len)
      buf[dir_len] = '\0';
  }

  return dir_len;
}

size_t file_stem(const char* path, char* buf, size_t len)
{
  char* dot = strrchr(path, '.');
  char* sep = strrchr(path, FILE_DIRSEP);

  if (dot == NULL || dot == path ||
      sep == NULL || sep == path)
    return 0;

  size_t stem_len = dot - sep - 1;

  if (stem_len == 0)
  {
    if (buf != NULL)
      strncpy(buf, dot, len);

    return strlen(dot);
  }

  if (buf != NULL)
  {
    strncpy(buf, sep + 1, min(len, stem_len));

    if (stem_len < len)
      buf[stem_len] = '\0';
  }

  return stem_len;
}

size_t file_join(char* buf, size_t len, size_t count, ...)
{
  if (count == 0)
    return 0;

  size_t result_len = 0;

  va_list paths;
  va_start(paths, count);
  const char* path;

  for (size_t i = 0; i < count && len > 0; ++i)
  {
    path = va_arg(paths, char*);
    size_t path_len = strlen(path);

    result_len += path_len;

    if (i + 1 < count)
      ++result_len;

    if (buf != NULL)
    {
      strncpy(buf, path, min(len, path_len));
      buf += path_len;
      len -= min(len, path_len);

      if (i + 1 < count)
      {
        *(buf++) = FILE_DIRSEP;
        --len;
      }
    }
  }

  if (buf != NULL && len > 0)
    *buf = '\0';

  return result_len;
}
