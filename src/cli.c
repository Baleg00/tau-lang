#include "cli.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

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

  for (int i = 1; i < argc; ++i)
    queue_offer(que, (char*)argv[i]);

  while (!queue_empty(que))
  {
    const char* arg = (const char*)queue_poll(que);

    cli_opt_t* opt;
    if ((opt = cli_match(cli, arg)) != NULL)
    {
      switch (opt->arg_num)
      {
      case 'N': cli_parse_N(opt, que); break;
      case '?': cli_parse_optional(opt, que); break;
      case '+': cli_parse_one_or_many(opt, que); break;
      case '*': cli_parse_any(opt, que); break;
      default: assert(("Invalid option cardinality.", false));
      }

      if (opt->callback != NULL)
        opt->callback(cli, que, opt, opt->user_ptr);
    }
    else if (cli->opts[cli->opt_count - 1].type == CLI_TYPE_SINK)
    {
      opt = &cli->opts[cli->opt_count - 1];
      
      assert(("Not enough memory to sink argument.", opt->arg_max != 0));

      if (opt->callback != NULL)
        opt->callback(cli, que, opt, opt->user_ptr);

      *((const char**)opt->data)++ = arg;
      --opt->arg_max;
    }
    else
      assert(("No matching option or sink.", false));
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

    if (arg == NULL || arg[0] != '-')
    {
      assert(("Too few arguments for option.", opt->default_data != NULL));

      for (; i < opt->arg_max; ++i)
        switch (opt->type)
        {
        case CLI_TYPE_INTEGER: *((int*)opt->data + i) = *(int*)opt->default_data; break;
        case CLI_TYPE_FLOAT: *((float*)opt->data + i) = *(float*)opt->default_data; break;
        case CLI_TYPE_BOOLEAN: *((bool*)opt->data + i) = *(bool*)opt->default_data; break;
        case CLI_TYPE_STRING: *((const char**)opt->data + i) = *(const char**)opt->default_data; break;
        default: assert(("Invalid option type.", false));
        }

      return;
    }

    switch (opt->type)
    {
    case CLI_TYPE_INTEGER: *((int*)opt->data + i) = atoi(arg); break;
    case CLI_TYPE_FLOAT: *((float*)opt->data + i) = (float)atof(arg); break;
    case CLI_TYPE_BOOLEAN: *((bool*)opt->data + i) = strcmp(arg, "true") == 0 || strcmp(arg, "on") == 0; break;
    case CLI_TYPE_STRING: *((const char**)opt->data + i) = arg; break;
    default: assert(("Invalid option type.", false));
    }
  }
}

void cli_parse_optional(cli_opt_t* opt, queue_t* que)
{
  const char* arg = NULL;

  if (!queue_empty(que))
    arg = (const char*)queue_peek(que);

  if (arg != NULL && arg[0] != '-')
  {
    queue_poll(que);

    if (opt->data_count != NULL)
      *opt->data_count = 1;

    switch (opt->type)
    {
    case CLI_TYPE_INTEGER: *(int*)opt->data = atoi(arg); break;
    case CLI_TYPE_FLOAT: *(float*)opt->data = (float)atof(arg); break;
    case CLI_TYPE_BOOLEAN: *(bool*)opt->data = strcmp(arg, "true") == 0 || strcmp(arg, "on") == 0; break;
    case CLI_TYPE_STRING: *(const char**)opt->data = arg; break;
    default: assert(("Invalid option type.", false));
    }

    return;
  }
  
  if (opt->default_data != NULL)
  {
    if (opt->data_count != NULL)
      *opt->data_count = 1;

    switch (opt->type)
    {
    case CLI_TYPE_INTEGER: *(int*)opt->data = *(int*)opt->default_data; break;
    case CLI_TYPE_FLOAT: *(float*)opt->data = *(float*)opt->default_data; break;
    case CLI_TYPE_BOOLEAN: *(bool*)opt->data = *(bool*)opt->default_data; break;
    case CLI_TYPE_STRING: *(const char**)opt->data = *(const char**)opt->default_data; break;
    default: assert(("Invalid option type.", false));
    }
  }
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
      assert(("Expected at least one argument.", count > 0));

      if (opt->data_count != NULL)
        *opt->data_count = count;

      return;
    }

    queue_poll(que);

    switch (opt->type)
    {
    case CLI_TYPE_INTEGER: *((int*)opt->data + count) = atoi(arg); break;
    case CLI_TYPE_FLOAT: *((float*)opt->data + count) = (float)atof(arg); break;
    case CLI_TYPE_BOOLEAN: *((bool*)opt->data + count) = strcmp(arg, "true") == 0 || strcmp(arg, "on") == 0; break;
    case CLI_TYPE_STRING: *((const char**)opt->data + count) = arg; break;
    default: assert(("Invalid option type.", false));
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
    case CLI_TYPE_INTEGER: *((int*)opt->data + count) = atoi(arg); break;
    case CLI_TYPE_FLOAT: *((float*)opt->data + count) = (float)atof(arg); break;
    case CLI_TYPE_BOOLEAN: *((bool*)opt->data + count) = strcmp(arg, "true") == 0 || strcmp(arg, "on") == 0; break;
    case CLI_TYPE_STRING: *((const char**)opt->data + count) = arg; break;
    default: assert(("Invalid option type.", false));
    }
  }
}

void cli_help_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, void* user_ptr)
{
  printf("Usage:\n");

  for (size_t i = 0; i < cli->usage_count; ++i)
    printf("\t%s\n", cli->usages[i]);
  
  printf("\nOptions:\n");

  for (size_t i = 0; i < cli->opt_count; ++i)
  {
    if (cli->opts[i].type == CLI_TYPE_SINK)
      continue;

    putchar('\t');

    for (size_t j = 0; j < cli->opts[i].name_count; ++j)
    {
      printf(cli->opts[i].names[j]);

      if (j + 1 < cli->opts[i].name_count)
        printf(", ");
    }

    printf("\t\t%s\n", cli->opts[i].desc);
  }
}

void cli_version_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, void* user_ptr)
{
  printf("Version: %s\n", (const char*)user_ptr);
}
