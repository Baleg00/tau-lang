/**
 * \file cli.h
 * 
 * \brief Command-line interface.
 * 
 * \details This utility library is used to simplify the process of parsing
 * command-line arguments provided to the program. It provides functions and
 * abstractions that handle the complexities of parsing and extracting values
 * from command-line arguments. You can easily define the expected arguments,
 * specify their types, handle optional or mandatory parameters, and retrieve
 * the values entered by the user. This library makes the development of
 * a command-line interface simple and helps enhance the user experience of
 * the program.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_CLI_H
#define TAU_CLI_H

#include <stddef.h>

#include "queue.h"

/**
 * \brief Enumeration of command-line argument types.
 */
typedef enum cli_type_e
{
  CLI_TYPE_INTEGER, // Integer type.
  CLI_TYPE_FLOAT, // Floating-point type.
  CLI_TYPE_BOOLEAN, // Boolean type.
  CLI_TYPE_STRING, // String type.
  CLI_TYPE_SINK, // Absorbs all following arguments.
} cli_type_t;

/**
 * \brief Command-line interface.
 */
typedef struct cli_s cli_t;

/**
 * \brief Command-line option.
 */
typedef struct cli_opt_s cli_opt_t;

/**
 * \brief Command-line option callback type.
 */
typedef void(*cli_callback_t)(cli_t*, queue_t*, cli_opt_t*, const char*, void*);

struct cli_opt_s
{
  cli_type_t type; // Option type.

  const char** names; // Array of option names (e.g. "--help", "-h").
  size_t name_count; // Number of option names.

  /**
   * Character indicating the expected number of arguments.
   * 
   * 'N' - exact number
   * '?' - zero or one
   * '+' - one or many
   * '*' - zero or many
  */
  char arg_num;
  size_t arg_max; // Maximum number of arguments.

  void* data; // Pointer to output buffer.
  void* default_data; // Pointer to default value or NULL.
  size_t* data_count; // Pointer to a size_t to be set as the number of arguments read or NULL.

  const char* desc; // Brief option description.

  cli_callback_t callback; // Pointer to function to be called when option is parsed or NULL.
  void* user_ptr; // Pointer to user provided data.
};

/**
 * \brief Initializes a command-line interface (CLI) with the specified options
 * and usages.
 * 
 * \param[out] cli Pointer to the CLI structure to be initialized.
 * \param[in] opts Array of CLI options.
 * \param[in] opt_count Number of CLI options in the array.
 * \param[in] usages Array of usage strings.
 * \param[in] usage_count Number of usage strings in the array.
 */
cli_t* cli_init(cli_opt_t* opts, size_t opt_count, const char* usages[], size_t usage_count);

/**
 * \brief Frees the resources used by the command-line interface (CLI).
 * 
 * \param[in] cli Pointer to the CLI structure to be freed.
 */
void cli_free(cli_t* cli);

/**
 * \brief Parses the command-line arguments according to the defined options.
 * 
 * \param[in] cli Pointer to the CLI structure.
 * \param[in] argc Number of command-line arguments.
 * \param[in] argv Array of command-line argument strings.
 */
void cli_parse(cli_t* cli, int argc, const char* argv[]);

/**
 * \brief Matches a command-line argument to a CLI option.
 * 
 * \param[in] cli Pointer to the CLI structure.
 * \param[in] arg The command-line argument string to match.
 * \returns Pointer to the matched CLI option, or NULL if no match is found.
 */
cli_opt_t* cli_match(cli_t* cli, const char* arg);

/**
 * \brief Parses the command-line arguments for an option with N values.
 * 
 * \param[in] opt Pointer to the CLI option.
 * \param[in] que Pointer to the queue of command-line arguments.
 */
void cli_parse_N(cli_opt_t* opt, queue_t* que);

/**
 * \brief Parses the command-line arguments for an optional option.
 * 
 * \param[in] opt Pointer to the CLI option.
 * \param[in] que Pointer to the queue of command-line arguments.
 */
void cli_parse_optional(cli_opt_t* opt, queue_t* que);

/**
 * \brief Parses the command-line arguments for an option with one or more values.
 * 
 * \param[in] opt Pointer to the CLI option.
 * \param[in] que Pointer to the queue of command-line arguments.
 */
void cli_parse_one_or_many(cli_opt_t* opt, queue_t* que);

/**
 * \brief Parses the command-line argument for an option with zero or more values.
 * 
 * \param[in] opt Pointer to the CLI option.
 * \param[in] que Pointer to the queue of command-line arguments.
 */
void cli_parse_any(cli_opt_t* opt, queue_t* que);

/**
 * \brief Callback function for the help option.
 * 
 * \param[in] cli Pointer to the CLI structure.
 * \param[in] que Pointer to the queue of command-line arguments.
 * \param[in] opt Pointer to the CLI option.
 * \param[in] arg The command-line argument string.
 * \param[in] user_ptr Pointer to user-defined data.
 */
void cli_help_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_ptr);

/**
 * \brief Callback function for the version option.
 * 
 * \param[in] cli Pointer to the CLI structure.
 * \param[in] que Pointer to the queue of command-line arguments.
 * \param[in] opt Pointer to the CLI option.
 * \param[in] arg The command-line argument string.
 * \param[in] user_ptr Pointer to user-defined data.
 */
void cli_version_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_ptr);

/**
 * \brief Callback function for the verbose option.
 * 
 * \param[in] cli Pointer to the CLI structure.
 * \param[in] que Pointer to the queue of command-line arguments.
 * \param[in] opt Pointer to the CLI option.
 * \param[in] arg The command-line argument string.
 * \param[in] user_ptr Pointer to user-defined data.
 */
void cli_verbose_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_ptr);

/**
 * \brief Callback function for a flag option.
 * 
 * \param[in] cli Pointer to the CLI structure.
 * \param[in] que Pointer to the queue of command-line arguments.
 * \param[in] opt Pointer to the CLI option.
 * \param[in] arg The command-line argument string.
 * \param[in] user_ptr Pointer to user-defined data.
 */
void cli_flag_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_ptr);

/**
 * \brief Initalizes an integer option.
 */
#define cli_opt_int(...)  (cli_opt_t){ CLI_TYPE_INTEGER, __VA_ARGS__ }

/**
 * \brief Initalizes a floating-point option.
 */
#define cli_opt_flt(...)  (cli_opt_t){ CLI_TYPE_FLOAT  , __VA_ARGS__ }

/**
 * \brief Initalizes a boolean option.
 */
#define cli_opt_bool(...) (cli_opt_t){ CLI_TYPE_BOOLEAN, __VA_ARGS__ }

/**
 * \brief Initalizes a string option.
 */
#define cli_opt_str(...)  (cli_opt_t){ CLI_TYPE_STRING , __VA_ARGS__ }

/**
 * \brief Initializes a help option.
 * 
 * \details Initializes a boolean option with "--help" and "-h" as names. If
 * present the option prints all command-line interface usages and option
 * descriptions to the standard output.
 * 
 * \returns New help option.
*/
#define cli_opt_help()\
  ((cli_opt_t){\
    .type = CLI_TYPE_BOOLEAN,\
    .names = (const char*[]){ "-h", "--help" },\
    .name_count = 2,\
    .arg_num = 'N',\
    .arg_max = 0,\
    .data = NULL,\
    .default_data = NULL,\
    .data_count = NULL,\
    .desc = "Display this message.",\
    .callback = cli_help_callback,\
    .user_ptr = NULL\
  })

/**
 * \brief Initializes a version option.
 * 
 * \details Initializes a boolean option with "--version" as name. If present
 * the option prints the program version represented by `VER` to the standard
 * output.
 * 
 * \param[in] VER String representation of program version.
 * \returns New version option.
*/
#define cli_opt_version(VER)\
  ((cli_opt_t){\
    .type = CLI_TYPE_BOOLEAN,\
    .names = (const char*[]){ "--version" },\
    .name_count = 1,\
    .arg_num = 'N',\
    .arg_max = 0,\
    .data = NULL,\
    .default_data = NULL,\
    .data_count = NULL,\
    .desc = "Display version.",\
    .callback = cli_version_callback,\
    .user_ptr = (VER)\
  })

/**
 * \brief Initializes a verbose option.
 * 
 * \details Initializes a boolean option with "--verbose" and "-v" as names. If
 * present the option sets the boolean variable pointed to by `DATA` to `true`.
 * 
 * \param[out] DATA Pointer to boolean variable.
 * \returns New verbose option.
*/
#define cli_opt_verbose(DATA)\
  ((cli_opt_t){\
    .type = CLI_TYPE_BOOLEAN,\
    .names = (const char*[]){ "-v", "--verbose" },\
    .name_count = 2,\
    .arg_num = 'N',\
    .arg_max = 0,\
    .data = NULL,\
    .default_data = NULL,\
    .data_count = NULL,\
    .desc = "Enable verbose mode.",\
    .callback = cli_verbose_callback,\
    .user_ptr = (DATA)\
  })

/**
 * \brief Initializes a sink.
 * 
 * \details The sink options absorbs at most `ARG_MAX` arguments if no other
 * options match.
 * 
 * \param[in] ARG_MAX Maximum number of arguments to be absorbed.
 * \param[out] DATA Pointer to buffer where the arguments will be written to or NULL.
 * \param[out] DATA_COUNT Pointer to counter to be set to the number of arguments read or NULL.
 * \param[out] CALLBACK Pointer to function to be called when an argument is read or NULL.
 * \param[in,out] USER_PTR Pointer to user provided data to be passed to callback function.
 * \returns New sink option.
*/
#define cli_opt_sink(ARG_MAX, DATA, DATA_COUNT, CALLBACK, USER_PTR)\
  ((cli_opt_t){\
    .type = CLI_TYPE_SINK,\
    .names = NULL,\
    .name_count = 0,\
    .arg_num = '\0',\
    .arg_max = (ARG_MAX),\
    .data = (DATA),\
    .default_data = NULL,\
    .data_count = (DATA_COUNT),\
    .desc = NULL,\
    .callback = (CALLBACK),\
    .user_ptr = (USER_PTR)\
  })

/**
 * \brief Initializes a flag option.
 * 
 * \details Initializes a boolean option which takes 0 positional arguments and
 * if present sets the boolean variable pointed to by `FLAG` to `true`.
 * 
 * \param[in] NAMES Array of option names.
 * \param[in] NAME_COUNT Number of option names.
 * \param[in] DESC Brief option description.
 * \param[out] FLAG Pointer to flag variable to be set if option is present.
 * \returns New flag option.
*/
#define cli_opt_flag(NAMES, NAME_COUNT, DESC, FLAG)\
  ((cli_opt_t){\
    .type = CLI_TYPE_BOOLEAN,\
    .names = (NAMES),\
    .name_count = (NAME_COUNT),\
    .arg_num = 'N',\
    .arg_max = 0,\
    .data = NULL,\
    .default_data = NULL,\
    .data_count = NULL,\
    .desc = (DESC),\
    .callback = cli_flag_callback,\
    .user_ptr = (FLAG)\
  })

/**
 * \brief Macro for initializing a list of option names.
 */
#define cli_names(...)\
  ((const char*[]){ __VA_ARGS__ })

#endif