/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/lexer/location.h"

void tau_location_json_dump(tau_location_t loc, FILE* stream)
{
  fprintf(stream,
    "{\"path\":\"%s\",\"row\":\"%zu\",\"col\":\"%zu\",\"pos\":\"%zu\",\"len\":\"%zu\"}",
    loc.path, loc.row, loc.col, loc.ptr - loc.src, loc.len);
}

tau_string_t* tau_location_to_string(tau_location_t loc)
{
  return tau_string_init_with_cstr_and_length(loc.ptr, loc.len);
}

tau_string_view_t tau_location_to_string_view(tau_location_t loc)
{
  return tau_string_view_init_with_length(loc.ptr, loc.len);
}
