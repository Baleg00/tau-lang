/**
 * \file
 * 
 * Command-line interface library.
*/

#ifndef TAU_CLI_H
#define TAU_CLI_H

#include <stddef.h>

#include "typedefs.h"
#include "queue.h"

struct cli_opt_s
{
  cli_type_t type; // Option type

  const char** names; // Array of option names (e.g. "--help", "-h")
  size_t name_count; // Number of option names

  /**
   * Symbolic character indicating the expected number of arguments.
   * 
   * 'N' - exact number
   * '?' - zero or one
   * '+' - one or many
   * '*' - zero or many
  */
  char arg_num;
  size_t arg_max; // Maximum number of arguments

  void* data; // Pointer to output buffer
  void* default_data; // Pointer to default value or null pointer
  size_t* data_count; // Pointer to a size_t to be set as the number of arguments read or null pointer

  const char* desc; // Brief option description

  cli_callback_t callback; // Pointer to function to be called when option is encountered or null pointer
  void* user_ptr; // Pointer to user provided data
};

struct cli_s
{
  cli_opt_t* opts; // Array of command-line options
  size_t opt_count; // Number of command-line options

  const char** usages; // Array of command usage descriptions
  size_t usage_count; // Number of command usage descriptions
};

/**
 * \brief Initializes a command-line interface instance with the specified parameters.
 * 
 * \param[out] cli Command-line interface instance to be initialized.
 * \param[in] opts Array of command-line options.
 * \param[in] opt_count Number of command-line options.
 * \param[in] usages Array of command usage descriptions.
 * \param[in] usage_count Number of command usage descriptions.
*/
void cli_init(cli_t* cli, cli_opt_t* opts, size_t opt_count, const char* usages[], size_t usage_count);

/** Destroys a command-line interface instance. */
void cli_free(cli_t* cli);

/**
 * \brief Parses an array of arguments using a command-line interface instance.
 * 
 * \param[in,out] cli Command-line interface instance to be used.
 * \param[in] argc Argument count.
 * \param[in] argv Array of command-line arguments.
*/
void cli_parse(cli_t* cli, int argc, const char* argv[]);

/**
 * \brief Matches an argument with an option using a command-line interface instance.
 * 
 * \param[in] cli ommand-line interface instance to be used.
 * \param[in] arg Argument.
 * \returns Pointer to a command-line option or null pointer if no matches were found.
*/
cli_opt_t* cli_match(cli_t* cli, const char* arg);

/**
 * \brief Parses exactly `N` arguments.
 * 
 * \param[in,out] opt Command-line option to be used.
 * \param[in,out] que Queue of arguments.
*/
void cli_parse_N(cli_opt_t* opt, queue_t* que);

/**
 * \brief Parses zero or one argument.
 * 
 * \param[in,out] opt Command-line option to be used.
 * \param[in,out] que Queue of arguments.
*/
void cli_parse_optional(cli_opt_t* opt, queue_t* que);

/**
 * \brief Parses one or many arguments.
 * 
 * \param[in,out] opt Command-line option to be used.
 * \param[in,out] que Queue of arguments.
*/
void cli_parse_one_or_many(cli_opt_t* opt, queue_t* que);

/**
 * \brief Parses zero or many arguments.
 * 
 * \param[in,out] opt Command-line option to be used.
 * \param[in,out] que Queue of arguments.
*/
void cli_parse_any(cli_opt_t* opt, queue_t* que);

/**
 * \brief Built-in callback funciton to be used for the "--help" option.
 * 
 * \details Prints all command-line interface usages and option descriptions.
 * 
 * \param[in] cli Command-line interface instance.
 * \param[in] que Queue of arguments. (ignored)
 * \param[in] opt Matched option. (ignored)
 * \param[in] arg Matched argument string. (ignored)
 * \param[in] user_ptr Pointer to user provided data. (ignored)
*/
void cli_help_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_ptr);

/**
 * \brief Built-in callback funciton to be used for the "--version" option.
 * 
 * \details `user_ptr` is interpreted as a string representation of the version
 * to be printed if this option is present.
 * 
 * \param[in] cli Command-line interface instance. (ignored)
 * \param[in] que Queue of arguments. (ignored)
 * \param[in] opt Matched option. (ignored)
 * \param[in] arg Matched argument string. (ignored)
 * \param[in] user_ptr Pointer to version string.
*/
void cli_version_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_ptr);

/**
 * \brief Built-in callback funciton to be used for the "--verbose" option.
 * 
 * \details `user_ptr` is interpreted as a pointer to bool to be set to true
 * if this option is present.
 * 
 * \param[in] cli Command-line interface instance. (ignored)
 * \param[in] que Queue of arguments. (ignored)
 * \param[in] opt Matched option. (ignored)
 * \param[in] arg Matched argument string. (ignored)
 * \param[out] user_ptr Pointer to flag variable.
*/
void cli_verbose_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_ptr);

/**
 * \brief Built-in callback funciton to be used for flags.
 * 
 * \details `user_ptr` is interpreted as a pointer to bool to be set to true
 * if this option is present.
 * 
 * \param[in] cli Command-line interface instance. (ignored)
 * \param[in] que Queue of arguments. (ignored)
 * \param[in] opt Matched option. (ignored)
 * \param[in] arg Matched argument string. (ignored)
 * \param[out] user_ptr Pointer to flag variable.
*/
void cli_flag_callback(cli_t* cli, queue_t* que, cli_opt_t* opt, const char* arg, void* user_ptr);

/** Shorthand macro for initalizing an integer option. */
#define cli_opt_int(...)  (cli_opt_t){ CLI_TYPE_INTEGER, __VA_ARGS__ }

/** Shorthand macro for initializing a floating-point option. */
#define cli_opt_flt(...)  (cli_opt_t){ CLI_TYPE_FLOAT  , __VA_ARGS__ }

/** Shorthand macro for initializing a boolean option. */
#define cli_opt_bool(...) (cli_opt_t){ CLI_TYPE_BOOLEAN, __VA_ARGS__ }

/** Shorthand macro for initializing a string option. */
#define cli_opt_str(...)  (cli_opt_t){ CLI_TYPE_STRING , __VA_ARGS__ }

/**
 * \brief Shorthand macro for initializing the help option.
 * 
 * \details Initializes a boolean option with "--help" and "-h" as names. The
 * option takes 0 positional arguments and prints all command-line interface
 * usages and option descriptions if option is present.
*/
#define cli_opt_help() (cli_opt_t){ CLI_TYPE_BOOLEAN, (const char*[]){ "-h", "--help" }, 2, 'N', 0, NULL, NULL, NULL, "Display this message.", cli_help_callback, NULL }

/**
 * \brief Shorthand macro for initializing the version option.
 * 
 * \details Initializes a boolean option with "--version" as name. The option
 * takes 0 positional arguments and prints the program version pointed to by
 * `VER` if option is present.
 * 
 * \param[in] VER String representation of program version.
 * \returns New option.
*/
#define cli_opt_version(VER) (cli_opt_t){ CLI_TYPE_BOOLEAN, (const char*[]){ "--version" }, 1, 'N', 0, NULL, NULL, NULL, "Display version.", cli_version_callback, VER }

/**
 * \brief Shorthand macro for initializing the verbose option.
 * 
 * \details Initializes a boolean option with "--verbose" and "-v" as names. The
 * option takes 0 positional arguments and sets the flag variable pointed to by
 * `DATA` to true if option is present.
 * 
 * \param[out] DATA Pointer to flag variable.
 * \returns New option.
*/
#define cli_opt_verbose(DATA) (cli_opt_t){ CLI_TYPE_BOOLEAN, (const char*[]){ "-v", "--verbose" }, 2, 'N', 0, NULL, NULL, NULL, "Enable verbose mode.", cli_verbose_callback, DATA }

/**
 * \brief Shorthand macro for initializing a sink.
 * 
 * \details Absorbs at most `ARG_MAX` of the remaining arguments.
 * 
 * \param[in] ARG_MAX Maximum number of arguments to be absorbed.
 * \param[out] DATA Pointer to buffer where the arguments will be written to or null pointer.
 * \param[out] DATA_COUNT Pointer to counter to be set as the number of arguments read or null pointer.
 * \param[out] CALLBACK Pointer to function to be called when an argument is read or null pointer.
 * \param[in,out] USER_PTR Pointer to user provided data to be passed to callback or null pointer.
 * \returns New option.
*/
#define cli_opt_sink(ARG_MAX, DATA, DATA_COUNT, CALLBACK, USER_PTR) (cli_opt_t){ CLI_TYPE_SINK, NULL, 0, '\0', ARG_MAX, DATA, NULL, DATA_COUNT, NULL, CALLBACK, USER_PTR }

/**
 * \brief Shorthand macro for initializing a flag option.
 * 
 * \param[in] NAMES Array of option names.
 * \param[in] NAME_COUNT Number of option names.
 * \param[in] DESC Brief option description.
 * \param[out] FLAG Pointer to flag variable to be set if option is present.
 * \returns New option.
*/
#define cli_opt_flag(NAMES, NAME_COUNT, DESC, FLAG) (cli_opt_t){ CLI_TYPE_BOOLEAN, NAMES, NAME_COUNT, 'N', 0, NULL, NULL, NULL, DESC, cli_flag_callback, FLAG }

/** Utility macro for making lists of option names. */
#define cli_names(...) ((const char*[]){ __VA_ARGS__ })

#endif