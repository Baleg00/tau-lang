/**
 * \file
 *
 * \brief Command-line argument parser interface.
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

#ifndef TAU_ARGPARSE_H
#define TAU_ARGPARSE_H

#include <stddef.h>
#include <stdio.h>

#include "utils/extern_c.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Creates a new option.
 *
 * \param[in] ID The identifier of the option.
 * \param[in] SHORT_NAME The short name (`-h`) of the option or NULL.
 * \param[in] LONG_NAME The long name (`--help`) of the option or NULL.
 * \param[in] VALUE_NAME The name of the value of the option or NULL.
 * \param[in] DESCRIPTION The decription of the option.
 */
#define ARGPARSE_OPTION(ID, SHORT_NAME, LONG_NAME, VALUE_NAME, DESCRIPTION)\
  {\
    .id          = (ID),\
    .short_name  = (SHORT_NAME),\
    .long_name   = (LONG_NAME),\
    .value_name  = (VALUE_NAME),\
    .description = (DESCRIPTION)\
  }

/**
 * \brief Identifier indicating end-of-arguments.
 */
#define ARGPARSE_EOA (-1)

/**
 * \brief Identifier indicating an unknown argument.
 */
#define ARGPARSE_UNKNOWN (-2)

/**
 * \brief Represents an option.
 */
typedef struct argparse_option_t
{
  const char* short_name;
  const char* long_name;
  const char* value_name;
  const char* description;
  int id;
} argparse_option_t;

/**
 * \brief Represents an argument parser context.
 */
typedef struct argparse_ctx_t argparse_ctx_t;

/**
 * \brief Initializes a new argument parser context.
 *
 * \param[in] opts Array of options.
 * \param[in] opt_count Number of options.
 * \param[in] argv Array of arguments.
 * \param[in] argc Number of arguments.
 * \returns Pointer to the newly initialized argument parser context.
 */
argparse_ctx_t* argparse_ctx_init(const argparse_option_t opts[], size_t opt_count, const char* argv[], int argc);

/**
 * \brief Frees all allocated memory associated with an argument parser context.
 *
 * \param[in] ctx Pointer to the argument parser context to be freed.
 */
void argparse_ctx_free(argparse_ctx_t* ctx);

/**
 * \brief Fetches the next option from the argument list.
 *
 * \param[in,out] ctx Pointer to the argument parser context to be used.
 * \returns The identifier of the fetched option.
 */
int argparse_fetch(argparse_ctx_t* ctx);

/**
 * \brief Prints all options to a stream.
 *
 * \param[in] ctx Pointer to the argument parser context to be used.
 * \param[in] stream Output stream to be used.
 * \returns The number of characters written to the stream.
 */
size_t argparse_print_options(argparse_ctx_t* ctx, FILE* stream);

/**
 * \brief Retrieves the current index within the argument list.
 *
 * \param[in] ctx Pointer to the argument parser context to be used.
 * \returns The current index.
 */
size_t argparse_get_index(argparse_ctx_t* ctx);

/**
 * \brief Sets the current index within the argument list.
 *
 * \param[in] ctx Pointer to the argument parser context to be used.
 */
void argparse_set_index(argparse_ctx_t* ctx, size_t idx);

/**
 * \brief Retrieves the current argument from the argument list.
 *
 * \param[in] ctx Pointer to the argument parser context to be used.
 * \returns The current argument.
 */
const char* argparse_get_arg(argparse_ctx_t* ctx);

/**
 * \brief Retrieves an argument from the argument list given its index.
 *
 * \param[in] ctx Pointer to the argument parser context to be used.
 * \param[in] idx The index of the argument.
 * \returns The argument at the specified index or NULL if the index is out of range.
 */
const char* argparse_get_arg_at(argparse_ctx_t* ctx, size_t idx);

/**
 * \brief Retrieves the current argument from the argument list and moves to the next.
 *
 * \param[in] ctx Pointer to the argument parser context to be used.
 * \returns The current argument or NULL if there are no more arguments.
 */
const char* argparse_next_arg(argparse_ctx_t* ctx);

TAU_EXTERN_C_END

#endif
