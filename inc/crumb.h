#ifndef TAU_CRUMB_H
#define TAU_CRUMB_H

#include <stdio.h>
#include <stdbool.h>

#include "location.h"

typedef enum crumb_kind_e
{
  CRUMB_WARN,
  CRUMB_ERROR
} crumb_kind_t;

void crumb_log(crumb_kind_t kind, bool show_sur_lines, const location_t* loc, const char* fmt, ...);

void crumb_set_file(FILE* file);
FILE* crumb_get_file(void);

const char* crumb_kind_to_color(crumb_kind_t kind);

#define crumb_warn(...)   crumb_log(CRUMB_WARN  , __VA_ARGS__)
#define crumb_error(...)  crumb_log(CRUMB_ERROR , __VA_ARGS__)

#endif