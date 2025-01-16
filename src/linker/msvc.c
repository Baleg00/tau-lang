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
typedef struct linker_msvc_ctx_t
{
  command_t* cmd;
  linker_output_kind_t output_kind;
  string_t* output_file;
  linker_optimization_level_t level;
  linker_visibility_t visibility;
  bool is_debugging;
} linker_msvc_ctx_t;

/**
 * \brief Sets the linker's output to generate a dynamic library.
 *
 * \param[in] linker Pointer to the linker to be used.
 */
static void linker_msvc_set_output_dynamic_library(linker_t* linker)
{
  command_add_arg(((linker_msvc_ctx_t*)linker->data)->cmd, "/DLL");
}

/**
 * \brief Resets the linker to its default state.
 *
 * \param[in,out] linker Pointer to the linker to be used.
 */
static void linker_msvc_reset(linker_t* linker)
{
  linker_msvc_ctx_t* ctx = (linker_msvc_ctx_t*)linker->data;

  command_reset(ctx->cmd);

  ctx->output_kind = LINKER_OUTPUT_DYNAMIC_NONPIE;

  if (ctx->output_file != NULL)
  {
    string_free(ctx->output_file);
    ctx->output_file = NULL;
  }

  ctx->level = LINKER_OPTIMIZATION_LESS;
  ctx->visibility = LINKER_VISIBILITY_DEFAULT;
  ctx->is_debugging = false;
}

linker_t* linker_msvc_init(void)
{
  linker_t* linker = (linker_t*)malloc(sizeof(linker_t));

  linker->kind = LINKER_MSVC;

  linker_msvc_ctx_t* ctx = (linker_msvc_ctx_t*)malloc(sizeof(linker_msvc_ctx_t));

  ctx->cmd = command_init("link");
  ctx->output_kind = LINKER_OUTPUT_DYNAMIC_NONPIE;
  ctx->output_file = NULL;
  ctx->level = LINKER_OPTIMIZATION_DEFAULT;
  ctx->visibility = LINKER_VISIBILITY_DEFAULT;
  ctx->is_debugging = false;

  linker->data = ctx;

  return linker;
}

void linker_msvc_free(linker_t* linker)
{
  command_free(((linker_msvc_ctx_t*)linker->data)->cmd);

  free(linker->data);
  free(linker);
}

void linker_msvc_add_object(linker_t* linker, const char* path)
{
  command_add_arg(((linker_msvc_ctx_t*)linker->data)->cmd, path);
}

void linker_msvc_add_library_directory(linker_t* linker, const char* path)
{
  string_t* arg_str = string_init_with_cstr("/LIBPATH:");
  string_append_cstr(arg_str, path);

  command_add_arg(((linker_msvc_ctx_t*)linker->data)->cmd, string_begin(arg_str));

  string_free(arg_str);
}

void linker_msvc_add_static_library_by_name(linker_t* linker, const char* name)
{
  command_add_arg(((linker_msvc_ctx_t*)linker->data)->cmd, name);
}

void linker_msvc_add_static_library_by_path(linker_t* linker, const char* path)
{
  command_add_arg(((linker_msvc_ctx_t*)linker->data)->cmd, path);
}

void linker_msvc_add_dynamic_library_by_name(linker_t* linker, const char* name)
{
  command_add_arg(((linker_msvc_ctx_t*)linker->data)->cmd, name);
}

void linker_msvc_add_dynamic_library_by_path(linker_t* linker, const char* path)
{
  command_add_arg(((linker_msvc_ctx_t*)linker->data)->cmd, path);
}

void linker_msvc_set_output_kind(linker_t* linker, linker_output_kind_t kind)
{
  ((linker_msvc_ctx_t*)linker->data)->output_kind = kind;
}

void linker_msvc_set_output_file(linker_t* linker, const char* file)
{
  linker_msvc_ctx_t* ctx = (linker_msvc_ctx_t*)linker->data;

  if (ctx->output_file == NULL)
    ctx->output_file = string_init_with_cstr(file);
  else
  {
    string_clear(ctx->output_file);
    string_append_cstr(ctx->output_file, file);
  }
}

void linker_msvc_set_optimization_level(linker_t* linker, linker_optimization_level_t level)
{
  ((linker_msvc_ctx_t*)linker->data)->level = level;
}

void linker_msvc_set_debugging(linker_t* linker, bool debugging)
{
  ((linker_msvc_ctx_t*)linker->data)->is_debugging = debugging;
}

void linker_msvc_set_entry_point(linker_t* linker, const char* entry)
{
  command_add_arg(((linker_msvc_ctx_t*)linker->data)->cmd, entry);
}

void linker_msvc_set_visibility(linker_t* linker, linker_visibility_t visibility)
{
  ((linker_msvc_ctx_t*)linker->data)->visibility = visibility;
}

void linker_msvc_add_flag(linker_t* linker, const char* flag)
{
  command_add_arg(((linker_msvc_ctx_t*)linker->data)->cmd, flag);
}

bool linker_msvc_link(linker_t* linker)
{
  linker_msvc_ctx_t* ctx = (linker_msvc_ctx_t*)linker->data;

  switch (ctx->output_kind)
  {
  case LINKER_OUTPUT_DYNAMIC_NONPIE:
  case LINKER_OUTPUT_DYNAMIC_PIE:
  case LINKER_OUTPUT_STATIC_NONPIE:
  case LINKER_OUTPUT_STATIC_PIE: break;
  case LINKER_OUTPUT_DYNAMIC_LIBRARY:
  case LINKER_OUTPUT_DYNAMIC_LIBRARY_LIBC: linker_msvc_set_output_dynamic_library(linker); break;
  default: UNREACHABLE();
  }

  if (ctx->output_file != NULL)
  {
    string_t* arg_str = string_init_with_cstr("/OUT:");
    string_append(arg_str, ctx->output_file);

    command_add_arg(ctx->cmd, string_begin(arg_str));

    string_free(arg_str);
  }

  if (ctx->is_debugging)
    command_add_arg(ctx->cmd, "/DEBUG");

  command_add_arg(ctx->cmd, "/SUBSYSTEM:CONSOLE");
  command_add_arg(ctx->cmd, "/ENTRY:mainCRTStartup");

  int exit_status = command_run(ctx->cmd);

  linker_msvc_reset(linker);

  return exit_status == EXIT_SUCCESS;
}
