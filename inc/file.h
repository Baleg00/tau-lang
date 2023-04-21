/**
 * \file
 * 
 * Filesystem and I/O library.
*/

#ifndef TAU_FILE_H
#define TAU_FILE_H

#include <stddef.h>

/**
 * \brief Reads file into string buffer.
 * 
 * \details If `buf` is null pointer then nothing is written
 * but the return value is still calculated.
 * 
 * \param[in] path Path to file.
 * \param[in,out] buf Pointer to string buffer to be written to or null pointer.
 * \param[in] len Maximum number of characters to be written.
 * \returns Number of characters that would have been written if `len` was ignored.
*/
size_t file_read(const char* path, char* buf, size_t len);

/** Returns the system specific directory separator character. */
char file_dirsep(void);

/**
 * \brief Extracts filename from path into string buffer.
 * 
 * \details If `buf` is null pointer then nothing is written
 * but the return value is still calculated.
 * 
 * \param[in] path Path to file.
 * \param[in] buf Pointer to string buffer to be written to or null pointer.
 * \param[in] len Maximum number of characters to be written.
 * \returns Number of characters that would have been written if `len` was ignored.
*/
size_t file_name(const char* path, char* buf, size_t len);

/**
 * \brief Extracts file extension from path into string buffer.
 * 
 * \details If `buf` is null pointer then nothing is written
 * but the return value is still calculated.
 * 
 * \param[in] path Path to file.
 * \param[in] buf Pointer to string buffer to be written to or null pointer.
 * \param[in] len Maximum number of characters to be written.
 * \returns Number of characters that would have been written if `len` was ignored.
*/
size_t file_ext(const char* path, char* buf, size_t len);

/**
 * \brief Extracts file parent directory from path into string buffer.
 * 
 * \details If `buf` is null pointer then nothing is written
 * but the return value is still calculated.
 * 
 * \param[in] path Path to file.
 * \param[in] buf Pointer to string buffer to be written to or null pointer.
 * \param[in] len Maximum number of characters to be written.
 * \returns Number of characters that would have been written if `len` was ignored.
*/
size_t file_dir(const char* path, char* buf, size_t len);

/**
 * \brief Extracts file stem from path into string buffer.
 * 
 * \details If `buf` is null pointer then nothing is written
 * but the return value is still calculated.
 * 
 * \param[in] path Path to file.
 * \param[in] buf Pointer to string buffer to be written to or null pointer.
 * \param[in] len Maximum number of characters to be written.
 * \returns Number of characters that would have been written if `len` was ignored.
*/
size_t file_stem(const char* path, char* buf, size_t len);

#endif