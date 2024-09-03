/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/io/argparse.h"

#include <string.h>

#include "utils/memory/memtrace.h"

struct argparse_ctx_t
{
  const argparse_option_t* opts;
  size_t opt_count;

  const char** argv;
  size_t argc;

  size_t idx;
};

argparse_ctx_t* argparse_ctx_init(const argparse_option_t opts[], size_t opt_count, const char* argv[], int argc)
{
  argparse_ctx_t* ctx = (argparse_ctx_t*)malloc(sizeof(argparse_ctx_t));

  ctx->opts = opts;
  ctx->opt_count = opt_count;
  ctx->argv = argv;
  ctx->argc = (size_t)argc;
  ctx->idx = 1;

  return ctx;
}

void argparse_ctx_free(argparse_ctx_t* ctx)
{
  free(ctx);
}

int argparse_fetch(argparse_ctx_t* ctx)
{
  if (ctx->idx >= ctx->argc)
    return ARGPARSE_EOA;

  const char* arg = ctx->argv[ctx->idx++];

  if (strncmp("--", arg, 2) == 0)
  {
    for (size_t i = 0; i < ctx->opt_count; ++i)
      if (ctx->opts[i].long_name != NULL)
        if (strcmp(arg + 2, ctx->opts[i].long_name) == 0)
          return ctx->opts[i].id;

    return ARGPARSE_UNKNOWN;
  }

  if (strncmp("-", arg, 1) == 0)
  {
    for (size_t i = 0; i < ctx->opt_count; ++i)
      if (ctx->opts[i].short_name != NULL)
        if (strcmp(arg + 1, ctx->opts[i].short_name) == 0)
          return ctx->opts[i].id;

    return ARGPARSE_UNKNOWN;
  }

  return ARGPARSE_UNKNOWN;
}

size_t argparse_print_options(argparse_ctx_t* ctx, FILE* stream)
{
  size_t result = 0;

  for (size_t i = 0; i < ctx->opt_count; ++i)
  {
    const argparse_option_t* opt = ctx->opts + i;

    result += fprintf(stream, "\t");

    size_t line_len = 0;

    if (opt->short_name != NULL)
      line_len += fprintf(stream, "-%s", opt->short_name);

    if (opt->short_name != NULL && opt->long_name != NULL)
      line_len += fputs(", ", stream);

    if (opt->long_name != NULL)
      line_len += fprintf(stream, "--%s", opt->long_name);

    if (opt->value_name != NULL)
      line_len += fprintf(stream, " <%s>", opt->value_name);

    if (line_len < 8)
      line_len += fprintf(stream, "\t\t\t\t");
    else if (line_len < 16)
      line_len += fprintf(stream, "\t\t\t");
    else if (line_len < 24)
      line_len += fprintf(stream, "\t\t");
    else
      line_len += fprintf(stream, "\t");

    line_len += fprintf(stream, "%s\n", opt->description);

    result += line_len;
  }

  return result;
}

size_t argparse_get_index(argparse_ctx_t* ctx)
{
  return ctx->idx;
}

void argparse_set_index(argparse_ctx_t* ctx, size_t idx)
{
  ctx->idx = idx;
}

const char* argparse_get_arg(argparse_ctx_t* ctx)
{
  return ctx->argv[ctx->argc];
}

const char* argparse_get_arg_at(argparse_ctx_t* ctx, size_t idx)
{
  if (idx >= ctx->argc)
    return NULL;

  return ctx->argv[idx];
}

const char* argparse_next_arg(argparse_ctx_t* ctx)
{
  if (ctx->idx >= ctx->argc)
    return NULL;

  return ctx->argv[ctx->idx++];
}
