/**
 * \file
 * 
 * \brief File system utility library.
 * 
 * \details This file system utility library provides functions for common file
 * system operations. It includes functions to read file contents, identify
 * various file types and check if a file exists or is empty.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_FILE_H
#define TAU_FILE_H

#include <stdbool.h>
#include <stddef.h>

#include "utils/io/path.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Checks whether a path refers to a directory.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path refers to a directory, `false` otherwise.
 */
bool tau_file_is_directory(tau_path_t* path);

/**
 * \brief Checks whether a path refers to a regular file.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path refers to a regular file, `false` otherwise.
 */
bool tau_file_is_regular_file(tau_path_t* path);

/**
 * \brief Checks whether a path refers to a block file.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path refers to a block file, `false` otherwise.
 */
bool tau_file_is_block_file(tau_path_t* path);

/**
 * \brief Checks whether a path refers to a character file.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path refers to a character file, `false` otherwise.
 */
bool tau_file_is_character_file(tau_path_t* path);

/**
 * \brief Checks whether a path refers to a named pipe.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path refers to a named pipe, `false` otherwise.
 */
bool tau_file_is_pipe(tau_path_t* path);

/**
 * \brief Checks whether a path refers to a named IPC socket.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path refers to a named IPC socket, `false` otherwise.
 */
bool tau_file_is_socket(tau_path_t* path);

/**
 * \brief Checks whether a path refers to a symbolic link.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path refers to a symbolic link, `false` otherwise.
 */
bool tau_file_is_symlink(tau_path_t* path);

/**
 * \brief Checks whether a path refers to an existing file system object.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path refers to an existing file system object, `false` otherwise.
 */
bool tau_file_exists(tau_path_t* path);

/**
 * \brief Checks whether a path refers to an empty file.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns `true` if the path refers to an empty file, `false` otherwise.
 */
bool tau_file_empty(tau_path_t* path);

/**
 * \brief Retrieves the size of a file in bytes.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns The size of the file in bytes if path refers to a file, 0 otherwise.
 */
size_t tau_file_size(tau_path_t* path);

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
size_t tau_file_read(tau_path_t* path, char* buf, size_t len);

TAU_EXTERN_C_END

#endif
