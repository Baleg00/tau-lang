#include "file.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
#include "log.h"
#include "diagnostics.h"
#include "memtrace.h"

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

char file_dirsep(void)
{
  static const char* file = __FILE__;
  static char sep = '\0';

  if (sep == '\0')
    sep = strchr(file, '\\') != NULL ? '\\' : '/';
  
  return sep;
}

size_t file_name(const char* path, char* buf, size_t len)
{
  char* sep = strrchr(path, file_dirsep());

  if (sep == NULL || sep == path)
    return 0;

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
  char* sep = strrchr(path, file_dirsep());

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
  char* sep = strrchr(path, file_dirsep());

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
