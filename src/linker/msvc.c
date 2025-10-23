/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "linker/msvc.h"

#include "utils/str.h"
#include "utils/io/command.h"

/**
 * \brief Context data for the MSVC linker.
 */
typedef struct tau_linker_msvc_ctx_t
{
  tau_command_t* cmd;
  tau_linker_output_kind_t output_kind;
  tau_string_t* output_file;
  tau_linker_optimization_level_t level;
  tau_linker_visibility_t visibility;
  bool is_debugging;
} tau_linker_msvc_ctx_t;

/**
 * \brief Sets the linker's output to generate a dynamic library.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void tau_linker_msvc_set_output_dynamic_library(tau_linker_t* linker)
{
  tau_command_add_arg(((tau_linker_msvc_ctx_t*)linker->data)->cmd, "/DLL");
}

/**
 * \brief Resets the linker to its default state.
 *
 * \param[in,out] linker Pointer to the linker to be used.
 */
static void tau_linker_msvc_reset(tau_linker_t* linker)
{
  tau_linker_msvc_ctx_t* ctx = (tau_linker_msvc_ctx_t*)linker->data;

  tau_command_reset(ctx->cmd);

  ctx->output_kind = TAU_LINKER_OUTPUT_DYNAMIC_NONPIE;

  if (ctx->output_file != NULL)
  {
    tau_string_free(ctx->output_file);
    ctx->output_file = NULL;
  }

  ctx->level = TAU_LINKER_OPTIMIZATION_LESS;
  ctx->visibility = TAU_LINKER_VISIBILITY_DEFAULT;
  ctx->is_debugging = false;
}

tau_linker_t* tau_linker_msvc_init(void)
{
  tau_linker_t* linker = (tau_linker_t*)malloc(sizeof(tau_linker_t));

  linker->kind = TAU_LINKER_MSVC;

  tau_linker_msvc_ctx_t* ctx = (tau_linker_msvc_ctx_t*)malloc(sizeof(tau_linker_msvc_ctx_t));

  ctx->cmd = tau_command_init("link");
  ctx->output_kind = TAU_LINKER_OUTPUT_DYNAMIC_NONPIE;
  ctx->output_file = NULL;
  ctx->level = TAU_LINKER_OPTIMIZATION_DEFAULT;
  ctx->visibility = TAU_LINKER_VISIBILITY_DEFAULT;
  ctx->is_debugging = false;

  linker->data = ctx;

  return linker;
}

void tau_linker_msvc_free(tau_linker_t* linker)
{
  tau_command_free(((tau_linker_msvc_ctx_t*)linker->data)->cmd);

  free(linker->data);
  free(linker);
}

void tau_linker_msvc_add_object(tau_linker_t* linker, const char* path)
{
  tau_command_add_arg(((tau_linker_msvc_ctx_t*)linker->data)->cmd, path);
}

void tau_linker_msvc_add_library_directory(tau_linker_t* linker, const char* path)
{
  tau_string_t* arg_str = tau_string_init_with_cstr("/LIBPATH:");
  tau_string_append_cstr(arg_str, path);

  tau_command_add_arg(((tau_linker_msvc_ctx_t*)linker->data)->cmd, tau_string_begin(arg_str));

  tau_string_free(arg_str);
}

void tau_linker_msvc_add_static_library_by_name(tau_linker_t* linker, const char* name)
{
  tau_command_add_arg(((tau_linker_msvc_ctx_t*)linker->data)->cmd, name);
}

void tau_linker_msvc_add_static_library_by_path(tau_linker_t* linker, const char* path)
{
  tau_command_add_arg(((tau_linker_msvc_ctx_t*)linker->data)->cmd, path);
}

void tau_linker_msvc_add_dynamic_library_by_name(tau_linker_t* linker, const char* name)
{
  tau_command_add_arg(((tau_linker_msvc_ctx_t*)linker->data)->cmd, name);
}

void tau_linker_msvc_add_dynamic_library_by_path(tau_linker_t* linker, const char* path)
{
  tau_command_add_arg(((tau_linker_msvc_ctx_t*)linker->data)->cmd, path);
}

void tau_linker_msvc_set_output_kind(tau_linker_t* linker, tau_linker_output_kind_t kind)
{
  ((tau_linker_msvc_ctx_t*)linker->data)->output_kind = kind;
}

void tau_linker_msvc_set_output_file(tau_linker_t* linker, const char* file)
{
  tau_linker_msvc_ctx_t* ctx = (tau_linker_msvc_ctx_t*)linker->data;

  if (ctx->output_file == NULL)
    ctx->output_file = tau_string_init_with_cstr(file);
  else
  {
    tau_string_clear(ctx->output_file);
    tau_string_append_cstr(ctx->output_file, file);
  }
}

void tau_linker_msvc_set_optimization_level(tau_linker_t* linker, tau_linker_optimization_level_t level)
{
  ((tau_linker_msvc_ctx_t*)linker->data)->level = level;
}

void tau_linker_msvc_set_debugging(tau_linker_t* linker, bool debugging)
{
  ((tau_linker_msvc_ctx_t*)linker->data)->is_debugging = debugging;
}

void tau_linker_msvc_set_entry_point(tau_linker_t* linker, const char* entry)
{
  tau_command_add_arg(((tau_linker_msvc_ctx_t*)linker->data)->cmd, entry);
}

void tau_linker_msvc_set_visibility(tau_linker_t* linker, tau_linker_visibility_t visibility)
{
  ((tau_linker_msvc_ctx_t*)linker->data)->visibility = visibility;
}

void tau_linker_msvc_add_flag(tau_linker_t* linker, const char* flag)
{
  tau_command_add_arg(((tau_linker_msvc_ctx_t*)linker->data)->cmd, flag);
}

bool tau_linker_msvc_link(tau_linker_t* linker)
{
  tau_linker_msvc_ctx_t* ctx = (tau_linker_msvc_ctx_t*)linker->data;

  switch (ctx->output_kind)
  {
  case TAU_LINKER_OUTPUT_DYNAMIC_NONPIE:
  case TAU_LINKER_OUTPUT_DYNAMIC_PIE:
  case TAU_LINKER_OUTPUT_STATIC_NONPIE:
  case TAU_LINKER_OUTPUT_STATIC_PIE: break;
  case TAU_LINKER_OUTPUT_DYNAMIC_LIBRARY:
  case TAU_LINKER_OUTPUT_DYNAMIC_LIBRARY_LIBC: tau_linker_msvc_set_output_dynamic_library(linker); break;
  default: TAU_UNREACHABLE();
  }

  if (ctx->output_file != NULL)
  {
    tau_string_t* arg_str = tau_string_init_with_cstr("/OUT:");
    tau_string_append(arg_str, ctx->output_file);

    tau_command_add_arg(ctx->cmd, tau_string_begin(arg_str));

    tau_string_free(arg_str);
  }

  if (ctx->is_debugging)
    tau_command_add_arg(ctx->cmd, "/DEBUG");

  tau_command_add_arg(ctx->cmd, "/SUBSYSTEM:CONSOLE");
  tau_command_add_arg(ctx->cmd, "/ENTRY:mainCRTStartup");

  int exit_status = tau_command_run(ctx->cmd);

  tau_linker_msvc_reset(linker);

  return exit_status == EXIT_SUCCESS;
}
