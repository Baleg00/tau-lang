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
static DWORD tau_file_attributes(tau_path_t* path)
{
  tau_string_t* tau_path_str = tau_path_to_string(path);

  DWORD attrs = GetFileAttributesA(tau_string_begin(tau_path_str));

  tau_string_free(tau_path_str);

  return attrs;
}

/**
 * \brief Retrieves the type of a file.
 *
 * \param[in] path Pointer to the path to be used.
 * \returns File type if no error occurred, FILE_TYPE_UNKNOWN otherwise.
 */
static DWORD tau_file_type(tau_path_t* path)
{
  tau_string_t* tau_path_str = tau_path_to_string(path);

  HANDLE handle = CreateFileA(
    tau_string_begin(tau_path_str),
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  tau_string_free(tau_path_str);

  if (handle == INVALID_HANDLE_VALUE)
    return FILE_TYPE_UNKNOWN;

  DWORD type = GetFileType(handle);

  CloseHandle(handle);

  return type;
}

bool tau_file_is_directory(tau_path_t* path)
{
  DWORD attrs = tau_file_attributes(path);

  if (attrs == INVALID_FILE_ATTRIBUTES)
    return false;

  return attrs & FILE_ATTRIBUTE_DIRECTORY;
}

bool tau_file_is_regular_file(tau_path_t* path)
{
  DWORD type = tau_file_type(path);

  if (type == FILE_TYPE_UNKNOWN)
    return false;

  return type == FILE_TYPE_DISK;
}

bool tau_file_is_block_file(tau_path_t* TAU_UNUSED(path))
{
  return false;
}

bool tau_file_is_character_file(tau_path_t* path)
{
  DWORD type = tau_file_type(path);

  if (type == FILE_TYPE_UNKNOWN)
    return false;

  return type == FILE_TYPE_CHAR;
}

bool tau_file_is_pipe(tau_path_t* path)
{
  DWORD type = tau_file_type(path);

  if (type == FILE_TYPE_UNKNOWN)
    return false;

  return type == FILE_TYPE_PIPE;
}

bool tau_file_is_socket(tau_path_t* TAU_UNUSED(path))
{
  return false;
}

bool tau_file_is_symlink(tau_path_t* path)
{
  DWORD attrs = tau_file_attributes(path);

  if (attrs == INVALID_FILE_ATTRIBUTES)
    return false;

  return attrs & FILE_ATTRIBUTE_REPARSE_POINT;
}

bool tau_file_exists(tau_path_t* path)
{
  DWORD attrs = tau_file_attributes(path);

  return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

bool tau_file_empty(tau_path_t* path)
{
  return tau_file_size(path) == 0;
}

size_t tau_file_size(tau_path_t* path)
{
  tau_string_t* tau_path_str = tau_path_to_string(path);

  HANDLE handle = CreateFileA(
    tau_string_begin(tau_path_str),
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  tau_string_free(tau_path_str);

  if (handle == INVALID_HANDLE_VALUE)
    return 0;

  LARGE_INTEGER size = { .QuadPart = 0 };

  GetFileSizeEx(handle, &size);

  CloseHandle(handle);

  return (size_t)size.QuadPart;
}

size_t tau_file_read(tau_path_t* path, char* buf, size_t len)
{
  if (buf == NULL)
    return tau_file_size(path);

  tau_string_t* tau_path_str = tau_path_to_string(path);

  HANDLE handle = CreateFileA(
    tau_string_begin(tau_path_str),
    GENERIC_READ,
    FILE_SHARE_READ,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  tau_string_free(tau_path_str);

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

bool tau_file_is_directory(tau_path_t *path)
{
  tau_string_view_t tau_path_str_view = tau_path_to_string_view(path);
  const char *tau_path_cstr = tau_string_view_begin(tau_path_str_view);

  struct stat st;

  return stat(tau_path_cstr, &st) == 0 && S_ISDIR(st.st_mode);
}

bool tau_file_is_regular_file(tau_path_t *path)
{
  tau_string_view_t tau_path_str_view = tau_path_to_string_view(path);
  const char *tau_path_cstr = tau_string_view_begin(tau_path_str_view);

  struct stat st;

  return stat(tau_path_cstr, &st) == 0 && S_ISREG(st.st_mode);
}

bool tau_file_is_block_file(tau_path_t *path)
{
  tau_string_view_t tau_path_str_view = tau_path_to_string_view(path);
  const char *tau_path_cstr = tau_string_view_begin(tau_path_str_view);

  struct stat st;

  return stat(tau_path_cstr, &st) == 0 && S_ISBLK(st.st_mode);
}

bool tau_file_is_character_file(tau_path_t *path)
{
  tau_string_view_t tau_path_str_view = tau_path_to_string_view(path);
  const char *tau_path_cstr = tau_string_view_begin(tau_path_str_view);

  struct stat st;

  return stat(tau_path_cstr, &st) == 0 && S_ISCHR(st.st_mode);
}

bool tau_file_is_pipe(tau_path_t *path)
{
  tau_string_view_t tau_path_str_view = tau_path_to_string_view(path);
  const char* tau_path_cstr = tau_string_view_begin(tau_path_str_view);

  struct stat st;

  return stat(tau_path_cstr, &st) == 0 && S_ISFIFO(st.st_mode);
}

bool tau_file_is_socket(tau_path_t *path)
{
  tau_string_view_t tau_path_str_view = tau_path_to_string_view(path);
  const char* tau_path_cstr = tau_string_view_begin(tau_path_str_view);

  struct stat st;

  return stat(tau_path_cstr, &st) == 0 && S_ISSOCK(st.st_mode);
}

bool tau_file_is_symlink(tau_path_t *path)
{
  tau_string_view_t tau_path_str_view = tau_path_to_string_view(path);
  const char* tau_path_cstr = tau_string_view_begin(tau_path_str_view);

  struct stat st;

  return lstat(tau_path_cstr, &st) == 0 && S_ISLNK(st.st_mode);
}

bool tau_file_exists(tau_path_t *path)
{
  tau_string_view_t tau_path_str_view = tau_path_to_string_view(path);
  const char* tau_path_cstr = tau_string_view_begin(tau_path_str_view);

  return access(tau_path_cstr, F_OK) == 0;
}

bool tau_file_empty(tau_path_t *path)
{
  tau_string_view_t tau_path_str_view = tau_path_to_string_view(path);
  const char* tau_path_cstr = tau_string_view_begin(tau_path_str_view);

  struct stat st;

  return stat(tau_path_cstr, &st) == 0 && (st.st_size == 0);
}

size_t tau_file_size(tau_path_t *path)
{
  tau_string_view_t tau_path_str_view = tau_path_to_string_view(path);
  const char* tau_path_cstr = tau_string_view_begin(tau_path_str_view);

  struct stat st;

  return stat(tau_path_cstr, &st) == 0 ? st.st_size : 0;
}

size_t tau_file_read(tau_path_t *path, char *buf, size_t len)
{
  tau_string_view_t tau_path_str_view = tau_path_to_string_view(path);
  const char* tau_path_cstr = tau_string_view_begin(tau_path_str_view);

  int fd = open(tau_path_cstr, O_RDONLY);

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
