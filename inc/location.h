#ifndef TAU_LOCATION_H
#define TAU_LOCATION_H

#include <stddef.h>

#include "forward_decls.h"

struct location_s
{
  const char *path;
  char *src, *cur;
  size_t row, col, len;
};

location_t* location_init(void);
void location_free(location_t* loc);

#endif