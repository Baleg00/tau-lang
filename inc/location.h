/**
 * \file
 * 
 * Location in source file.
*/

#ifndef TAU_LOCATION_H
#define TAU_LOCATION_H

#include <stddef.h>

#include "typedefs.h"

struct location_s
{
  const char *path; // Path to source file.
  char* src; // Pointer to beginning of source string.
  char* cur; // Pointer to marked location in source string.
  size_t row; // Row number in source file.
  size_t col; // Column number in row.
  size_t len; // Character length of marked location.
};

/**
 * \brief Initializes a location instance.
 * 
 * \param[out] loc Location instance to be initialized.
 * \param[in] path Path to source file.
 * \param[in] src Pointer to beginning of source string.
 * \param[in] cur Pointer to marked location in source string.
 * \param[in] row Row number in source file.
 * \param[in] col Column number in row.
 * \param[in] len Character length of marked location.
*/
void location_init(location_t* loc, const char *path, char* src, char* cur, size_t row, size_t col, size_t len);

/**
 * \brief Destroys a location.
 * 
 * \param[in] loc Location to be destroyed.
*/
void location_free(location_t* loc);

#endif