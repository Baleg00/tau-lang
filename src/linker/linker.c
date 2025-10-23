/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "linker/linker.h"

#include "linker/gcc.h"
#include "linker/msvc.h"

tau_linker_t* tau_linker_init(tau_linker_kind_t kind)
{
  switch (kind)
  {
  case TAU_LINKER_GCC:  return tau_linker_gcc_init ();
  case TAU_LINKER_MSVC: return tau_linker_msvc_init();
  default: TAU_UNREACHABLE();
  }

  return NULL;
}

void tau_linker_free(tau_linker_t* linker)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_free (linker); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_free(linker); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_add_object(tau_linker_t* linker, const char* path)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_add_object (linker, path); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_add_object(linker, path); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_add_library_directory(tau_linker_t* linker, const char* path)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_add_library_directory (linker, path); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_add_library_directory(linker, path); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_add_static_library_by_name(tau_linker_t* linker, const char* name)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_add_static_library_by_name (linker, name); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_add_static_library_by_name(linker, name); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_add_static_library_by_path(tau_linker_t* linker, const char* path)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_add_static_library_by_path (linker, path); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_add_static_library_by_path(linker, path); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_add_dynamic_library_by_name(tau_linker_t* linker, const char* name)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_add_dynamic_library_by_path (linker, name); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_add_dynamic_library_by_path(linker, name); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_add_dynamic_library_by_path(tau_linker_t* linker, const char* path)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_add_dynamic_library_by_path (linker, path); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_add_dynamic_library_by_path(linker, path); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_set_output_kind(tau_linker_t* linker, tau_linker_output_kind_t kind)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_set_output_kind (linker, kind); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_set_output_kind(linker, kind); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_set_output_file(tau_linker_t* linker, const char* file)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_set_output_file (linker, file); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_set_output_file(linker, file); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_set_optimization_level(tau_linker_t* linker, tau_linker_optimization_level_t level)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_set_optimization_level (linker, level); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_set_optimization_level(linker, level); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_set_debugging(tau_linker_t* linker, bool debugging)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_set_debugging (linker, debugging); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_set_debugging(linker, debugging); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_set_entry_point(tau_linker_t* linker, const char* entry)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_set_entry_point (linker, entry); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_set_entry_point(linker, entry); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_set_visibility(tau_linker_t* linker, tau_linker_visibility_t visibility)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_set_visibility (linker, visibility); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_set_visibility(linker, visibility); return;
  default: TAU_UNREACHABLE();
  }
}

void tau_linker_add_flag(tau_linker_t* linker, const char* flag)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  tau_linker_gcc_add_flag (linker, flag); return;
  case TAU_LINKER_MSVC: tau_linker_msvc_add_flag(linker, flag); return;
  default: TAU_UNREACHABLE();
  }
}

bool tau_linker_link(tau_linker_t* linker)
{
  switch (linker->kind)
  {
  case TAU_LINKER_GCC:  return tau_linker_gcc_link (linker);
  case TAU_LINKER_MSVC: return tau_linker_msvc_link(linker);
  default: TAU_UNREACHABLE();
  }

  return false;
}
