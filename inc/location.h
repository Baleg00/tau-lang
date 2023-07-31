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

/**
 * \brief Location in the source code.
 */
typedef struct location_s location_t;

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
 * \brief Retrieves the file path associated with a location.
 *
 * \param[in] loc Pointer to the location.
 * \returns Pointer to the path string.
 */
const char* location_get_path(location_t* loc);

/**
 * \brief Sets the file path associated with a location.
 *
 * \param[out] loc Pointer to the location.
 * \param[in] path Pointer to the path string.
 */
void location_set_path(location_t* loc, const char* path);

/**
 * \brief Retrieves the source code associated with a location.
 *
 * \param[in] loc Pointer to the location.
 * \returns Pointer to the source code string.
 */
const char* location_get_src(location_t* loc);

/**
 * \brief Sets the source code associated with a location.
 *
 * \param[out] loc Pointer to the location.
 * \param[in] src Pointer to the source code string.
 */
void location_set_src(location_t* loc, const char* src);

/**
 * \brief Retrieves a pointer to a location.
 *
 * \param[in] loc Pointer to the location.
 * \returns Pointer to the current character string.
 */
const char* location_get_ptr(location_t* loc);

/**
 * \brief Sets the pointer to a location.
 *
 * \param[out] loc Pointer to the location.
 * \param[in] ptr Pointer to the current character string.
 */
void location_set_ptr(location_t* loc, const char* ptr);

/**
 * \brief Retrieves the row number associated with a location.
 *
 * \param[in] loc Pointer to the location.
 * \returns The row number.
 */
size_t location_get_row(location_t* loc);

/**
 * \brief Sets the row number associated with a location.
 *
 * \param[out] loc Pointer to the location.
 * \param[in] row The row number.
 */
void location_set_row(location_t* loc, size_t row);

/**
 * \brief Retrieves the column number associated with a location.
 *
 * \param[in] loc Pointer to the location.
 * \returns The column number.
 */
size_t location_get_col(location_t* loc);

/**
 * \brief Sets the column number associated with a location.
 *
 * \param[out] loc Pointer to the location.
 * \param[in] col The column number.
 */
void location_set_col(location_t* loc, size_t col);

/**
 * \brief Retrieves the length associated with a location.
 *
 * \param[in] loc Pointer to the location.
 * \returns The length.
 */
size_t location_get_len(location_t* loc);

/**
 * \brief Sets the length associated with a location.
 *
 * \param[out] loc Pointer to the location.
 * \param[in] len The length.
 */
void location_set_len(location_t* loc, size_t len);

#endif