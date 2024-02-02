/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/io/cli.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/common.h"
#include "utils/memory/memtrace.h"

struct cli_t
{
  cli_opt_t* opts; // Array of command-line options.
  size_t opt_count; // Number of command-line options.

  const char** usages; // Array of usage descriptions.
  size_t usage_count; // Number of usage descriptions.
};

cli_t* cli_init(cli_opt_t* opts, size_t opt_count, const char* usages[], size_t usage_count)
{
  cli_t* cli = (cli_t*)malloc(sizeof(cli_t));
  assert(cli != NULL);

  cli->opts = opts;
  cli->opt_count = opt_count;
  cli->usages = usages;
  cli->usage_count = usage_count;

  return cli;
}

void cli_free(cli_t* cli)
{
  free(cli);
}

void cli_parse(cli_t* cli, int argc, const char* argv[])
{
  queue_t* que = queue_init();

  // Push arguments to queue
  for (int i = 1; i < argc; ++i)
    queue_offer(que, (void*)argv[i]);

  // Process while there are arguments in the queue
  while (!queue_empty(que))
  {
    const char* arg = (const char*)queue_poll(que);

    cli_opt_t* opt;

    // Try to match argument with an option
    if ((opt = cli_match(cli, arg)) != NULL)
    {
      // Parse arguments
      switch (opt->arg_num)
      {
      case 'N': cli_parse_N(opt, que); break;
      case '?': cli_parse_optional(opt, que); break;
      case '+': cli_parse_one_or_many(opt, que); break;
      case '*': cli_parse_any(opt, que); break;
      default: unreachable();
      }

      if (opt->callback != NULL)
        opt->callback(cli, que, opt, arg, opt->user_ptr);
    }
    // Is option a sink? Sink option should be the last one if present
    else if (cli->opts[cli->opt_count - 1].type == CLI_TYPE_SINK)
    {
      opt = &cli->opts[cli->opt_count - 1];
      
      // Skip if can't sink argument
      if (opt->arg_max == 0)
      {
        log_warn("Not enough memory to sink argument: %s", arg);
        continue;
      }

      if (opt->callback != NULL)
        opt->callback(cli, que, opt, arg, opt->user_ptr);

      if (opt->data != NULL)
      {
        const char** data = (const char**)opt->data;
        *(data++) = arg;
        opt->data = data;
      }

      --opt->arg_max;

      if (opt->data_count != NULL)
        ++(*opt->data_count);
    }
    // Skip if no options matched and there is no sink
    else
      log_warn("cli", "No matching option or sink: %s", arg);
  }

  queue_free(que);
}

cli_opt_t* cli_match(cli_t* cli, const char* arg)
{
  for (size_t i = 0; i < cli->opt_count; ++i)
    for (size_t j = 0; j < cli->opts[i].name_count; ++j)
      if (strcmp(arg, cli->opts[i].names[j]) == 0)
        return &cli->opts[i];
  
  return NULL;
}

void cli_parse_N(cli_opt_t* opt, queue_t* que)
{
  for (size_t i = 0; i < opt->arg_max; ++i)
  {
    const char* arg = NULL;

    if (!queue_empty(que))
      arg = (const char*)queue_poll(que);

    // There are no more arguments
    if (arg == NULL || arg[0] == '-')
    {
      // Break if no default value was provided
      if (opt->default_data == NULL)
      {
        log_error("cli", "Too few arguments for option: %s", opt->names[0]);
        break;
      }

      // Fill remaining buffer with default values
      for (; i < opt->arg_max; ++i)
        switch (opt->type)
        {
        case CLI_TYPE_INTEGER: *((int*)        opt->data + i) = *(int*)        opt->default_data; break;
        case CLI_TYPE_FLOAT:   *((float*)      opt->data + i) = *(float*)      opt->default_data; break;
        case CLI_TYPE_BOOLEAN: *((bool*)       opt->data + i) = *(bool*)       opt->default_data; break;
        case CLI_TYPE_STRING:  *((const char**)opt->data + i) = *(const char**)opt->default_data; break;
        default: unreachable();
        }

      return;
    }

    switch (opt->type)
    {
    case CLI_TYPE_INTEGER: *((int*)        opt->data + i) = atoi(arg); break;
    case CLI_TYPE_FLOAT:   *((float*)      opt->data + i) = (float)atof(arg); break;
    case CLI_TYPE_BOOLEAN: *((bool*)       opt->data + i) = strcmp(arg, "true") == 0 || strcmp(arg, "on") == 0; break;
    case CLI_TYPE_STRING:  *((const char**)opt->data + i) = arg; break;
    default: unreachable();
    }
  }
}

void cli_parse_optional(cli_opt_t* opt, queue_t* que)
{
  const char* arg = NULL;

  if (!queue_empty(que))
    arg = (const char*)queue_peek(que);

  // There is an argument
  if (arg != NULL && arg[0] != '-')
  {
    queue_poll(que);

    if (opt->data_count != NULL)
      *opt->data_count = 1;

    switch (opt->type)
    {
    case CLI_TYPE_INTEGER: *(int*)        opt->data = atoi(arg); break;
    case CLI_TYPE_FLOAT:   *(float*)      opt->data = (float)atof(arg); break;
    case CLI_TYPE_BOOLEAN: *(bool*)       opt->data = strcmp(arg, "true") == 0 || strcmp(arg, "on") == 0; break;
    case CLI_TYPE_STRING:  *(const char**)opt->data = arg; break;
    default: unreachable();
    }

    return;
  }
  
  // There is no argument but default value was provided
  if (opt->default_data != NULL)
  {
    if (opt->data_count != NULL)
      *opt->data_count = 1;

    switch (opt->type)
    {
    case CLI_TYPE_INTEGER: *(int*)        opt->data = *(int*)        opt->default_data; break;
    case CLI_TYPE_FLOAT:   *(float*)      opt->data = *(float*)      opt->default_data; break;
    case CLI_TYPE_BOOLEAN: *(bool*)       opt->data = *(bool*)       opt->default_data; break;
    case CLI_TYPE_STRING:  *(const char**)opt->data = *(const char**)opt->default_data; break;
    default: unreachable();
    }
  }
  // There is no argument and no default value was provided
  else if (opt->data_count != NULL)
    *opt->data_count = 0;
}

void cli_parse_one_or_many(cli_opt_t* opt, queue_t* que)
{
  for (size_t count = 0; count < opt->arg_max; ++count)
  {
    const char* arg = NULL;

    if (!queue_empty(que))
      arg = (const char*)queue_peek(que);
    
    if (arg == NULL || arg[0] == '-')
    {
      if (count == 0)
      {
        log_error("cli", "Expected at least one argument: %s", opt->names[0]);
        return;
      }

      if (opt->data_count != NULL)
        *opt->data_count = count;

      return;
    }

    queue_poll(que);

    switch (opt->type)
    {
    case CLI_TYPE_INTEGER: *((int*)        opt->data + count) = atoi(arg); break;
    case CLI_TYPE_FLOAT:   *((float*)      opt->data + count) = (float)atof(arg); break;
    case CLI_TYPE_BOOLEAN: *((bool*)       opt->data + count) = strcmp(arg, "true") == 0 || strcmp(arg, "on") == 0; break;
    case CLI_TYPE_STRING:  *((const char**)opt->data + count) = arg; break;
    default: unreachable();
    }
  }
}

void cli_parse_any(cli_opt_t* opt, queue_t* que)
{
  for (size_t count = 0; count < opt->arg_max; ++count)
  {
    const char* arg = NULL;

    if (!queue_empty(que))
      arg = (const char*)queue_peek(que);
    
    if (arg == NULL || arg[0] == '-')
    {
      if (opt->data_count != NULL)
        *opt->data_count = count;

      return;
    }

    queue_poll(que);

    switch (opt->type)
    {
    case CLI_TYPE_INTEGER: *((int*)        opt->data + count) = atoi(arg); break;
    case CLI_TYPE_FLOAT:   *((float*)      opt->data + count) = (float)atof(arg); break;
    case CLI_TYPE_BOOLEAN: *((bool*)       opt->data + count) = strcmp(arg, "true") == 0 || strcmp(arg, "on") == 0; break;
    case CLI_TYPE_STRING:  *((const char**)opt->data + count) = arg; break;
    default: unreachable();
    }
  }
}

void cli_help_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_ptr)
{
  unused(que);
  unused(opt);
  unused(arg);
  unused(user_ptr);

  puts("Usage:");

  for (size_t i = 0; i < cli->usage_count; ++i)
    printf("\t%s\n", cli->usages[i]);
  
  puts("\nOptions:");

  for (size_t i = 0; i < cli->opt_count; ++i)
  {
    if (cli->opts[i].type == CLI_TYPE_SINK)
      continue;

    putchar('\t');

    for (size_t j = 0; j < cli->opts[i].name_count; ++j)
    {
      printf("%s", cli->opts[i].names[j]);

      if (j + 1 < cli->opts[i].name_count)
        printf(", ");
    }

    printf("\t\t%s\n", cli->opts[i].desc);
  }

  exit(EXIT_SUCCESS);
}

void cli_version_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_ptr)
{
  unused(cli);
  unused(que);
  unused(opt);
  unused(arg);

  printf("Version: %s\n", (const char*)user_ptr);
}

void cli_verbose_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_ptr)
{
  unused(cli);
  unused(que);
  unused(opt);
  unused(arg);

  *(bool*)user_ptr = true;
}

void cli_flag_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_ptr)
{
  unused(cli);
  unused(que);
  unused(opt);
  unused(arg);

  *(bool*)user_ptr = true;
}
