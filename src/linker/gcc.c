/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "linker/gcc.h"

#include "utils/str.h"
#include "utils/io/command.h"

/**
 * \brief Context data for the GCC linker.
 */
typedef struct tau_linker_gcc_ctx_t
{
  tau_command_t* cmd;
  tau_linker_output_kind_t output_kind;
  tau_string_t* output_file;
  tau_linker_optimization_level_t level;
  tau_linker_visibility_t visibility;
  bool is_debugging;
} tau_linker_gcc_ctx_t;

/**
 * \brief Hints to the linker that the next library is static.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void tau_linker_gcc_hint_static(tau_linker_t* linker)
{
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-Bstatic");
}

/**
 * \brief Hints to the linker that the next library is dynamic.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void tau_linker_gcc_hint_dynamic(tau_linker_t* linker)
{
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-Bdynamic");
}

/**
 * \brief Sets the linker's output to generate a dynamically linked, non position-independent executable.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void tau_linker_gcc_set_output_dynamic_nonpie(tau_linker_t* linker)
{
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-no-pie");
}

/**
 * \brief Sets the linker's output to generate a dynamically linked, position-independent executable.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void tau_linker_gcc_set_output_dynamic_pie(tau_linker_t* linker)
{
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-fPIE");
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-pie");
}

/**
 * \brief Sets the linker's output to generate a statically linked, non position-independent executable.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void tau_linker_gcc_set_output_static_nonpie(tau_linker_t* linker)
{
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-static");
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-no-pie");
}

/**
 * \brief Sets the linker's output to generate a statically linked, position-independent executable.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void tau_linker_gcc_set_output_static_pie(tau_linker_t* linker)
{
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-static");
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-fPIE");
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-pie");
}

/**
 * \brief Sets the linker's output to generate a dynamic library.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void tau_linker_gcc_set_output_dynamic_library(tau_linker_t* linker)
{
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-shared");
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-fPIC");
}

/**
 * \brief Sets the linker's output to generate a dynamic library with bundled libc.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void tau_linker_gcc_set_output_dynamic_library_libc(tau_linker_t* linker)
{
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-shared"); // Generate a shared library.
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-fPIC"); // Ensure position-independent code.
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-static-libgcc"); // Statically link the GCC runtime library.
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-Wl,-Bstatic"); // Link statically.
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-lc"); // Link libc.
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, "-Wl,-Bdynamic"); // Return to dynamic linking.
}

tau_linker_t* tau_linker_gcc_init(void)
{
  tau_linker_t* linker = (tau_linker_t*)malloc(sizeof(tau_linker_t));

  linker->kind = TAU_LINKER_GCC;

  tau_linker_gcc_ctx_t* ctx = (tau_linker_gcc_ctx_t*)malloc(sizeof(tau_linker_gcc_ctx_t));

  ctx->cmd = tau_command_init("gcc");
  ctx->output_kind = TAU_LINKER_OUTPUT_DYNAMIC_NONPIE;
  ctx->output_file = NULL;
  ctx->level = TAU_LINKER_OPTIMIZATION_DEFAULT;
  ctx->visibility = TAU_LINKER_VISIBILITY_DEFAULT;
  ctx->is_debugging = false;

  linker->data = ctx;

  return linker;
}

void tau_linker_gcc_free(tau_linker_t* linker)
{
  tau_command_free(((tau_linker_gcc_ctx_t*)linker->data)->cmd);

  free(linker->data);
  free(linker);
}

void tau_linker_gcc_add_object(tau_linker_t* linker, const char* path)
{
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, path);
}

void tau_linker_gcc_add_library_directory(tau_linker_t* linker, const char* path)
{
  tau_string_t* arg_str = tau_string_init_with_cstr("-L");
  tau_string_append_cstr(arg_str, path);

  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, tau_string_begin(arg_str));

  tau_string_free(arg_str);
}

void tau_linker_gcc_add_static_library_by_name(tau_linker_t* linker, const char* name)
{
  tau_linker_gcc_hint_static(linker);

  tau_string_t* arg_str = tau_string_init_with_cstr("-l");
  tau_string_append_cstr(arg_str, name);

  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, tau_string_begin(arg_str));

  tau_string_free(arg_str);
}

void tau_linker_gcc_add_static_library_by_path(tau_linker_t* linker, const char* path)
{
  tau_linker_gcc_hint_static(linker);

  tau_string_t* arg_str = tau_string_init_with_cstr("-l");
  tau_string_append_cstr(arg_str, path);

  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, tau_string_begin(arg_str));

  tau_string_free(arg_str);
}

void tau_linker_gcc_add_dynamic_library_by_name(tau_linker_t* linker, const char* name)
{
  tau_linker_gcc_hint_dynamic(linker);

  tau_string_t* arg_str = tau_string_init_with_cstr("-l");
  tau_string_append_cstr(arg_str, name);

  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, tau_string_begin(arg_str));

  tau_string_free(arg_str);
}

void tau_linker_gcc_add_dynamic_library_by_path(tau_linker_t* linker, const char* path)
{
  tau_linker_gcc_hint_dynamic(linker);

  tau_string_t* arg_str = tau_string_init_with_cstr("-l");
  tau_string_append_cstr(arg_str, path);

  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, tau_string_begin(arg_str));

  tau_string_free(arg_str);
}

void tau_linker_gcc_set_output_kind(tau_linker_t* linker, tau_linker_output_kind_t kind)
{
  ((tau_linker_gcc_ctx_t*)linker->data)->output_kind = kind;
}

void tau_linker_gcc_set_output_file(tau_linker_t* linker, const char* file)
{
  tau_linker_gcc_ctx_t* ctx = (tau_linker_gcc_ctx_t*)linker->data;

  if (ctx->output_file == NULL)
    ctx->output_file = tau_string_init_with_cstr(file);
  else
  {
    tau_string_clear(ctx->output_file);
    tau_string_append_cstr(ctx->output_file, file);
  }
}

void tau_linker_gcc_set_optimization_level(tau_linker_t* linker, tau_linker_optimization_level_t level)
{
  ((tau_linker_gcc_ctx_t*)linker->data)->level = level;
}

void tau_linker_gcc_set_debugging(tau_linker_t* linker, bool debugging)
{
  ((tau_linker_gcc_ctx_t*)linker->data)->is_debugging = debugging;
}

void tau_linker_gcc_set_entry_point(tau_linker_t* linker, const char* entry)
{
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, entry);
}

void tau_linker_gcc_set_visibility(tau_linker_t* linker, tau_linker_visibility_t visibility)
{
  ((tau_linker_gcc_ctx_t*)linker->data)->visibility = visibility;
}

void tau_linker_gcc_add_flag(tau_linker_t* linker, const char* flag)
{
  tau_command_add_arg(((tau_linker_gcc_ctx_t*)linker->data)->cmd, flag);
}

bool tau_linker_gcc_link(tau_linker_t* linker)
{
  tau_linker_gcc_ctx_t* ctx = (tau_linker_gcc_ctx_t*)linker->data;

  switch (ctx->output_kind)
  {
  case TAU_LINKER_OUTPUT_DYNAMIC_NONPIE:       tau_linker_gcc_set_output_dynamic_nonpie      (linker); break;
  case TAU_LINKER_OUTPUT_DYNAMIC_PIE:          tau_linker_gcc_set_output_dynamic_pie         (linker); break;
  case TAU_LINKER_OUTPUT_STATIC_NONPIE:        tau_linker_gcc_set_output_static_nonpie       (linker); break;
  case TAU_LINKER_OUTPUT_STATIC_PIE:           tau_linker_gcc_set_output_static_pie          (linker); break;
  case TAU_LINKER_OUTPUT_DYNAMIC_LIBRARY:      tau_linker_gcc_set_output_dynamic_library     (linker); break;
  case TAU_LINKER_OUTPUT_DYNAMIC_LIBRARY_LIBC: tau_linker_gcc_set_output_dynamic_library_libc(linker); break;
  default: TAU_UNREACHABLE();
  }

  if (ctx->output_file != NULL)
  {
    tau_command_add_arg(ctx->cmd, "-o");
    tau_command_add_arg(ctx->cmd, tau_string_begin(ctx->output_file));
  }

  const char* optim_arg = NULL;

  switch (ctx->level)
  {
  case TAU_LINKER_OPTIMIZATION_NONE:       optim_arg = "-O0";    break;
  case TAU_LINKER_OPTIMIZATION_LESS:       optim_arg = "-O1";    break;
  case TAU_LINKER_OPTIMIZATION_DEFAULT:    optim_arg = "-O2";    break;
  case TAU_LINKER_OPTIMIZATION_AGGRESSIVE: optim_arg = "-O3";    break;
  case TAU_LINKER_OPTIMIZATION_SIZE:       optim_arg = "-Os";    break;
  case TAU_LINKER_OPTIMIZATION_SPEED:      optim_arg = "-Ofast"; break;
  case TAU_LINKER_OPTIMIZATION_DEBUG:      optim_arg = "-Og";    break;
  default: TAU_UNREACHABLE();
  }

  tau_command_add_arg(ctx->cmd, optim_arg);

  if (ctx->is_debugging)
    tau_command_add_arg(ctx->cmd, "-g");

  const char* visibility_arg = NULL;

  switch (ctx->visibility)
  {
  case TAU_LINKER_VISIBILITY_DEFAULT:   visibility_arg = "-fvisibility=default";   break;
  case TAU_LINKER_VISIBILITY_HIDDEN:    visibility_arg = "-fvisibility=hidden";    break;
  case TAU_LINKER_VISIBILITY_PROTECTED: visibility_arg = "-fvisibility=protected"; break;
  default: TAU_UNREACHABLE();
  }

  tau_command_add_arg(ctx->cmd, visibility_arg);

  int exit_status = tau_command_run(ctx->cmd);

  tau_command_reset(ctx->cmd);

  ctx->output_kind = TAU_LINKER_OUTPUT_DYNAMIC_NONPIE;

  tau_string_free(ctx->output_file);
  ctx->output_file = NULL;

  ctx->level = TAU_LINKER_OPTIMIZATION_LESS;
  ctx->visibility = TAU_LINKER_VISIBILITY_DEFAULT;
  ctx->is_debugging = false;

  return exit_status == EXIT_SUCCESS;
}
