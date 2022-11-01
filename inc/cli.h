#ifndef TAU_CLI_H
#define TAU_CLI_H

#include <stddef.h>
#include <stdbool.h>

#include "queue.h"

typedef enum cli_type_e
{
  CLI_TYPE_INTEGER,
  CLI_TYPE_FLOAT,
  CLI_TYPE_BOOLEAN,
  CLI_TYPE_STRING,
  CLI_TYPE_SINK,
} cli_type_t;

typedef struct cli_s cli_t;
typedef struct cli_opt_s cli_opt_t;

typedef void(*cli_callback_t)(cli_t*, queue_t*, cli_opt_t*, void*);

struct cli_opt_s
{
  cli_type_t type;

  const char** names;
  size_t name_count;

  char arg_num;
  size_t arg_max;

  void* data;
  void* default_data;
  size_t* data_count;

  const char* desc;

  cli_callback_t callback;
  void* user_ptr;
};

struct cli_s
{
  cli_opt_t* opts;
  size_t opt_count;

  const char** usages;
  size_t usage_count;
};

cli_t* cli_init(cli_opt_t* opts, size_t opt_count, const char* usages[], size_t usage_count);
void cli_free(cli_t* cli);
void cli_parse(cli_t* cli, int argc, const char* argv[]);

cli_opt_t* cli_match(cli_t* cli, const char* arg);

void cli_parse_N(cli_opt_t* opt, queue_t* que);
void cli_parse_optional(cli_opt_t* opt, queue_t* que);
void cli_parse_one_or_many(cli_opt_t* opt, queue_t* que);
void cli_parse_any(cli_opt_t* opt, queue_t* que);

void cli_help_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, void* user_ptr);
void cli_version_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, void* user_ptr);
void cli_verbose_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, void* user_ptr);
void cli_flag_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, void* user_ptr);

#define cli_opt_integer(...) (cli_opt_t){ CLI_TYPE_INTEGER, __VA_ARGS__ }
#define cli_opt_float(...)   (cli_opt_t){ CLI_TYPE_FLOAT  , __VA_ARGS__ }
#define cli_opt_boolean(...) (cli_opt_t){ CLI_TYPE_BOOLEAN, __VA_ARGS__ }
#define cli_opt_string(...)  (cli_opt_t){ CLI_TYPE_STRING , __VA_ARGS__ }

#define cli_opt_help() (cli_opt_t){ CLI_TYPE_BOOLEAN, (char*[]){ "-h", "--help" }, 2, 'N', 0, NULL, NULL, NULL, "Display this message.", cli_help_callback, NULL }
#define cli_opt_version(VER) (cli_opt_t){ CLI_TYPE_BOOLEAN, (char*[]){ "--version" }, 1, 'N', 0, NULL, NULL, NULL, "Display version.", cli_version_callback, VER }
#define cli_opt_verbose(DATA) (cli_opt_t){ CLI_TYPE_BOOLEAN, (char*[]){ "-v", "--verbose" }, 2, 'N', 0, NULL, NULL, NULL, "Enable verbose mode.", cli_verbose_callback, DATA }
#define cli_opt_sink(ARG_MAX, DATA, DATA_COUNT, CALLBACK, USER_PTR) (cli_opt_t){ CLI_TYPE_SINK, NULL, 0, '\0', ARG_MAX, DATA, NULL, DATA_COUNT, NULL, CALLBACK, USER_PTR }
#define cli_opt_flag(NAMES, NAME_COUNT, DESC, FLAG) (cli_opt_t){ CLI_TYPE_BOOLEAN, NAMES, NAME_COUNT, 'N', 0, NULL, NULL, NULL, DESC, cli_flag_callback, FLAG }

#endif