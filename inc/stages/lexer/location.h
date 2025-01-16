/**
 * \file
 * 
 * \brief Source code location interface.
 * 
 * \details A source code locations tracks the locations within the source code
 * where errors or other important events occur. It provides a way to associate
 * positions, such as file paths, line numbers, and column numbers, with
 * specific locations in the source code. This information is crucial for error
 * reporting, debugging, and generating meaningful diagnostics. The source code
 * location helps the compiler accurately pinpoint the source of issues and
 * provides developers with actionable information to understand and resolve
 * them.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_LOCATION_H
#define TAU_LOCATION_H

#include "utils/common.h"
#include "utils/str.h"
#include "utils/str_view.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Location in the source code.
 */
typedef struct location_t
{
  const char* path; // Path to the source file.
  const char* src; // Pointer to beginning of the source string.
  const char* ptr; // Pointer to marked location in the source string.
  size_t row; // The row number in the source file.
  size_t col; // The column number in the row.
  size_t len; // The character length of marked location.
} location_t;

/**
 * \brief Dump the JSON representation of a location to a file stream.
 *
 * \param[in] loc Pointer to the location.
 * \param[in] stream Pointer to the file stream where the JSON dump will be
 * written.
 */
void location_json_dump(location_t loc, FILE* stream);

/**
 * \brief Creates a string from a location.
 * 
 * \param loc Pointer to the location.
 * \returns Pointer to the string.
 */
string_t* location_to_string(location_t loc);

/**
 * \brief Creates a string view of a location.
 * 
 * \param loc Pointer to the location.
 * \returns The string view.
 */
string_view_t location_to_string_view(location_t loc);

TAU_EXTERN_C_END

#endif
