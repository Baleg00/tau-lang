/**
 * \file
 * 
 * String manipulation library.
*/

#ifndef TAU_STR_H
#define TAU_STR_H

#include <stddef.h>
#include <stdbool.h>

/** Represents a string. */
typedef struct string_s 
{
  size_t cap; // Maxmimum capacity of buffer in characters.
  size_t len; // Length of string in characters (not including the terminating null character).
  char* buf; // Pointer to character buffer.
  bool user_buf; // Determines wether the character buffer was user provided.
} string_t;

/** Initializes a new empty string. */
string_t* string_init(void);

/**
 * \brief Initializes a new string from a c-string.
 * 
 * \param[in] cstr Pointer to c-string.
 * \returns New string.
*/
string_t* string_init_cstr(char* cstr);

/**
 * \brief Initializes a new string with the specified capacity.
 * 
 * \param[in] cap Initial capacity.
 * \returns New string.
*/
string_t* string_init_capacity(size_t cap);

/**
 * \brief Initializes a new string using the specified buffer and capacity.
 * 
 * \details The string's capacity cannot be changed and the buffer is not freed
 * when the string is destroyed.
 * 
 * \param[in] buf Pointer to character buffer to be used.
 * \param[in] cap Capacity of buffer.
 * \returns New string.
*/
string_t* string_init_buffer(char* buf, size_t cap);

/**
 * \brief Destroys a string.
 * 
 * \details The buffer is not freed if it was provided by the user.
 * 
 * \param[in] str String to be destroyed.
*/
void string_free(string_t* str);

/**
 * \brief Returns a pointer to the beginning of the string.
 * 
 * \param[in] str String to be used.
 * \returns Pointer to beginning of string.
*/
char* string_begin(string_t* str);

/**
 * \brief Returns a pointer to the end of the string.
 * 
 * \details The pointer always points at the terminating null character.
 * 
 * \param[in] str String to be used.
 * \returns Pointer to end of string.
*/
char* string_end(string_t* str);

/**
 * \brief Returns the character length of the string.
 * 
 * \details Does not include the terminating null character.
 * 
 * \param[in] str String to be used.
 * \returns Character length of string.
*/
size_t string_length(string_t* str);

/**
 * \brief Returns the character capacity of the string.
 * 
 * \param[in] str String to be used.
 * \returns Character capacity of string.
*/
size_t string_capacity(string_t* str);

/**
 * \brief Reserves space for a given number of characters.
 * 
 * \details Has no effect if `cap` is not greater than the string's capacity.
 * 
 * \param[in,out] str String to be used.
 * \param[in] cap Target capacity.
*/
void string_reserve(string_t* str, size_t cap);

/**
 * \brief Shrinks string's capacity to fit its contents.
 * 
 * \param[in,out] str String to be used.
*/
void string_fit(string_t* str);

/**
 * \brief Append a string to the end of another.
 * 
 * \param[in,out] str String to append to.
 * \param[in] other String to be appended.
*/
void string_append(string_t* str, string_t* other);

/**
 * \brief Create a new sub-string from another string.
 * 
 * \param[in] str String to be used.
 * \param[in] begin Index of first character in sub-string.
 * \param[in] len Length of sub-string.
 * \returns New sub-string.
*/
string_t* string_substr(string_t* str, size_t begin, size_t len);

/**
 * \brief Compares two strings lexicographically.
 * 
 * \param[in] lhs String to be compared.
 * \param[in] rhs String to be compared.
 * \returns 0 if strings are the same, negative value if `lhs` comes before,
 * and positive value if `lhs` comes after `rhs` lexicographically.
*/
int string_compare(string_t* lhs, string_t* rhs);

/**
 * \brief Escapes special characters in a string.
 * 
 * \details Special characters include: apostrophe ('), quotation mark ("),
 * horizontal tab (\t), vertical tab (\v), line feed (\n), carriage return (\r),
 * backslash (\), audible bell (\a), backspace (\b), form feed (\f)
 * 
 * \param[in] str String to be used.
 * \returns Escaped string.
*/
string_t* string_escape(string_t* str);

#endif