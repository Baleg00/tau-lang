/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "linker/linker.h"

#include "linker/gcc.h"
#include "utils/common.h"
#include "utils/collections/vector.h"

/**
 * \brief Function pointer type for `linker_free` function implementation.
 */
typedef void(*linker_free_func_t)(linker_t* /* linker */);

/**
 * \brief Function pointer type for `linker_add_object` function implementation.
 */
typedef void(*linker_add_object_func_t)(linker_t* /* linker */, const char* /* path */);

/**
 * \brief Function pointer type for `linker_add_library_directory` function implementation.
 */
typedef void(*linker_add_library_directory_func_t)(linker_t* /* linker */, const char* /* path */);

/**
 * \brief Function pointer type for `linker_add_static_library_by_name` function implementation.
 */
typedef void(*linker_add_static_library_by_name_func_t)(linker_t* /* linker */, const char* /* name */);

/**
 * \brief Function pointer type for `linker_add_static_library_by_path` function implementation.
 */
typedef void(*linker_add_static_library_by_path_func_t)(linker_t* /* linker */, const char* /* path */);

/**
 * \brief Function pointer type for `linker_add_dynamic_library_by_name` function implementation.
 */
typedef void(*linker_add_dynamic_library_by_name_func_t)(linker_t* /* linker */, const char* /* name */);

/**
 * \brief Function pointer type for `linker_add_dynamic_library_by_path` function implementation.
 */
typedef void(*linker_add_dynamic_library_by_path_func_t)(linker_t* /* linker */, const char* /* path */);

/**
 * \brief Function pointer type for `linker_set_output_kind` function implementation.
 */
typedef void(*linker_set_output_kind_func_t)(linker_t* /* linker */, linker_output_kind_t /* kind */);

/**
 * \brief Function pointer type for `linker_set_output_file` function implementation.
 */
typedef void(*linker_set_output_file_func_t)(linker_t* /* linker */, const char* /* file */);

/**
 * \brief Function pointer type for `linker_set_optimization_level` function implementation.
 */
typedef void(*linker_set_optimization_level_func_t)(linker_t* /* linker */, linker_optimization_level_t /* level */);

/**
 * \brief Function pointer type for `linker_set_debugging` function implementation.
 */
typedef void(*linker_set_debugging_func_t)(linker_t* /* linker */, bool /* debugging */);

/**
 * \brief Function pointer type for `linker_set_entry_point` function implementation.
 */
typedef void(*linker_set_entry_point_func_t)(linker_t* /* linker */, const char* /* entry */);

/**
 * \brief Function pointer type for `linker_set_visibility` function implementation.
 */
typedef void(*linker_set_visibility_func_t)(linker_t* /* linker */, linker_visibility_t /* visibility */);

/**
 * \brief Function pointer type for `linker_add_flag` function implementation.
 */
typedef void(*linker_add_flag_func_t)(linker_t* /* linker */, const char* /* flag */);

/**
 * \brief Function pointer type for `linker_link` function implementation.
 */
typedef bool(*linker_link_func_t)(linker_t* /* linker */);

linker_t* linker_init(linker_kind_t kind)
{
  switch (kind)
  {
  case LINKER_GCC: return linker_gcc_init();
  default: UNREACHABLE();
  }

  return NULL;
}

void linker_free(linker_t* linker)
{
  ((linker_free_func_t)linker->vtable[LINKER_VTABLE_FREE])(linker);
}

void linker_add_object(linker_t* linker, const char* path)
{
  ((linker_add_object_func_t)linker->vtable[LINKER_VTABLE_ADD_OBJECT])(linker, path);
}

void linker_add_library_directory(linker_t* linker, const char* path)
{
  ((linker_add_library_directory_func_t)linker->vtable[LINKER_VTABLE_ADD_LIBRARY_DIRECTORY])(linker, path);
}

void linker_add_static_library_by_name(linker_t* linker, const char* name)
{
  ((linker_add_static_library_by_name_func_t)linker->vtable[LINKER_VTABLE_ADD_STATIC_LIBRARY_BY_NAME])(linker, name);
}

void linker_add_static_library_by_path(linker_t* linker, const char* path)
{
  ((linker_add_static_library_by_path_func_t)linker->vtable[LINKER_VTABLE_ADD_STATIC_LIBRARY_BY_PATH])(linker, path);
}

void linker_add_dynamic_library_by_name(linker_t* linker, const char* name)
{
  ((linker_add_dynamic_library_by_name_func_t)linker->vtable[LINKER_VTABLE_ADD_DYNAMIC_LIBRARY_BY_NAME])(linker, name);
}

void linker_add_dynamic_library_by_path(linker_t* linker, const char* path)
{
  ((linker_add_dynamic_library_by_path_func_t)linker->vtable[LINKER_VTABLE_ADD_DYNAMIC_LIBRARY_BY_PATH])(linker, path);
}

void linker_set_output_kind(linker_t* linker, linker_output_kind_t kind)
{
  ((linker_set_output_kind_func_t)linker->vtable[LINKER_VTABLE_SET_OUTPUT_KIND])(linker, kind);
}

void linker_set_output_file(linker_t* linker, const char* file)
{
  ((linker_set_output_file_func_t)linker->vtable[LINKER_VTABLE_SET_OUTPUT_FILE])(linker, file);
}

void linker_set_optimization_level(linker_t* linker, linker_optimization_level_t level)
{
  ((linker_set_optimization_level_func_t)linker->vtable[LINKER_VTABLE_SET_OPTIMIZATION_LEVEL])(linker, level);
}

void linker_set_debugging(linker_t* linker, bool debugging)
{
  ((linker_set_debugging_func_t)linker->vtable[LINKER_VTABLE_SET_DEBUGGING])(linker, debugging);
}

void linker_set_entry_point(linker_t* linker, const char* entry)
{
  ((linker_set_entry_point_func_t)linker->vtable[LINKER_VTABLE_SET_ENTRY_POINT])(linker, entry);
}

void linker_set_visibility(linker_t* linker, linker_visibility_t visibility)
{
  ((linker_set_visibility_func_t)linker->vtable[LINKER_VTABLE_SET_VISIBILITY])(linker, visibility);
}

void linker_add_flag(linker_t* linker, const char* flag)
{
  ((linker_add_flag_func_t)linker->vtable[LINKER_VTABLE_ADD_FLAG])(linker, flag);
}

bool linker_link(linker_t* linker)
{
  return ((linker_link_func_t)linker->vtable[LINKER_VTABLE_LINK])(linker);
}
