#include "file.h"

#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "log.h"
#include "diagnostics.h"
#include "memtrace.h"

void file_read_to_string(char* path, size_t* len, char** src)
{
  FILE* file = fopen(path, "r");
  
  if (file == NULL)
    report_error_failed_to_open_file(path);

  fseek(file, 0, SEEK_END);
  size_t flen = (size_t)ftell(file);
  fseek(file, 0, SEEK_SET);

  char* fbuf = (char*)calloc(flen + 1, sizeof(char));
  assert(fbuf != NULL);

  fread(fbuf, sizeof(char), flen, file);

  fclose(file);

  if (len != NULL)
    *len = flen;

  *src = fbuf;
}
