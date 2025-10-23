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
static bool tau_path_is_directory_separator(char ch)
{
  return ch == TAU_PATH_GENERIC_DIRECTORY_SEPARATOR_CHAR || ch == TAU_PATH_NATIVE_DIRECTORY_SEPARATOR_CHAR;
}

tau_path_t* tau_path_init(void)
{
  return (tau_path_t*)tau_string_init();
}

tau_path_t* tau_path_init_with_cstr(const char* cstr)
{
  return (tau_path_t*)tau_string_init_with_cstr(cstr);
}

tau_path_t* tau_path_init_with_cstr_and_length(const char* cstr, size_t len)
{
  return (tau_path_t*)tau_string_init_with_cstr_and_length(cstr, len);
}

void tau_path_free(tau_path_t* path)
{
  tau_string_free((tau_string_t*)path);
}

tau_path_t* tau_path_copy(tau_path_t* path)
{
  return (tau_path_t*)tau_string_copy((tau_string_t*)path);
}

void tau_path_replace(tau_path_t* path, tau_path_t* other)
{
  tau_string_replace((tau_string_t*)path, 0, tau_string_length((tau_string_t*)path), (tau_string_t*)other);
}

void tau_path_replace_with_cstr(tau_path_t* path, const char* cstr)
{
  tau_string_replace_with_cstr((tau_string_t*)path, 0, tau_string_length((tau_string_t*)path), cstr);
}

void tau_path_clear(tau_path_t* path)
{
  tau_string_clear((tau_string_t*)path);
}

size_t tau_path_to_cstr(tau_path_t* path, char* buf, size_t size)
{
  tau_string_t* str = (tau_string_t*)path;

  if (buf == NULL)
    return tau_string_length(str);

  size_t min_len = TAU_MIN(tau_string_length(str) + 1, size);
  strncpy(buf, tau_string_begin(str), min_len);

  return min_len;
}

tau_string_t* tau_path_to_string(tau_path_t* path)
{
  return tau_string_copy((tau_string_t*)path);
}

tau_string_view_t tau_path_to_string_view(tau_path_t* path)
{
  tau_string_t* str = (tau_string_t*)path;

  return tau_string_view_init_with_length(tau_string_begin(str), tau_string_length(str));
}

bool tau_path_is_relative(tau_path_t* path)
{
  return !tau_path_is_absolute(path);
}

bool tau_path_is_absolute(tau_path_t* path)
{
  if (tau_path_empty(path))
    return false;

  tau_string_t* str = (tau_string_t*)path;
  const char* str_ptr = tau_string_begin(str);

  // Unix-like absolute path (e.g. /usr/local/bin).
  if (tau_path_is_directory_separator(str_ptr[0]))
    return true;

  size_t str_len = tau_string_length(str);

  // Windows absolute path starting with drive letter (e.g. C:\).
  if (str_len >= 3 && isalpha(str_ptr[0]) && str_ptr[1] == ':' && tau_path_is_directory_separator(str_ptr[2]))
    return true;

  // Windows UNC path (e.g. \\server\share).
  if (str_len >= 2 && tau_path_is_directory_separator(str_ptr[0]) && tau_path_is_directory_separator(str_ptr[1]))
    return true;

  return false;
}

bool tau_path_empty(tau_path_t* path)
{
  return tau_string_length((tau_string_t*)path) == 0;
}

tau_path_t* tau_path_join(tau_path_t* first, tau_path_t* second)
{
  if (tau_path_empty(first))
    return tau_path_copy(second);

  if (tau_path_empty(second))
    return tau_path_copy(first);

  return tau_path_join_cstr(first, tau_string_begin((tau_string_t*)second));
}

tau_path_t* tau_path_join_cstr(tau_path_t* first, const char* cstr)
{
  tau_path_t* result = tau_path_copy(first);

  tau_string_t* str = (tau_string_t*)result;

  tau_string_append_cstr(str, TAU_PATH_GENERIC_DIRECTORY_SEPARATOR_CSTR);
  tau_string_append_cstr(str, cstr);

  return result;
}

void tau_path_append(tau_path_t* path, tau_path_t* other)
{
  if (tau_path_empty(other))
    return;

  tau_path_append_cstr(path, tau_string_begin((tau_string_t*)other));
}

void tau_path_append_cstr(tau_path_t* path, const char* cstr)
{
  tau_string_append_cstr((tau_string_t*)path, cstr);
}

bool tau_path_has_filename(tau_path_t* path)
{
  if (tau_path_empty(path))
    return false;

  tau_string_t* str = (tau_string_t*)path;
  const char* str_ptr = tau_string_begin(str);
  size_t str_len = tau_string_length(str);

  if (tau_path_is_directory_separator(str_ptr[str_len - 1]))
    return false;

  return true;
}

bool tau_path_has_extension(tau_path_t* path)
{
  if (tau_path_empty(path))
    return false;

  tau_string_t* str = (tau_string_t*)path;
  const char* str_ptr = tau_string_begin(str);
  size_t str_len = tau_string_length(str);

  if (tau_path_is_directory_separator(str_ptr[str_len - 1]))
    return false;

  size_t last_dot = str_len;

  while (last_dot > 0 && str_ptr[last_dot - 1] != '.')
    --last_dot;

  if (last_dot <= 1 || last_dot == str_len)
    return false;

  return str_ptr[last_dot - 2] != '.' && !tau_path_is_directory_separator(str_ptr[last_dot - 2]);
}

bool tau_path_has_parent(tau_path_t* path)
{
  if (tau_path_empty(path))
    return false;

  tau_string_t* str = (tau_string_t*)path;
  const char* str_ptr = tau_string_begin(str);
  size_t str_len = tau_string_length(str);

  size_t last_sep = str_len;

  while (last_sep > 0 && tau_path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  while (last_sep > 0 && !tau_path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  if (last_sep == 0)
    return false;

  return true;
}

bool tau_path_has_stem(tau_path_t* path)
{
  if (tau_path_empty(path))
    return false;

  tau_string_t* str = (tau_string_t*)path;
  const char* str_ptr = tau_string_begin(str);
  size_t str_len = tau_string_length(str);

  if (tau_path_is_directory_separator(str_ptr[str_len - 1]))
    return false;

  size_t last_dot_or_sep = str_len;

  while (last_dot_or_sep > 0 && str_ptr[last_dot_or_sep - 1] != '.' && !tau_path_is_directory_separator(str_ptr[last_dot_or_sep - 1]))
    --last_dot_or_sep;

  if (last_dot_or_sep == 0)
    return false;

  if (tau_path_is_directory_separator(str_ptr[last_dot_or_sep - 1]))
    return false;

  return true;
}

tau_path_t* tau_path_filename(tau_path_t* path)
{
  if (tau_path_empty(path))
    return tau_path_init();

  tau_string_t* str = (tau_string_t*)path;
  const char* str_ptr = tau_string_begin(str);
  size_t str_len = tau_string_length(str);

  if (tau_path_is_directory_separator(str_ptr[str_len - 1]))
    return tau_path_init();

  size_t last_sep = str_len;

  while (last_sep > 0 && !tau_path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  return tau_path_init_with_cstr(str_ptr + last_sep);
}

tau_path_t* tau_path_extension(tau_path_t* path)
{
  if (tau_path_empty(path))
    return tau_path_init();

  tau_string_t* str = (tau_string_t*)path;
  const char* str_ptr = tau_string_begin(str);
  size_t str_len = tau_string_length(str);

  if (tau_path_is_directory_separator(str_ptr[str_len - 1]))
    return tau_path_init();

  size_t last_dot = str_len;

  while (last_dot > 0 && str_ptr[last_dot - 1] != '.')
    --last_dot;

  if (last_dot <= 1 || last_dot == str_len)
    return tau_path_init();

  if (str_ptr[last_dot - 2] == '.' || tau_path_is_directory_separator(str_ptr[last_dot - 2]))
    return tau_path_init();

  return tau_path_init_with_cstr(str_ptr + last_dot);
}

tau_path_t* tau_path_parent(tau_path_t* path)
{
  if (tau_path_empty(path))
    return tau_path_init();

  tau_string_t* str = (tau_string_t*)path;
  const char* str_ptr = tau_string_begin(str);
  size_t str_len = tau_string_length(str);

  size_t last_sep = str_len;

  while (last_sep > 0 && tau_path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  while (last_sep > 0 && !tau_path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  if (last_sep == 0)
    return tau_path_init();

  return tau_path_init_with_cstr_and_length(str_ptr, last_sep);
}

tau_path_t* tau_path_stem(tau_path_t* path)
{
  if (tau_path_empty(path))
    return tau_path_init();

  tau_string_t* str = (tau_string_t*)path;
  const char* str_ptr = tau_string_begin(str);
  size_t str_len = tau_string_length(str);

  if (tau_path_is_directory_separator(str_ptr[str_len - 1]))
    return tau_path_init();

  size_t last_dot_or_sep = str_len;

  while (last_dot_or_sep > 0 && str_ptr[last_dot_or_sep - 1] != '.' && !tau_path_is_directory_separator(str_ptr[last_dot_or_sep - 1]))
    --last_dot_or_sep;

  if (last_dot_or_sep == 0)
    return tau_path_init();

  if (tau_path_is_directory_separator(str_ptr[last_dot_or_sep - 1]))
    return tau_path_init();

  size_t last_dot = last_dot_or_sep;
  size_t last_sep = last_dot_or_sep;

  while (last_sep > 0 && !tau_path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  if (last_sep + 1 == last_dot)
    return tau_path_init_with_cstr(str_ptr + last_dot);

  return tau_path_init_with_cstr_and_length(str_ptr + last_sep + 1, last_dot - last_sep - 1);
}

tau_path_t* tau_path_replace_filename(tau_path_t* path, const char* filename)
{
  if (tau_path_empty(path))
    return tau_path_init_with_cstr(filename);

  tau_string_t* str = (tau_string_t*)path;
  const char* str_ptr = tau_string_begin(str);
  size_t str_len = tau_string_length(str);

  if (tau_path_is_directory_separator(str_ptr[str_len - 1]))
  {
    tau_path_t* result = tau_path_copy(path);

    tau_path_append_cstr(result, filename);

    return result;
  }

  size_t last_sep = str_len;

  while (last_sep > 0 && !tau_path_is_directory_separator(str_ptr[last_sep - 1]))
    --last_sep;

  tau_path_t* result = tau_path_init_with_cstr_and_length(str_ptr, last_sep);

  tau_path_append_cstr(result, filename);

  return result;
}

tau_path_t* tau_path_replace_extension(tau_path_t* path, const char* extension)
{
  if (tau_path_empty(path))
  {
    tau_path_t* result = tau_path_init();

    tau_path_append_cstr(result, ".");
    tau_path_append_cstr(result, extension);

    return result;
  }

  tau_string_t* str = (tau_string_t*)path;
  const char* str_ptr = tau_string_begin(str);
  size_t str_len = tau_string_length(str);

  if (tau_path_is_directory_separator(str_ptr[str_len - 1]))
  {
    tau_path_t* result = tau_path_copy(path);

    tau_path_append_cstr(result, ".");
    tau_path_append_cstr(result, extension);

    return result;
  }

  size_t last_dot_or_sep = str_len;

  while (last_dot_or_sep > 0 && str_ptr[last_dot_or_sep - 1] != '.' && !tau_path_is_directory_separator(str_ptr[last_dot_or_sep - 1]))
    --last_dot_or_sep;

  if (last_dot_or_sep == 0)
  {
    tau_path_t* result = tau_path_copy(path);

    tau_path_append_cstr(result, ".");
    tau_path_append_cstr(result, extension);

    return result;
  }

  if (str_ptr[last_dot_or_sep - 1] == '.')
  {
    tau_path_t* result = tau_path_init_with_cstr_and_length(str_ptr, last_dot_or_sep);

    tau_path_append_cstr(result, extension);

    return result;
  }

  tau_path_t* result = tau_path_init_with_cstr_and_length(str_ptr, last_dot_or_sep);

  tau_path_append_cstr(result, ".");
  tau_path_append_cstr(result, extension);

  return result;
}

int tau_path_compare(tau_path_t* lhs, tau_path_t* rhs)
{
  return tau_string_compare((tau_string_t*)lhs, (tau_string_t*)rhs);
}
