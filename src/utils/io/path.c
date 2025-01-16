/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/io/path.h"

#include "utils/common.h"

/**
 * \brief Checks whether a character is a directory separator.
 *
 * \param[in] ch The character to be used.
 * \returns `true` if the character is a native or generic directory separator, `false` otherwise.
 */
static bool path_is_directory_separator(char ch)
{
  return ch == PATH_GENERIC_DIRECTORY_SEPARATOR_CHAR || ch == PATH_NATIVE_DIRECTORY_SEPARATOR_CHAR;
}

path_t* path_init(void)
{
  return (path_t*)string_init();
}

path_t* path_init_with_cstr(const char* cstr)
{
  return (path_t*)string_init_with_cstr(cstr);
}

path_t* path_init_with_cstr_and_length(const char* cstr, size_t len)
{
  return (path_t*)string_init_with_cstr_and_length(cstr, len);
}

void path_free(path_t* path)
{
  string_free((string_t*)path);
}

path_t* path_copy(path_t* path)
{
  return (path_t*)string_copy((string_t*)path);
}

void path_replace(path_t* path, path_t* other)
{
  string_replace((string_t*)path, 0, string_length((string_t*)path), (string_t*)other);
}

void path_replace_with_cstr(path_t* path, const char* cstr)
{
  string_replace_with_cstr((string_t*)path, 0, string_length((string_t*)path), cstr);
}

void path_clear(path_t* path)
{
  string_clear((string_t*)path);
}

size_t path_to_cstr(path_t* path, char* buf, size_t size)
{
  string_t* str = (string_t*)path;

  if (buf == NULL)
    return string_length(str);

  size_t min_len = MIN(string_length(str) + 1, size);
  strncpy(buf, string_begin(str), min_len);

  return min_len;
}

string_t* path_to_string(path_t* path)
{
  return string_copy((string_t*)path);
}

string_view_t path_to_string_view(path_t* path)
{
  string_t* str = (string_t*)path;

  return string_view_init_with_length(string_begin(str), string_length(str));
}

bool path_is_relative(path_t* path)
{
  return !path_is_absolute(path);
}

bool path_is_absolute(path_t* path)
{
  if (path_empty(path))
    return false;

  string_t* str = (string_t*)path;
  const char* str_ptr = string_begin(str);

  // Unix-like absolute path (e.g. /usr/local/bin).
  if (path_is_directory_separator(str_ptr[0]))
    return true;

  size_t str_len = string_length(str);

  // Windows absolute path starting with drive letter (e.g. C:\).
  if (str_len >= 3 && isalpha(str_ptr[0]) && str_ptr[1] == ':' && path_is_directory_separator(str_ptr[2]))
    return true;

  // Windows UNC path (e.g. \\server\share).
  if (str_len >= 2 && path_is_directory_separator(str_ptr[0]) && path_is_directory_separator(str_ptr[1]))
    return true;

  return false;
}

bool path_empty(path_t* path)
{
  return string_length((string_t*)path) == 0;
}

path_t* path_join(path_t* first, path_t* second)
{
  if (path_empty(first))
    return path_copy(second);

  if (path_empty(second))
    return path_copy(first);

  return path_join_cstr(first, string_begin((string_t*)second));
}

path_t* path_join_cstr(path_t* first, const char* cstr)
{
  path_t* result = path_copy(first);

  string_t* str = (string_t*)result;

  string_append_cstr(str, PATH_GENERIC_DIRECTORY_SEPARATOR_CSTR);
  string_append_cstr(str, cstr);

  return result;
}

void path_append(path_t* path, path_t* other)
{
  if (path_empty(other))
    return;

  path_append_cstr(path, string_begin((string_t*)other));
}

void path_append_cstr(path_t* path, const char* cstr)
{
  string_append_cstr((string_t*)path, cstr);
}

bool path_has_filename(path_t* path)
{
  if (path_empty(path))
    return false;

  string_t* str = (string_t*)path;
  const char* str_ptr = string_begin(str);
  size_t str_len = string_length(str);

  if (path_is_directory_separator(str_ptr[str_len - 1]))
    return false;

  return true;
}

bool path_has_extension(path_t* path)
{
  if (path_empty(path))
    return false;

  string_t* str = (string_t*)path;
  const char* str_ptr = string_begin(str);
  size_t str_len = string_length(str);

  if (path_is_directory_separator(str_ptr[str_len - 1]))
    return false;

  size_t last_dot = str_len;

  while (last_dot > 0 && str_ptr[last_dot - 1] != '.')
    --last_dot;

  if (last_dot <= 1 || last_dot == str_len)
    return false;

  return str_ptr[last_dot - 2] != '.' && !path_is_directory_separator(str_ptr[last_dot - 2]);
}

bool path_has_parent(path_t* path)
{
  if (path_empty(path))
    return false;

  string_t* str = (string_t*)path;
  const char* str_ptr = string_begin(str);
  size_t str_len = string_length(str);

  size_t last_sep = str_len;

  while (last_sep > 0 && path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  while (last_sep > 0 && !path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  if (last_sep == 0)
    return false;

  return true;
}

bool path_has_stem(path_t* path)
{
  if (path_empty(path))
    return false;

  string_t* str = (string_t*)path;
  const char* str_ptr = string_begin(str);
  size_t str_len = string_length(str);

  if (path_is_directory_separator(str_ptr[str_len - 1]))
    return false;

  size_t last_dot_or_sep = str_len;

  while (last_dot_or_sep > 0 && str_ptr[last_dot_or_sep - 1] != '.' && !path_is_directory_separator(str_ptr[last_dot_or_sep - 1]))
    --last_dot_or_sep;

  if (last_dot_or_sep == 0)
    return false;

  if (path_is_directory_separator(str_ptr[last_dot_or_sep - 1]))
    return false;

  return true;
}

path_t* path_filename(path_t* path)
{
  if (path_empty(path))
    return path_init();

  string_t* str = (string_t*)path;
  const char* str_ptr = string_begin(str);
  size_t str_len = string_length(str);

  if (path_is_directory_separator(str_ptr[str_len - 1]))
    return path_init();

  size_t last_sep = str_len;

  while (last_sep > 0 && !path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  return path_init_with_cstr(str_ptr + last_sep);
}

path_t* path_extension(path_t* path)
{
  if (path_empty(path))
    return path_init();

  string_t* str = (string_t*)path;
  const char* str_ptr = string_begin(str);
  size_t str_len = string_length(str);

  if (path_is_directory_separator(str_ptr[str_len - 1]))
    return path_init();

  size_t last_dot = str_len;

  while (last_dot > 0 && str_ptr[last_dot - 1] != '.')
    --last_dot;

  if (last_dot <= 1 || last_dot == str_len)
    return path_init();

  if (str_ptr[last_dot - 2] == '.' || path_is_directory_separator(str_ptr[last_dot - 2]))
    return path_init();

  return path_init_with_cstr(str_ptr + last_dot);
}

path_t* path_parent(path_t* path)
{
  if (path_empty(path))
    return path_init();

  string_t* str = (string_t*)path;
  const char* str_ptr = string_begin(str);
  size_t str_len = string_length(str);

  size_t last_sep = str_len;

  while (last_sep > 0 && path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  while (last_sep > 0 && !path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  if (last_sep == 0)
    return path_init();

  return path_init_with_cstr_and_length(str_ptr, last_sep);
}

path_t* path_stem(path_t* path)
{
  if (path_empty(path))
    return path_init();

  string_t* str = (string_t*)path;
  const char* str_ptr = string_begin(str);
  size_t str_len = string_length(str);

  if (path_is_directory_separator(str_ptr[str_len - 1]))
    return path_init();

  size_t last_dot_or_sep = str_len;

  while (last_dot_or_sep > 0 && str_ptr[last_dot_or_sep - 1] != '.' && !path_is_directory_separator(str_ptr[last_dot_or_sep - 1]))
    --last_dot_or_sep;

  if (last_dot_or_sep == 0)
    return path_init();

  if (path_is_directory_separator(str_ptr[last_dot_or_sep - 1]))
    return path_init();

  size_t last_dot = last_dot_or_sep;
  size_t last_sep = last_dot_or_sep;

  while (last_sep > 0 && !path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  if (last_sep + 1 == last_dot)
    return path_init_with_cstr(str_ptr + last_dot);

  return path_init_with_cstr_and_length(str_ptr + last_sep + 1, last_dot - last_sep - 1);
}

path_t* path_replace_filename(path_t* path, const char* filename)
{
  if (path_empty(path))
    return path_init_with_cstr(filename);

  string_t* str = (string_t*)path;
  const char* str_ptr = string_begin(str);
  size_t str_len = string_length(str);

  if (path_is_directory_separator(str_ptr[str_len - 1]))
  {
    path_t* result = path_copy(path);

    path_append_cstr(result, filename);

    return result;
  }

  size_t last_sep = str_len;

  while (last_sep > 0 && !path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  path_t* result = path_init_with_cstr_and_length(str_ptr, last_sep);

  path_append_cstr(result, filename);

  return result;
}

path_t* path_replace_extension(path_t* path, const char* extension)
{
  if (path_empty(path))
  {
    path_t* result = path_init();

    path_append_cstr(result, ".");
    path_append_cstr(result, extension);

    return result;
  }

  string_t* str = (string_t*)path;
  const char* str_ptr = string_begin(str);
  size_t str_len = string_length(str);

  if (path_is_directory_separator(str_ptr[str_len - 1]))
  {
    path_t* result = path_copy(path);

    path_append_cstr(result, ".");
    path_append_cstr(result, extension);

    return result;
  }

  size_t last_dot_or_sep = str_len;

  while (last_dot_or_sep > 0 && str_ptr[last_dot_or_sep - 1] != '.' && !path_is_directory_separator(str_ptr[last_dot_or_sep - 1]))
    --last_dot_or_sep;

  if (last_dot_or_sep == 0)
  {
    path_t* result = path_copy(path);

    path_append_cstr(result, ".");
    path_append_cstr(result, extension);

    return result;
  }

  if (str_ptr[last_dot_or_sep - 1] == '.')
  {
    path_t* result = path_init_with_cstr_and_length(str_ptr, last_dot_or_sep);

    path_append_cstr(result, extension);

    return result;
  }

  path_t* result = path_init_with_cstr_and_length(str_ptr, last_dot_or_sep);

  path_append_cstr(result, ".");
  path_append_cstr(result, extension);

  return result;
}

int path_compare(path_t* lhs, path_t* rhs)
{
  return string_compare((string_t*)lhs, (string_t*)rhs);
}
