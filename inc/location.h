/**
 * \file location.h
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

#include <stddef.h>
#include <stdio.h>

#include "str_view.h"
#include "str.h"

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
 * \brief Initializes a location.
 * 
 * \param[in] path The path to the source file.
 * \param[in] src Pointer to the source string.
 * \param[in] ptr Pointer to the marked location in the source string.
 * \param[in] row The row number in the source file.
 * \param[in] col The column number in the row.
 * \param[in] len The character length of marked location.
 * \returns Pointer to the newly initialized location.
*/
location_t* location_init(const char *path, const char* src, const char* ptr, size_t row, size_t col, size_t len);

/**
 * \brief Frees a location.
 * 
 * \param[in] loc Pointer to the location.
*/
void location_free(location_t* loc);

/**
 * \brief Dump the JSON representation of a location to a file stream.
 *
 * \param[in] loc Pointer to the location.
 * \param[in] stream Pointer to the file stream where the JSON dump will be
 * written.
 */
void location_json_dump(location_t* loc, FILE* stream);

/**
 * \brief Creates a string from a location.
 * 
 * \param loc Pointer to the location.
 * \returns Pointer to the string.
 */
string_t* location_to_string(location_t* loc);

/**
 * \brief Creates a string view of a location.
 * 
 * \param loc Pointer to the location.
 * \returns The string view.
 */
string_view_t location_to_string_view(location_t* loc);

#endif