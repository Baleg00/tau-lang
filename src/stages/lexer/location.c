/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/lexer/location.h"

void location_json_dump(location_t loc, FILE* stream)
{
  fprintf(stream,
    "{\"path\":\"%s\",\"row\":\"%zu\",\"col\":\"%zu\",\"pos\":\"%zu\",\"len\":\"%zu\"}",
    loc.path, loc.row, loc.col, loc.ptr - loc.src, loc.len);
}

string_t* location_to_string(location_t loc)
{
  return string_init_with_cstr_and_length(loc.ptr, loc.len);
}

string_view_t location_to_string_view(location_t loc)
{
  return string_view_init_with_length(loc.ptr, loc.len);
}
