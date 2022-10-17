#ifndef TAU_LOCATION_H
#define TAU_LOCATION_H

#include <stddef.h>
#include <stdint.h>

/// Struct containing location information within a source file. 
typedef struct location_s
{
  /// Path to the source file.
  const char* filepath;

  /// Pointer to the source string.
  const char* src;

  /// Pointer to the location in the source string.
  const char* cur;

  /// Location row (line) number in source string.
  size_t row;

  /// Location column number in source string.
  size_t col;

  /// Character length of the word.
  size_t len;
} location_t;

#endif