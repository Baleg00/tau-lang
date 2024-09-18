/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "linker/linker.h"

#include "linker/gcc.h"
#include "linker/msvc.h"
#include "utils/common.h"
#include "utils/collections/vector.h"

linker_t* linker_init(linker_kind_t kind)
{
  switch (kind)
  {
  case LINKER_GCC:  return linker_gcc_init ();
  case LINKER_MSVC: return linker_msvc_init();
  default: UNREACHABLE();
  }

  return NULL;
}

void linker_free(linker_t* linker)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_free (linker); return;
  case LINKER_MSVC: linker_msvc_free(linker); return;
  default: UNREACHABLE();
  }
}

void linker_add_object(linker_t* linker, const char* path)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_add_object (linker, path); return;
  case LINKER_MSVC: linker_msvc_add_object(linker, path); return;
  default: UNREACHABLE();
  }
}

void linker_add_library_directory(linker_t* linker, const char* path)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_add_library_directory (linker, path); return;
  case LINKER_MSVC: linker_msvc_add_library_directory(linker, path); return;
  default: UNREACHABLE();
  }
}

void linker_add_static_library_by_name(linker_t* linker, const char* name)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_add_static_library_by_name (linker, name); return;
  case LINKER_MSVC: linker_msvc_add_static_library_by_name(linker, name); return;
  default: UNREACHABLE();
  }
}

void linker_add_static_library_by_path(linker_t* linker, const char* path)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_add_static_library_by_path (linker, path); return;
  case LINKER_MSVC: linker_msvc_add_static_library_by_path(linker, path); return;
  default: UNREACHABLE();
  }
}

void linker_add_dynamic_library_by_name(linker_t* linker, const char* name)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_add_dynamic_library_by_path (linker, name); return;
  case LINKER_MSVC: linker_msvc_add_dynamic_library_by_path(linker, name); return;
  default: UNREACHABLE();
  }
}

void linker_add_dynamic_library_by_path(linker_t* linker, const char* path)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_add_dynamic_library_by_path (linker, path); return;
  case LINKER_MSVC: linker_msvc_add_dynamic_library_by_path(linker, path); return;
  default: UNREACHABLE();
  }
}

void linker_set_output_kind(linker_t* linker, linker_output_kind_t kind)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_set_output_kind (linker, kind); return;
  case LINKER_MSVC: linker_msvc_set_output_kind(linker, kind); return;
  default: UNREACHABLE();
  }
}

void linker_set_output_file(linker_t* linker, const char* file)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_set_output_file (linker, file); return;
  case LINKER_MSVC: linker_msvc_set_output_file(linker, file); return;
  default: UNREACHABLE();
  }
}

void linker_set_optimization_level(linker_t* linker, linker_optimization_level_t level)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_set_optimization_level (linker, level); return;
  case LINKER_MSVC: linker_msvc_set_optimization_level(linker, level); return;
  default: UNREACHABLE();
  }
}

void linker_set_debugging(linker_t* linker, bool debugging)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_set_debugging (linker, debugging); return;
  case LINKER_MSVC: linker_msvc_set_debugging(linker, debugging); return;
  default: UNREACHABLE();
  }
}

void linker_set_entry_point(linker_t* linker, const char* entry)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_set_entry_point (linker, entry); return;
  case LINKER_MSVC: linker_msvc_set_entry_point(linker, entry); return;
  default: UNREACHABLE();
  }
}

void linker_set_visibility(linker_t* linker, linker_visibility_t visibility)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_set_visibility (linker, visibility); return;
  case LINKER_MSVC: linker_msvc_set_visibility(linker, visibility); return;
  default: UNREACHABLE();
  }
}

void linker_add_flag(linker_t* linker, const char* flag)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  linker_gcc_add_flag (linker, flag); return;
  case LINKER_MSVC: linker_msvc_add_flag(linker, flag); return;
  default: UNREACHABLE();
  }
}

bool linker_link(linker_t* linker)
{
  switch (linker->kind)
  {
  case LINKER_GCC:  return linker_gcc_link (linker);
  case LINKER_MSVC: return linker_msvc_link(linker);
  default: UNREACHABLE();
  }

  return false;
}
