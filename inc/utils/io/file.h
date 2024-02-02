/**
 * \file
 * 
 * \brief File system utility interface.
 * 
 * \details This file system utility library provides functions for common file
 * system operations. It includes functions to read file contents, extract file
 * names, extensions, directories, and file stems from paths, as well as
 * joining multiple path components into a single path.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_FILE_H
#define TAU_FILE_H

#include <stddef.h>

#if defined(_WIN32)
/**
 * \brief System specific directory separator character.
 */
# define FILE_DIRSEP '\\'
#elif defined(__linux__) || defined(__unix__)
/**
 * \brief System specific directory separator character.
 */
# define FILE_DIRSEP '/'
#else
/**
 * \brief System specific directory separator character.
 */
# define FILE_DIRSEP '/'
#endif

/**
 * \brief Reads the contents of a file and stores it in the provided buffer.
 *
 * \details If buf is NULL, the function calculates the required buffer size
 * without actually writing any data.
 * 
 * \param[in] path The path to the file.
 * \param[out] buf Pointer to the buffer where the file contents will be stored.
 * \param[in] len The length of the buffer.
 * \returns The number of characters written into the buffer, excluding the
 * null-terminator.
 */
size_t file_read(const char* path, char* buf, size_t len);

/**
 * \brief Extracts the file name from the given path and stores it in the
 * provided buffer.
 *
 * \details If buf is NULL, the function calculates the required buffer size
 * without actually writing any data.
 * 
 * \param[in] path The path to the file.
 * \param[out] buf Pointer to the buffer where the file name will be stored.
 * \param[in] len The length of the buffer.
 * \returns The number of characters written into the buffer, excluding the
 * null-terminator.
 */
size_t file_name(const char* path, char* buf, size_t len);

/**
 * \brief Extracts the file extension from the given path and stores it in the
 * provided buffer.
 *
 * \details If buf is NULL, the function calculates the required buffer size
 * without actually writing any data.
 * 
 * \param[in] path The path to the file.
 * \param[out] buf Pointer to the buffer where the file extension will be stored.
 * \param[in] len The length of the buffer.
 * \returns The number of characters written into the buffer, excluding the
 * null-terminator.
 */
size_t file_ext(const char* path, char* buf, size_t len);

/**
 * \brief Extracts the directory from the given path and stores it in the
 * provided buffer.
 *
 * \details If buf is NULL, the function calculates the required buffer size
 * without actually writing any data.
 * 
 * \param[in] path The path to the file.
 * \param[out] buf Pointer to the buffer where the directory will be stored.
 * \param[in] len The length of the buffer.
 * \returns The number of characters written into the buffer, excluding the
 * null-terminator.
 */
size_t file_dir(const char* path, char* buf, size_t len);

/**
 * \brief Extracts the file stem (filename without extension) from the given
 * path and stores it in the provided buffer.
 *
 * \details If buf is NULL, the function calculates the required buffer size
 * without actually writing any data.
 * 
 * \param[in] path The path to the file.
 * \param[out] buf Pointer to the buffer where the file stem will be stored.
 * \param[in] len The length of the buffer.
 * \returns The number of characters written into the buffer, excluding the
 * null-terminator.
 */
size_t file_stem(const char* path, char* buf, size_t len);

/**
 * \brief Joins multiple path components into a single path and stores it in
 * the provided buffer.
 *
 * \details If buf is NULL, the function calculates the required buffer size
 * without actually writing any data.
 * 
 * \param[out] buf Pointer to the buffer where the joined path will be stored.
 * \param[in] len The length of the buffer.
 * \param[in] count The number of path components to join.
 * \param[in] ... Variable number of path components to join.
 * \returns The number of characters written into the buffer, excluding the
 * null-terminator.
 */
size_t file_join(char* buf, size_t len, size_t count, ...);

#endif