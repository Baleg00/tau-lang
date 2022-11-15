#include "location.h"

#include <stdlib.h>

#include "util.h"
#include "memtrace.h"

location_t* location_init(void)
{
  location_t* loc = (location_t*)malloc(sizeof(location_t));
  assert(loc != NULL);
  return loc;
}

void location_free(location_t* loc)
{
  free(loc);
}
