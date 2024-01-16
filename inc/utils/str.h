/**
 * \file str.h
 * 
 * \brief String utility library interface.
 * 
 * \details The string utility library is a collection of functions and
 * utilities designed to manipulate strings. The library provides a wide range
 * of operations, such as copying, concatenating, and searching strings, making
 * it easier for programmers to work with textual data. It also handles memory
 * management, ensuring that string operations are safe and efficient. The string
 * library simplifies common string-related tasks, reduces the risk of
 * memory-related errors, and improve code readability and maintainability.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_STR_H
#define TAU_STR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * \brief Represents a string.
 */
typedef struct string_t string_t;

/**
 * \brief Initializes a new empty string.
 * 
 * \returns Pointer to the newly initialized string.
 */
string_t* string_init(void);

/**
 * \brief Initializes a new string from a c-string.
 * 
 * \param[in] cstr Pointer to the c-string.
 * \returns Pointer to the newly initalized string.
 */
string_t* string_init_with_cstr(const char* cstr);

/**
 * \brief Initializes a new string from a c-string with a specified length.
 * 
 * \param[in] cstr Pointer to the c-string.
 * \param[in] len The length of the string.
 * \returns Pointer to the newly initalized string.
 */
string_t* string_init_with_cstr_and_length(const char* cstr, size_t len);

/**
 * \brief Initializes a new string with the specified capacity.
 * 
 * \param[in] cap Initial capacity.
 * \returns Pointer to the newly initalized string.
 */
string_t* string_init_with_capacity(size_t cap);

/**
 * \brief Initializes a new string using the specified buffer and capacity.
 * 
 * \note The string does not take ownership of the provided buffer. Its capacity
 * will not be changed and it will not be freed when the string is destroyed.
 * 
 * \param[in] buf Pointer to the character buffer to be used.
 * \param[in] cap The capacity of the buffer.
 * \returns Pointer to the newly initalized string.
 */
string_t* string_init_with_buffer(char* buf, size_t cap);

/**
 * \brief Frees all resources associated with a string.
 * 
 * \param[in] str Pointer to the string to be destroyed.
 */
void string_free(string_t* str);

/**
 * \brief Returns a pointer to the beginning of a string.
 * 
 * \param[in] str Pointer to the string.
 * \returns Pointer to beginning of the string.
 */
char* string_begin(string_t* str);

/**
 * \brief Returns a pointer to the end of a string.
 * 
 * \param[in] str Pointer to the string.
 * \returns Pointer to end of the string.
*/
char* string_end(string_t* str);

/**
 * \brief Returns the length of a string.
 * 
 * \not The terminating null character is not included in the length.
 * 
 * \param[in] str Pointer to the string.
 * \returns The length of the string.
*/
size_t string_length(string_t* str);

/**
 * \brief Returns the capacity of the string.
 * 
 * \param[in] str Pointer to the string.
 * \returns The capacity of the string.
*/
size_t string_capacity(string_t* str);

/**
 * \brief Reserves space for a given number of characters.
 * 
 * \param[in,out] str Pointer to the string.
 * \param[in] cap The target capacity.
*/
void string_reserve(string_t* str, size_t cap);

/**
 * \brief Shrinks a string's capacity to fit its contents.
 * 
 * \param[in,out] str Pointer to the string.
*/
void string_fit(string_t* str);

/**
 * \brief Uses a string as a format specifier and writes to a stream.
 * 
 * \param stream Pointer to the stream.
 * \param fmt Pointer to the format string.
 * \param ... Arguments according to the format string.
 * \returns The number of characters written.
 */
int string_printf(FILE* stream, string_t* fmt, ...);

/**
 * \brief Appends a string to the end of another.
 * 
 * \param[in,out] str Pointer to the string to append to.
 * \param[in] other Pointer to the string to be appended.
*/
void string_append(string_t* str, string_t* other);

/**
 * \brief Appends a c-string to the end of a string.
 * 
 * \param[in,out] str Pointer to the string to append to.
 * \param[in] other Pointer to the c-string to be appended.
*/
void string_append_cstr(string_t* str, const char* other);

/**
 * \brief Inserts a string into another string.
 * 
 * \param[in,out] str Pointer to the string to insert into.
 * \param[in] pos Position of the insertion.
 * \param[in] other Pointer to the string to be inserted.
*/
void string_insert(string_t* str, size_t pos, string_t* other);

/**
 * \brief Inserts a c-string into a string.
 * 
 * \param[in,out] str Pointer to the string to insert into.
 * \param[in] pos Position of the insertion.
 * \param[in] other Pointer to the c-string to be inserted.
*/
void string_insert_cstr(string_t* str, size_t pos, const char* other);

/**
 * \brief Erases a range of characters from a string.
 * 
 * \param[in,out] str Pointer to the string to erase from.
 * \param[in] pos Position to erase from.
 * \param[in] len Number of characters to erase.
*/
void string_erase(string_t* str, size_t pos, size_t len);

/**
 * \brief Clears the contents of a string.
 * 
 * \param[in,out] str Pointer to the string.
*/
void string_clear(string_t* str);

/**
 * \brief Creates a copy of a string.
 * 
 * \param[in] str Pointer to the string.
 * \returns Pointer to the copy.
*/
string_t* string_copy(string_t* str);

/**
 * \brief Creates a new sub-string from another string.
 * 
 * \param[in] str Pointer to the string.
 * \param[in] begin The index of the first character in the sub-string.
 * \param[in] len The length of the sub-string.
 * \returns Pointer to the new sub-string.
*/
string_t* string_substr(string_t* str, size_t begin, size_t len);

/**
 * \brief Compares two strings lexicographically.
 * 
 * \param[in] lhs Pointer to the string to be compared.
 * \param[in] rhs Pointer to the string to be compared.
 * \returns Zero if the strings are the same, a negative value if `lhs` comes
 * before `rhs`, and a positive value if `lhs` comes after `rhs`.
*/
int string_compare(string_t* lhs, string_t* rhs);

/**
 * \brief Compares a string and a c-string lexicographically.
 * 
 * \param[in] lhs Pointer to the string to be compared.
 * \param[in] rhs Pointer to the c-string to be compared.
 * \returns Zero if the strings are the same, a negative value if `lhs` comes
 * before `rhs`, and a positive value if `lhs` comes after `rhs`.
*/
int string_compare_cstr(string_t* lhs, const char* rhs);

/**
 * \brief Escapes special characters in a string.
 * 
 * \note Special characters: apostrophe (`'`), quotation mark (`"`), horizontal
 * tab (`\\t`), vertical tab (`\\v`), line feed (`\\n`), carriage return (`\\r`),
 * backslash (`\\`), audible bell (`\\a`), backspace (`\\b`), form feed (`\\f`)
 * 
 * \param[in] str Pointer to the string.
 * \returns Pointer to the new escaped string.
*/
string_t* string_escape(string_t* str);

/**
 * \brief Checks if a string starts with the specified prefix.
 * 
 * \param str Pointer to the string.
 * \param prefix Pointer to the prefix.
 * \returns `true` if the string starts with the specified prefix, `false`
 * otherwise.
 */
bool string_starts_with(string_t* str, string_t* prefix);

/**
 * \brief Checks if a string starts with the specified c-string prefix.
 * 
 * \param str Pointer to the string.
 * \param prefix Pointer to the c-string prefix.
 * \returns `true` if the string starts with the specified prefix, `false`
 * otherwise.
 */
bool string_starts_with_cstr(string_t* str, const char* prefix);

/**
 * \brief Checks if a string ends with the specified suffix.
 * 
 * \param str Pointer to the string.
 * \param suffix Pointer to the suffix.
 * \returns `true` if the string ends with the specified suffix, `false`
 * otherwise.
 */
bool string_ends_with(string_t* str, string_t* suffix);

/**
 * \brief Checks if a string ends with the specified c-string suffix.
 * 
 * \param str Pointer to the string.
 * \param suffix Pointer to the c-string suffix.
 * \returns `true` if the string ends with the specified suffix, `false`
 * otherwise.
 */
bool string_ends_with_cstr(string_t* str, const char* suffix);

/**
 * \brief Checks if a string contains the specified sub-string.
 * 
 * \param str Pointer to the string.
 * \param sub Pointer to the sub-string.
 * \returns `true` if the string contains the specified sub-string, `false`
 * otherwise.
 */
bool string_contains(string_t* str, string_t* sub);

/**
 * \brief Checks if a string contains the specified c-string.
 * 
 * \param str Pointer to the string.
 * \param sub Pointer to the c-string.
 * \returns `true` if the string contains the specified sub-string, `false`
 * otherwise.
 */
bool string_contains_cstr(string_t* str, const char* sub);

/**
 * \brief Replaces the specified range of a string with a replacement string.
 * 
 * \param str Pointer to the string.
 * \param pos Start of the sub-string to be replaced.
 * \param len Length of the sub-string to be replaced.
 * \param rep Pointer to the string to be used for replacement.
 */
void string_replace(string_t* str, size_t pos, size_t len, string_t* rep);

/**
 * \brief Replaces the specified range of a string with a sub-string of a
 * replacement string.
 * 
 * \param str Pointer to the string.
 * \param pos Start of the sub-string to be replaced.
 * \param len Length of the sub-string to be replaced.
 * \param rep Pointer to the string to be used for replacement.
 * \param rep_pos Start of the sub-string in the replacement string.
 */
void string_replace_with_substr(string_t* str, size_t pos, size_t len, string_t* rep, size_t rep_pos);

/**
 * \brief Replaces the specified range of a string with a replacement c-string.
 * 
 * \param str Pointer to the string.
 * \param pos Start of the sub-string to be replaced.
 * \param len Length of the sub-string to be replaced.
 * \param rep Pointer to the c-string to be used for replacement.
 */
void string_replace_with_cstr(string_t* str, size_t pos, size_t len, const char* rep);

/**
 * \brief Replaces the specified range of a string with a sub-string of a
 * replacement c-string.
 * 
 * \param str Pointer to the string.
 * \param pos Start of the sub-string to be replaced.
 * \param len Length of the sub-string to be replaced.
 * \param rep Pointer to the c-string to be used for replacement.
 * \param rep_pos Start of the sub-string in the replacement c-string.
 */
void string_replace_with_csubstr(string_t* str, size_t pos, size_t len, const char* rep, size_t rep_pos);

/**
 * \brief Finds the first occurrence of a sub-string.
 * 
 * \param str Pointer to the string.
 * \param sub Pointer to the sub-string.
 * \returns The position of the first occurrence of the sub-string.
 */
size_t string_find(string_t* str, string_t* sub);

/**
 * \brief Finds the first occurrence of a c-string.
 * 
 * \param str Pointer to the string.
 * \param sub Pointer to the c-string.
 * \returns The position of the first occurrence of the c-string.
 */
size_t string_find_cstr(string_t* str, const char* sub);

#endif