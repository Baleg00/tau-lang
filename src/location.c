#include "location.h"

#include <stdlib.h>

#include "util.h"
#include "memtrace.h"

location_t* location_init(const char *path, char* src, char* cur, size_t row, size_t col, size_t len)
{
  location_t* loc = (location_t*)malloc(sizeof(location_t));
  assert(loc != NULL);
  loc->path = path;
  loc->src = src;
  loc->cur = cur;
  loc->row = row;
  loc->col = col;
  loc->len = len;
  return loc;
}

location_t* location_copy(location_t* loc)
{
  return location_init(loc->path, loc->src, loc->cur, loc->row, loc->col, loc->len);
}

void location_free(location_t* loc)
{
  free(loc);
}
