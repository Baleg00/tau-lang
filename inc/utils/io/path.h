/**
 * \file
 *
 * \brief File system path library.
 *
 * \details This file system path library provides functions for manipulating
 * paths on the file system. It includes functions to extract file names, extensions,
 * directories, and file stems from paths, as well as joining multiple path
 * components into a single path.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_PATH_H
#define TAU_PATH_H

#include <stdbool.h>
#include <stdint.h>

#include "os_detect.h"
#include "utils/extern_c.h"

/// Cross-platform directory separator character.
#define PATH_GENERIC_DIRECTORY_SEPARATOR_CHAR '/'

/// Cross-platform directory separator C-string.
#define PATH_GENERIC_DIRECTORY_SEPARATOR_CSTR "/"

#if defined(TAU_OS_WINDOWS)
/// Platform specific directory separator character.
# define PATH_NATIVE_DIRECTORY_SEPARATOR_CHAR '\\'

/// Platform specific directory separator C-string.
# define PATH_NATIVE_DIRECTORY_SEPARATOR_CSTR "\\"
#else
/// Platform specific directory separator character.
# define PATH_NATIVE_DIRECTORY_SEPARATOR_CHAR '/'

/// Platform specific directory separator C-string.
# define PATH_NATIVE_DIRECTORY_SEPARATOR_CSTR "/"
#endif

TAU_EXTERN_C_BEGIN

/**
 * \brief Represents a file system path.
 */
typedef struct path_t path_t;

/**
 * \brief Initializes a new empty path.
 *
 * \returns Pointer to the newly initialized path.
 */
path_t* path_init(void);

/**
 * \brief Initializes a new path using a C-string.
 *
 * \param[in] cstr C-string path to be used.
 * \returns Pointer to the newly initialized path.
 */
path_t* path_init_with_cstr(const char* cstr);

/**
 * \brief Initializes a new path using a C-string and its length.
 *
 * \param[in] cstr C-string path to be used.
 * \param[in] len The length of the C-string.
 * \returns Pointer to the newly initialized path.
 */
path_t* path_init_with_cstr_and_length(const char* cstr, size_t len);

/**
 * \brief Frees all memory associated with a path.
 *
 * \param[in] path Pointer to the path to be freed.
 */
void path_free(path_t* path);

/**
 * \brief Creates a copy of a path.
 *
 * \param[in] path Pointer to the path to be copied.
 * \returns Pointer to the newly initialized path copy.
 */
path_t* path_copy(path_t* path);

/**
 * \brief Replaces a path with another path.
 *
 * \param[in,out] path Pointer to the path to be replaced.
 * \param[in] other Pointer to the path to be used as replacement.
 */
void path_replace(path_t* path, path_t* other);

/**
 * \brief Replaces a path with a C-string.
 *
 * \param[in,out] path Pointer to the path to be replaced.
 * \param[in] cstr The C-string to be used as replacement.
 */
void path_replace_with_cstr(path_t* path, const char* cstr);

/**
 * \brief Clears the stored path string.
 *
 * \param[in,out] path Pointer to the path to be cleared.
 */
void path_clear(path_t* path);

/**
 * \brief Writes a path to a C-string buffer.
 *
 * \details If buf is NULL, the function calculates the required buffer size
 * without actually writing any characters.
 *
 * \param[in] path Pointer to the path to be used.
 * \param[out] buf Pointer to the output buffer where the path will be written.
 * \param[in] size The maximum number of characters to be written to the output buffer.
 * \returns The number of characters written to the output buffer.
 */
size_t path_to_cstr(path_t* path, char* buf, size_t size);

/**
 * \brief Checks whether a path is relative.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path is relative, `false` otherwise.
 */
bool path_is_relative(path_t* path);

/**
 * \brief Checks whether a path is absolute.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path is absolute, `false` otherwise.
 */
bool path_is_absolute(path_t* path);

/**
 * \brief Checks whether a path in string format is empty.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path in string format is empty, `false` otherwise.
 */
bool path_empty(path_t* path);

/**
 * \brief Joins two paths using a directory separator.
 *
 * \param[in] first Pointer to the first path to be used.
 * \param[in] second Pointer to the second path to be used.
 * \returns Pointer to the newly initialized joined path.
 */
path_t* path_join(path_t* first, path_t* second);

/**
 * \brief Joins two paths using a directory separator.
 *
 * \param[in] first Pointer to the first path to be used.
 * \param[in] cstr C-string to be used.
 * \returns Pointer to the newly initialized joined path.
 */
path_t* path_join_cstr(path_t* first, const char* cstr);

/**
 * \brief Appends a path to another using a directory separator.
 *
 * \param[in,out] path Pointer to the path to be appended to.
 * \param[in] other Pointer to the path to be appended.
 */
void path_append(path_t* path, path_t* other);

/**
 * \brief Appends a path to another using a directory separator.
 *
 * \param[in,out] path Pointer to the path to be appended to.
 * \param[in] cstr C-string to be appended.
 */
void path_append_cstr(path_t* path, const char* cstr);

/**
 * \brief Check whether the path has a filename component.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path has a filename component, `false` otherwise.
 */
bool path_has_filename(path_t* path);

/**
 * \brief Check whether the path has an extension component.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path has an extension component, `false` otherwise.
 */
bool path_has_extension(path_t* path);

/**
 * \brief Check whether the path has a parent directory.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path has a parent directory, `false` otherwise.
 */
bool path_has_parent(path_t* path);

/**
 * \brief Check whether the path has a stem component.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path has a stem component, `false` otherwise.
 */
bool path_has_stem(path_t* path);

/**
 * \brief Extracts the filename component of a path.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns Pointer to the filename component if present, NULL otherwise.
 */
path_t* path_filename(path_t* path);

/**
 * \brief Extracts the extension component of a path.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns Pointer to the extension component if present, NULL otherwise.
 */
path_t* path_extension(path_t* path);

/**
 * \brief Retrieves the path of the parent directory.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns Pointer to the path of the parent directory.
 */
path_t* path_parent(path_t* path);

/**
 * \brief Extracts the stem component of a path.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns Pointer to the stem component if present, NULL otherwise.
 */
path_t* path_stem(path_t* path);

/**
 * \brief Replaces the filename component in a path.
 *
 * \param[in] path Pointer to the path to be used.
 * \param[in] filename The filename to be used as replacement.
 * \returns Pointer to the new path with the replaced filename if present, NULL otherwise.
 */
path_t* path_replace_filename(path_t* path, const char* filename);

/**
 * \brief Replaces the extension component in a path.
 *
 * \param[in] path Pointer to the path to be used.
 * \param[in] extension The extension to be used as replacement.
 * \returns Pointer to the new path with the replaced extension if present, NULL otherwise.
 */
path_t* path_replace_extension(path_t* path, const char* extension);

/**
 * \brief Lexicographically compares two paths.
 *
 * \param[in] lhs Pointer to the left-hand path to be used.
 * \param[in] rhs Pointer to the right-hand path to be used.
 * \returns Negative value if lhs appears before rhs in lexicographical order, positive value if lhs appears
 * after rhs, zero if lhs and rhs compare equal.
 */
int path_compare(path_t* lhs, path_t* rhs);

TAU_EXTERN_C_END

#endif
