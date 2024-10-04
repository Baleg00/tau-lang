/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/io/file.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/common.h"
#include "utils/memory/memtrace.h"

#ifdef TAU_OS_WINDOWS

#include <windows.h>

/**
 * \brief Retrieves the attributes of a file or directory.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns File or directory attributes if no error occurred, INVALID_FILE_ATTRIBUTES otherwise.
 */
static DWORD file_attributes(path_t* path)
{
  size_t path_len = path_to_cstr(path, NULL, 0);

  LPSTR path_cstr = (LPSTR)malloc((path_len + 1) * sizeof(CHAR));
  path_to_cstr(path, path_cstr, path_len + 1);

  DWORD attrs = GetFileAttributesA(path_cstr);

  free(path_cstr);

  return attrs;
}

/**
 * \brief Retrieves the type of a file.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns File type if no error occurred, FILE_TYPE_UNKNOWN otherwise.
 */
static DWORD file_type(path_t* path)
{
  size_t path_len = path_to_cstr(path, NULL, 0);

  LPSTR path_cstr = (LPSTR)malloc((path_len + 1) * sizeof(CHAR));
  path_to_cstr(path, path_cstr, path_len + 1);

  HANDLE handle = CreateFileA(
    path_cstr,
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  free(path_cstr);

  if (handle == INVALID_HANDLE_VALUE)
    return FILE_TYPE_UNKNOWN;

  DWORD type = GetFileType(handle);

  CloseHandle(handle);

  return type;
}

bool file_is_directory(path_t* path)
{
  DWORD attrs = file_attributes(path);

  if (attrs == INVALID_FILE_ATTRIBUTES)
    return false;

  return attrs & FILE_ATTRIBUTE_DIRECTORY;
}

bool file_is_regular_file(path_t* path)
{
  DWORD type = file_type(path);

  if (type == FILE_TYPE_UNKNOWN)
    return false;

  return type == FILE_TYPE_DISK;
}

bool file_is_block_file(path_t* UNUSED(path))
{
  return false;
}

bool file_is_character_file(path_t* path)
{
  DWORD type = file_type(path);

  if (type == FILE_TYPE_UNKNOWN)
    return false;

  return type == FILE_TYPE_CHAR;
}

bool file_is_pipe(path_t* path)
{
  DWORD type = file_type(path);

  if (type == FILE_TYPE_UNKNOWN)
    return false;

  return type == FILE_TYPE_PIPE;
}

bool file_is_socket(path_t* UNUSED(path))
{
  return false;
}

bool file_is_symlink(path_t* path)
{
  DWORD attrs = file_attributes(path);

  if (attrs == INVALID_FILE_ATTRIBUTES)
    return false;

  return attrs & FILE_ATTRIBUTE_REPARSE_POINT;
}

bool file_exists(path_t* path)
{
  DWORD attrs = file_attributes(path);

  return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

bool file_empty(path_t* path)
{
  return file_size(path) == 0;
}

size_t file_size(path_t* path)
{
  size_t path_len = path_to_cstr(path, NULL, 0);

  LPSTR path_cstr = (LPSTR)malloc((path_len + 1) * sizeof(CHAR));
  path_to_cstr(path, path_cstr, path_len + 1);

  HANDLE handle = CreateFileA(
    path_cstr,
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  free(path_cstr);

  if (handle == INVALID_HANDLE_VALUE)
    return 0;

  LARGE_INTEGER size = { .QuadPart = 0 };

  GetFileSizeEx(handle, &size);

  CloseHandle(handle);

  return (size_t)size.QuadPart;
}

size_t file_read(path_t* path, char* buf, size_t len)
{
  if (buf == NULL)
    return file_size(path);

  size_t path_len = path_to_cstr(path, NULL, 0);

  LPSTR path_cstr = (LPSTR)malloc((path_len + 1) * sizeof(CHAR));
  path_to_cstr(path, path_cstr, path_len + 1);

  HANDLE handle = CreateFileA(
    path_cstr,
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  free(path_cstr);

  if (handle == INVALID_HANDLE_VALUE)
    return 0;

  DWORD bytes_read = 0;

  if (!ReadFile(handle, buf, (DWORD)len, &bytes_read, NULL))
    return 0;

  buf[bytes_read] = '\0';

  return (size_t)bytes_read;
}

#else
# error "File operations are not implemented for operating system!"
#endif
