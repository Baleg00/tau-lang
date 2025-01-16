/**
 * \file
 *
 * \brief Compiler errors.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_ERROR_H
#define TAU_ERROR_H

#include <stdbool.h>

#include "stages/lexer/location.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Enumeration of compilation error kinds.
 */
typedef enum error_kind_t
{
  ERROR_COMPILER_FILE_OPEN_FAILED,

  ERROR_LEXER_UNEXPECTED_CHARACTER,
  ERROR_LEXER_IDENTIFIER_TOO_LONG,
  ERROR_LEXER_MISSING_SINGLE_QUOTE,
  ERROR_LEXER_MISSING_DOUBLE_QUOTE,
  ERROR_LEXER_EMPTY_CHARACTER,
  ERROR_LEXER_MISSING_HEX_DIGITS,
  ERROR_LEXER_TOO_MANY_HEX_DIGITS,
  ERROR_LEXER_UNKNOWN_ESCAPE_SEQUENCE,
  ERROR_LEXER_ILL_FORMED_INTEGER,
  ERROR_LEXER_ILL_FORMED_FLOAT,
  ERROR_LEXER_INVALID_INTEGER_SUFFIX,

  ERROR_PARSER_UNEXPECTED_TOKEN,
  ERROR_PARSER_UNKNOWN_CALLING_CONVENTION,
  ERROR_PARSER_DEFAULT_PARAMETER_ORDER,
  ERROR_PARSER_MISSING_PAREN,
  ERROR_PARSER_MISSING_BRACKET,
  ERROR_PARSER_MISSING_UNARY_ARGUMENT,
  ERROR_PARSER_MISSING_BINARY_ARGUMENT,

  ERROR_NAMERES_SYMBOL_COLLISION,
  ERROR_NAMERES_UNDEFINED_SYMBOL,
  ERROR_NAMERES_EXPECTED_EXPRESSION_SYMBOL,
  ERROR_NAMERES_EXPECTED_TYPENAME,
  ERROR_NAMERES_SHADOWED_SYMBOL,

  ERROR_TYPECHECK_EXPECTED_INTEGER,
  ERROR_TYPECHECK_EXPECTED_ARITHMETIC,
  ERROR_TYPECHECK_EXPECTED_BOOL,
  ERROR_TYPECHECK_EXPECTED_MUTABLE,
  ERROR_TYPECHECK_EXPECTED_OPTIONAL,
  ERROR_TYPECHECK_EXPECTED_POINTER,
  ERROR_TYPECHECK_EXPECTED_REFERENCE,
  ERROR_TYPECHECK_INCOMPATIBLE_RETURN_TYPE,
  ERROR_TYPECHECK_TOO_MANY_FUNCTION_PARAMETERS,
  ERROR_TYPECHECK_TOO_FEW_FUNCTION_PARAMETERS,
  ERROR_TYPECHECK_NO_MEMBER,
  ERROR_TYPECHECK_PRIVATE_MEMBER,
  ERROR_TYPECHECK_ILLEGAL_CONVERSION,
  ERROR_TYPECHECK_INTEGER_LITERAL_TOO_LARGE,

  ERROR_CTRLFLOW_BREAK_OUTSIDE_LOOP,
  ERROR_CTRLFLOW_CONTINUE_OUTSIDE_LOOP,
  ERROR_CTRLFLOW_RETURN_INSIDE_DEFER,
} error_kind_t;

/**
 * \brief Represents a compiler error.
 */
typedef struct error_t
{
  error_kind_t kind;

  union
  {
    struct
    {
      const char* path;
    } file_open_failed;

    struct
    {
      location_t loc;
    } unexpected_character,
      identifier_too_long,
      missing_single_quote,
      missing_double_quote,
      empty_character,
      empty_string,
      missing_hex_digits,
      too_many_hex_digits,
      unknown_escape_sequence,
      missing_paren,
      missing_bracket,
      missing_unary_argument,
      missing_binary_argument,
      ill_formed_integer,
      ill_formed_float,
      invalid_integer_suffix,
      unexpected_token,
      unknown_calling_convention,
      undefined_symbol,
      expected_expression_symbol,
      expected_typename,
      expected_integer,
      expected_arithmetic,
      expected_bool,
      expected_mutable,
      expected_optional,
      expected_pointer,
      expected_reference,
      incompatible_return_type,
      too_many_function_parameters,
      too_few_function_parameters,
      no_member,
      private_member,
      illegal_conversion,
      integer_literal_too_large,
      break_outside_loop,
      continue_outside_loop,
      return_inside_defer;

    struct
    {
      location_t default_param_loc;
      location_t param_loc;
    } default_parameter_order;

    struct
    {
      location_t symbol_loc;
      location_t new_symbol_loc;
    } symbol_collision;

    struct
    {
      location_t shadowed_symbol_loc;
      location_t inner_symbol_loc;
    } shadowed_symbol;
  };
} error_t;

/**
 * \brief Represents a container for errors.
 */
typedef struct error_bag_t error_bag_t;

/**
 * \brief Prints an error to `stderr`.
 *
 * \param[in] error The error to be printed.
 */
void error_print(error_t error);

/**
 * \brief Initializes a new error bag.
 *
 * \param[in] capacity The capacity of the bag.
 * \returns Pointer to the newly initialized error bag.
 */
error_bag_t* error_bag_init(size_t capacity);

/**
 * \brief Frees all memory associated with an error bag.
 *
 * \param[in] bag Pointer to the error bag to be freed.
 */
void error_bag_free(error_bag_t* bag);

/**
 * \brief Adds an error to the error bag.
 *
 * \param[in] bag Pointer to the error bag to be used.
 * \param[in] error The error to add to the bag.
 * \returns `true` if the error was added to the bag, `false` otherwise.
 */
bool error_bag_put(error_bag_t* bag, error_t error);

/**
 * \brief Takes an error out of the error bag.
 *
 * \param[in] bag Pointer to the error bag to be used.
 * \param[out] dst Pointer to memory to write the error into if the bag is not empty.
 * \returns `true` if an error was written to `dst`, `false` otherwise.
 */
bool error_bag_get(error_bag_t* restrict bag, error_t* restrict dst);

/**
 * \brief Checks whether the error bag is empty.
 *
 * \param[in] bag Pointer to the error bag to be used.
 * \returns `true` if the error bag is empty, `false` otherwise.
 */
bool error_bag_empty(const error_bag_t* bag);

/**
 * \brief Checks whether the error bag is full.
 *
 * \param[in] bag Pointer to the error bag to be used.
 * \returns `true` if the error bag is full, `false` otherwise.
 */
bool error_bag_full(const error_bag_t* bag);

TAU_EXTERN_C_END

#endif
