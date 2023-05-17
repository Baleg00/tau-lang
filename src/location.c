#include "location.h"

void location_init(location_t* loc, const char *path, char* src, char* cur, size_t row, size_t col, size_t len)
{
  loc->path = path;
  loc->src = src;
  loc->cur = cur;
  loc->row = row;
  loc->col = col;
  loc->len = len;
}

void location_free(location_t* loc)
{
}
