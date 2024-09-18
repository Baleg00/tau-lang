/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "linker/gcc.h"

#include "utils/common.h"
#include "utils/str.h"
#include "utils/io/command.h"
#include "utils/memory/memtrace.h"

/**
 * \brief Context data for the GCC linker.
 */
typedef struct linker_gcc_ctx_t
{
  command_t* cmd;
  linker_output_kind_t output_kind;
  string_t* output_file;
  linker_optimization_level_t level;
  linker_visibility_t visibility;
  bool is_debugging;
} linker_gcc_ctx_t;

/**
 * \brief Hints to the linker that the next library is static.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void linker_gcc_hint_static(linker_t* linker)
{
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-Bstatic");
}

/**
 * \brief Hints to the linker that the next library is dynamic.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void linker_gcc_hint_dynamic(linker_t* linker)
{
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-Bdynamic");
}

/**
 * \brief Sets the linker's output to generate a dynamically linked, non position-independent executable.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void linker_gcc_set_output_dynamic_nonpie(linker_t* linker)
{
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-no-pie");
}

/**
 * \brief Sets the linker's output to generate a dynamically linked, position-independent executable.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void linker_gcc_set_output_dynamic_pie(linker_t* linker)
{
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-fPIE");
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-pie");
}

/**
 * \brief Sets the linker's output to generate a statically linked, non position-independent executable.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void linker_gcc_set_output_static_nonpie(linker_t* linker)
{
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-static");
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-no-pie");
}

/**
 * \brief Sets the linker's output to generate a statically linked, position-independent executable.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void linker_gcc_set_output_static_pie(linker_t* linker)
{
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-static");
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-fPIE");
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-pie");
}

/**
 * \brief Sets the linker's output to generate a dynamic library.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void linker_gcc_set_output_dynamic_library(linker_t* linker)
{
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-shared");
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-fPIC");
}

/**
 * \brief Sets the linker's output to generate a dynamic library with bundled libc.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void linker_gcc_set_output_dynamic_library_libc(linker_t* linker)
{
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-shared"); // Generate a shared library.
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-fPIC"); // Ensure position-independent code.
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-static-libgcc"); // Statically link the GCC runtime library.
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-Wl,-Bstatic"); // Link statically.
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-lc"); // Link libc.
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, "-Wl,-Bdynamic"); // Return to dynamic linking.
}

linker_t* linker_gcc_init(void)
{
  linker_t* linker = (linker_t*)malloc(sizeof(linker_t));

  linker->kind = LINKER_GCC;

  linker_gcc_ctx_t* ctx = (linker_gcc_ctx_t*)malloc(sizeof(linker_gcc_ctx_t));

  ctx->cmd = command_init("gcc");
  ctx->output_kind = LINKER_OUTPUT_DYNAMIC_NONPIE;
  ctx->output_file = NULL;
  ctx->level = LINKER_OPTIMIZATION_DEFAULT;
  ctx->visibility = LINKER_VISIBILITY_DEFAULT;
  ctx->is_debugging = false;

  linker->data = ctx;

  return linker;
}

void linker_gcc_free(linker_t* linker)
{
  command_free(((linker_gcc_ctx_t*)linker->data)->cmd);

  free(linker->data);
  free(linker);
}

void linker_gcc_add_object(linker_t* linker, const char* path)
{
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, path);
}

void linker_gcc_add_library_directory(linker_t* linker, const char* path)
{
  string_t* arg_str = string_init_with_cstr("-L");
  string_append_cstr(arg_str, path);

  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, string_begin(arg_str));

  string_free(arg_str);
}

void linker_gcc_add_static_library_by_name(linker_t* linker, const char* name)
{
  linker_gcc_hint_static(linker);

  string_t* arg_str = string_init_with_cstr("-l");
  string_append_cstr(arg_str, name);

  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, string_begin(arg_str));

  string_free(arg_str);
}

void linker_gcc_add_static_library_by_path(linker_t* linker, const char* path)
{
  linker_gcc_hint_static(linker);

  string_t* arg_str = string_init_with_cstr("-l");
  string_append_cstr(arg_str, path);

  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, string_begin(arg_str));

  string_free(arg_str);
}

void linker_gcc_add_dynamic_library_by_name(linker_t* linker, const char* name)
{
  linker_gcc_hint_dynamic(linker);

  string_t* arg_str = string_init_with_cstr("-l");
  string_append_cstr(arg_str, name);

  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, string_begin(arg_str));

  string_free(arg_str);
}

void linker_gcc_add_dynamic_library_by_path(linker_t* linker, const char* path)
{
  linker_gcc_hint_dynamic(linker);

  string_t* arg_str = string_init_with_cstr("-l");
  string_append_cstr(arg_str, path);

  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, string_begin(arg_str));

  string_free(arg_str);
}

void linker_gcc_set_output_kind(linker_t* linker, linker_output_kind_t kind)
{
  ((linker_gcc_ctx_t*)linker->data)->output_kind = kind;
}

void linker_gcc_set_output_file(linker_t* linker, const char* file)
{
  linker_gcc_ctx_t* ctx = (linker_gcc_ctx_t*)linker->data;

  if (ctx->output_file == NULL)
    ctx->output_file = string_init_with_cstr(file);
  else
  {
    string_clear(ctx->output_file);
    string_append_cstr(ctx->output_file, file);
  }
}

void linker_gcc_set_optimization_level(linker_t* linker, linker_optimization_level_t level)
{
  ((linker_gcc_ctx_t*)linker->data)->level = level;
}

void linker_gcc_set_debugging(linker_t* linker, bool debugging)
{
  ((linker_gcc_ctx_t*)linker->data)->is_debugging = debugging;
}

void linker_gcc_set_entry_point(linker_t* linker, const char* entry)
{
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, entry);
}

void linker_gcc_set_visibility(linker_t* linker, linker_visibility_t visibility)
{
  ((linker_gcc_ctx_t*)linker->data)->visibility = visibility;
}

void linker_gcc_add_flag(linker_t* linker, const char* flag)
{
  command_add_arg(((linker_gcc_ctx_t*)linker->data)->cmd, flag);
}

bool linker_gcc_link(linker_t* linker)
{
  linker_gcc_ctx_t* ctx = (linker_gcc_ctx_t*)linker->data;

  switch (ctx->output_kind)
  {
  case LINKER_OUTPUT_DYNAMIC_NONPIE:       linker_gcc_set_output_dynamic_nonpie      (linker); break;
  case LINKER_OUTPUT_DYNAMIC_PIE:          linker_gcc_set_output_dynamic_pie         (linker); break;
  case LINKER_OUTPUT_STATIC_NONPIE:        linker_gcc_set_output_static_nonpie       (linker); break;
  case LINKER_OUTPUT_STATIC_PIE:           linker_gcc_set_output_static_pie          (linker); break;
  case LINKER_OUTPUT_DYNAMIC_LIBRARY:      linker_gcc_set_output_dynamic_library     (linker); break;
  case LINKER_OUTPUT_DYNAMIC_LIBRARY_LIBC: linker_gcc_set_output_dynamic_library_libc(linker); break;
  default: UNREACHABLE();
  }

  if (ctx->output_file != NULL)
  {
    command_add_arg(ctx->cmd, "-o");
    command_add_arg(ctx->cmd, string_begin(ctx->output_file));
  }

  const char* optim_arg = NULL;

  switch (ctx->level)
  {
  case LINKER_OPTIMIZATION_NONE:       optim_arg = "-O0";    break;
  case LINKER_OPTIMIZATION_LESS:       optim_arg = "-O1";    break;
  case LINKER_OPTIMIZATION_DEFAULT:    optim_arg = "-O2";    break;
  case LINKER_OPTIMIZATION_AGGRESSIVE: optim_arg = "-O3";    break;
  case LINKER_OPTIMIZATION_SIZE:       optim_arg = "-Os";    break;
  case LINKER_OPTIMIZATION_SPEED:      optim_arg = "-Ofast"; break;
  case LINKER_OPTIMIZATION_DEBUG:      optim_arg = "-Og";    break;
  default: UNREACHABLE();
  }

  command_add_arg(ctx->cmd, optim_arg);

  if (ctx->is_debugging)
    command_add_arg(ctx->cmd, "-g");

  const char* visibility_arg = NULL;

  switch (ctx->visibility)
  {
  case LINKER_VISIBILITY_DEFAULT:   visibility_arg = "-fvisibility=default";   break;
  case LINKER_VISIBILITY_HIDDEN:    visibility_arg = "-fvisibility=hidden";    break;
  case LINKER_VISIBILITY_PROTECTED: visibility_arg = "-fvisibility=protected"; break;
  default: UNREACHABLE();
  }

  command_add_arg(ctx->cmd, visibility_arg);

  int exit_status = command_run(ctx->cmd);

  command_reset(ctx->cmd);

  ctx->output_kind = LINKER_OUTPUT_DYNAMIC_NONPIE;

  string_free(ctx->output_file);
  ctx->output_file = NULL;

  ctx->level = LINKER_OPTIMIZATION_LESS;
  ctx->visibility = LINKER_VISIBILITY_DEFAULT;
  ctx->is_debugging = false;

  return exit_status == EXIT_SUCCESS;
}
