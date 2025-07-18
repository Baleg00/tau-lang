/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/io/file.h"

#include "utils/common.h"

#if TAU_OS_WINDOWS

#include <windows.h>

/**
 * \brief Retrieves the attributes of a file or directory.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns File or directory attributes if no error occurred, INVALID_FILE_ATTRIBUTES otherwise.
 */
static DWORD file_attributes(path_t* path)
{
  string_t* path_str = path_to_string(path);

  DWORD attrs = GetFileAttributesA(string_begin(path_str));

  string_free(path_str);

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
  string_t* path_str = path_to_string(path);

  HANDLE handle = CreateFileA(
    string_begin(path_str),
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  string_free(path_str);

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
  string_t* path_str = path_to_string(path);

  HANDLE handle = CreateFileA(
    string_begin(path_str),
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  string_free(path_str);

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

  string_t* path_str = path_to_string(path);

  HANDLE handle = CreateFileA(
    string_begin(path_str),
    GENERIC_READ,
    FILE_SHARE_READ,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  string_free(path_str);

  if (handle == INVALID_HANDLE_VALUE)
    return 0;

  DWORD bytes_read = 0;

  if (!ReadFile(handle, buf, (DWORD)len, &bytes_read, NULL))
    return 0;

  buf[bytes_read] = '\0';

  return (size_t)bytes_read;
}

#elif TAU_OS_LINUX

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

bool file_is_directory(path_t *path)
{
  string_view_t path_str_view = path_to_string_view(path);
  const char *path_cstr = string_view_begin(path_str_view);

  struct stat st;

  return stat(path_cstr, &st) == 0 && S_ISDIR(st.st_mode);
}

bool file_is_regular_file(path_t *path)
{
  string_view_t path_str_view = path_to_string_view(path);
  const char *path_cstr = string_view_begin(path_str_view);

  struct stat st;

  return stat(path_cstr, &st) == 0 && S_ISREG(st.st_mode);
}

bool file_is_block_file(path_t *path)
{
  string_view_t path_str_view = path_to_string_view(path);
  const char *path_cstr = string_view_begin(path_str_view);

  struct stat st;

  return stat(path_cstr, &st) == 0 && S_ISBLK(st.st_mode);
}

bool file_is_character_file(path_t *path)
{
  string_view_t path_str_view = path_to_string_view(path);
  const char *path_cstr = string_view_begin(path_str_view);

  struct stat st;

  return stat(path_cstr, &st) == 0 && S_ISCHR(st.st_mode);
}

bool file_is_pipe(path_t *path)
{
  string_view_t path_str_view = path_to_string_view(path);
  const char* path_cstr = string_view_begin(path_str_view);

  struct stat st;

  return stat(path_cstr, &st) == 0 && S_ISFIFO(st.st_mode);
}

bool file_is_socket(path_t *path)
{
  string_view_t path_str_view = path_to_string_view(path);
  const char* path_cstr = string_view_begin(path_str_view);

  struct stat st;

  return stat(path_cstr, &st) == 0 && S_ISSOCK(st.st_mode);
}

bool file_is_symlink(path_t *path)
{
  string_view_t path_str_view = path_to_string_view(path);
  const char* path_cstr = string_view_begin(path_str_view);

  struct stat st;

  return lstat(path_cstr, &st) == 0 && S_ISLNK(st.st_mode);
}

bool file_exists(path_t *path)
{
  string_view_t path_str_view = path_to_string_view(path);
  const char* path_cstr = string_view_begin(path_str_view);

  return access(path_cstr, F_OK) == 0;
}

bool file_empty(path_t *path)
{
  string_view_t path_str_view = path_to_string_view(path);
  const char* path_cstr = string_view_begin(path_str_view);

  struct stat st;

  return stat(path_cstr, &st) == 0 && (st.st_size == 0);
}

size_t file_size(path_t *path)
{
  string_view_t path_str_view = path_to_string_view(path);
  const char* path_cstr = string_view_begin(path_str_view);

  struct stat st;

  return stat(path_cstr, &st) == 0 ? st.st_size : 0;
}

size_t file_read(path_t *path, char *buf, size_t len)
{
  string_view_t path_str_view = path_to_string_view(path);
  const char* path_cstr = string_view_begin(path_str_view);

  int fd = open(path_cstr, O_RDONLY);

  if (fd == -1)
    return 0;

  size_t bytes_read = 0;

  if (buf)
  {
    bytes_read = read(fd, buf, len);
  }
  else
  {
    struct stat st;

    if (fstat(fd, &st) == 0)
      bytes_read = st.st_size;
  }

  close(fd);

  return bytes_read;
}

#else
# error "File operations are not implemented for operating system!"
#endif
