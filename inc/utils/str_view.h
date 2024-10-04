/**
 * \file
 * 
 * \brief String view utility library interface.
 * 
 * \details The string view library provides a lightweight abstraction for
 * representing and working with strings without copying the underlying data.
 * It defines a type that stores a reference to existing character data rather
 * than duplicating it. This approach offers benefits in terms of reduced memory
 * usage and improved performance since it avoids unnecessary string copying.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_STR_VIEW_H
#define TAU_STR_VIEW_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "utils/extern_c.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Represents a lightweight, readonly, non-owning string.
 */
typedef struct string_view_t
{
  size_t len; // Length of the string (excluding the terminating null character).
  const char* buf; // Pointer to character buffer.
} string_view_t;

/**
 * \brief Initializes a new string view.
 * 
 * \param[in] buf Pointer to the character buffer to be used.
 * \returns Pointer to the newly initialized string view.
 */
string_view_t string_view_init(const char* buf);

/**
 * \brief Initializes a new string view.
 * 
 * \param[in] buf Pointer to the character buffer to be used.
 * \param[in] len Number of characters in the buffer.
 * \returns Pointer to the newly initialized string view.
 */
string_view_t string_view_init_with_length(const char* buf, size_t len);

/**
 * \brief Returns a pointer to the beginning of a string view.
 * 
 * \param[in] str The string view.
 * \returns Pointer to beginning of the string view.
 */
const char* string_view_begin(string_view_t str);

/**
 * \brief Returns a pointer to the end of a string view.
 * 
 * \param[in] str The string view.
 * \returns Pointer to end of the string view.
*/
const char* string_view_end(string_view_t str);

/**
 * \brief Returns the length of a string view.
 * 
 * \not The terminating null character is not included in the length.
 * 
 * \param[in] str The string view.
 * \returns The length of the string view.
*/
size_t string_view_length(string_view_t str);

/**
 * \brief Creates a new sub-string from another string view.
 * 
 * \param[in] str The string view.
 * \param[in] pos The index of the first character in the sub-string.
 * \param[in] len The length of the sub-string.
 * \returns Pointer to the new sub-string.
*/
string_view_t string_view_substr(string_view_t str, size_t pos, size_t len);

/**
 * \brief Compares two string views lexicographically.
 * 
 * \param[in] lhs The string view to be compared.
 * \param[in] rhs The string view to be compared.
 * \returns Zero if the strings views are the same, a negative value if `lhs`
 * comes before `rhs`, and a positive value if `lhs` comes after `rhs`.
*/
int string_view_compare(string_view_t lhs, string_view_t rhs);

/**
 * \brief Compares a string view and a c-string lexicographically.
 * 
 * \param[in] lhs The string view to be compared.
 * \param[in] rhs Pointer to the c-string to be compared.
 * \returns Zero if the strings are the same, a negative value if `lhs` comes
 * before `rhs`, and a positive value if `lhs` comes after `rhs`.
*/
int string_view_compare_cstr(string_view_t lhs, const char* rhs);

/**
 * \brief Checks if a string view starts with the specified prefix.
 * 
 * \param str The string view.
 * \param prefix The prefix.
 * \returns `true` if the string view starts with the specified prefix, `false`
 * otherwise.
 */
bool string_view_starts_with(string_view_t str, string_view_t prefix);

/**
 * \brief Checks if a string view starts with the specified c-string prefix.
 * 
 * \param str The string view.
 * \param prefix Pointer to the c-string prefix.
 * \returns `true` if the string view starts with the specified prefix, `false`
 * otherwise.
 */
bool string_view_starts_with_cstr(string_view_t str, const char* prefix);

/**
 * \brief Checks if a string view ends with the specified suffix.
 * 
 * \param str The string view.
 * \param suffix The suffix.
 * \returns `true` if the string view ends with the specified suffix, `false`
 * otherwise.
 */
bool string_view_ends_with(string_view_t str, string_view_t suffix);

/**
 * \brief Checks if a string view ends with the specified c-string suffix.
 * 
 * \param str The string view.
 * \param suffix Pointer to the c-string suffix.
 * \returns `true` if the string view ends with the specified suffix, `false`
 * otherwise.
 */
bool string_view_ends_with_cstr(string_view_t str, const char* suffix);

/**
 * \brief Checks if a string view contains the specified sub-string.
 * 
 * \param str The string view.
 * \param sub The sub-string.
 * \returns `true` if the string view contains the specified sub-string, `false`
 * otherwise.
 */
bool string_view_contains(string_view_t str, string_view_t sub);

/**
 * \brief Checks if a string view contains the specified c-sub-string.
 * 
 * \param str The string view.
 * \param sub Pointer to the c-sub-string.
 * \returns `true` if the string view contains the specified sub-string, `false`
 * otherwise.
 */
bool string_view_contains_cstr(string_view_t str, const char* sub);

/**
 * \brief Finds the first occurrence of a sub-string.
 * 
 * \param str The string view.
 * \param sub The sub-string.
 * \returns The position of the first occurrence of the sub-string.
 */
size_t string_view_find(string_view_t str, string_view_t sub);

/**
 * \brief Finds the first occurrence of a c-string.
 * 
 * \param str The string view.
 * \param sub Pointer to the c-string.
 * \returns The position of the first occurrence of the c-string.
 */
size_t string_view_find_cstr(string_view_t str, const char* sub);

/**
 * \brief Uses a string view as a format specifier and writes to a stream.
 * 
 * \param stream Pointer to the stream.
 * \param fmt The format string view.
 * \param ... Arguments according to the format string view.
 * \returns The number of characters written.
 */
int string_view_printf(FILE* stream, string_view_t fmt, ...);

/**
 * \brief Writes the escaped version of a string view to a stream.
 * 
 * \param stream Pointer to the stream.
 * \param str The string view to be printed.
 * \returns The number of characters written.
 */
int string_view_print_escaped(FILE* stream, string_view_t str);

TAU_EXTERN_C_END

#endif
