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
#include "stages/lexer/token/token.h"

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
  ERROR_PARSER_EXPECTED_CALLING_CONVENTION,
  ERROR_PARSER_DEFAULT_PARAMETER_ORDER,
  ERROR_PARSER_MISSING_PAREN,
  ERROR_PARSER_MISSING_BRACKET,
  ERROR_PARSER_MISSING_UNARY_ARGUMENT,
  ERROR_PARSER_MISSING_BINARY_ARGUMENT,
  ERROR_PARSER_MISSING_CALLEE,
  ERROR_PARSER_INCONSISTENT_MATRIX_DIMENSIONS,

  ERROR_NAMERES_SYMBOL_COLLISION,
  ERROR_NAMERES_UNDEFINED_SYMBOL,
  ERROR_NAMERES_EXPECTED_EXPRESSION_SYMBOL,
  ERROR_NAMERES_EXPECTED_TYPENAME,
  ERROR_NAMERES_SHADOWED_SYMBOL,
  ERROR_NAMERES_NO_MEMBER,
  ERROR_NAMERES_PRIVATE_MEMBER,

  ERROR_TYPECHECK_EXPECTED_INTEGER,
  ERROR_TYPECHECK_EXPECTED_ARITHMETIC,
  ERROR_TYPECHECK_EXPECTED_BOOL,
  ERROR_TYPECHECK_EXPECTED_MUTABLE,
  ERROR_TYPECHECK_EXPECTED_OPTIONAL,
  ERROR_TYPECHECK_EXPECTED_POINTER,
  ERROR_TYPECHECK_EXPECTED_ARRAY,
  ERROR_TYPECHECK_EXPECTED_REFERENCE,
  ERROR_TYPECHECK_EXPECTED_VECTOR,
  ERROR_TYPECHECK_EXPECTED_MATRIX,
  ERROR_TYPECHECK_EXPECTED_INTEGER_OR_FLOAT,
  ERROR_TYPECHECK_INCOMPATIBLE_RETURN_TYPE,
  ERROR_TYPECHECK_TOO_MANY_FUNCTION_PARAMETERS,
  ERROR_TYPECHECK_TOO_FEW_FUNCTION_PARAMETERS,
  ERROR_TYPECHECK_NO_MEMBER,
  ERROR_TYPECHECK_PRIVATE_MEMBER,
  ERROR_TYPECHECK_ILLEGAL_CONVERSION,
  ERROR_TYPECHECK_INTEGER_LITERAL_TOO_LARGE,
  ERROR_TYPECHECK_INCOMPATIBLE_VECTOR_DIMENSIONS,
  ERROR_TYPECHECK_INCOMPATIBLE_MATRIX_DIMENSIONS,
  ERROR_TYPECHECK_INCOMPATIBLE_MATRIX_VECTOR_DIMENSIONS,

  ERROR_CTRLFLOW_BREAK_OUTSIDE_LOOP,
  ERROR_CTRLFLOW_CONTINUE_OUTSIDE_LOOP,
  ERROR_CTRLFLOW_RETURN_INSIDE_DEFER,
} error_kind_t;

/**
 * \brief Represents a compiler error.
 */
typedef struct error_info_t
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
      missing_callee,
      inconsistent_matrix_dimensions,
      ill_formed_integer,
      ill_formed_float,
      invalid_integer_suffix,
      unexpected_token,
      unknown_calling_convention,
      expected_calling_convention,
      undefined_symbol,
      expected_expression_symbol,
      expected_typename,
      expected_integer,
      expected_arithmetic,
      expected_bool,
      expected_mutable,
      expected_optional,
      expected_pointer,
      expected_array,
      expected_reference,
      expected_vector,
      expected_matrix,
      incompatible_return_type,
      too_many_function_parameters,
      too_few_function_parameters,
      no_member,
      private_member,
      illegal_conversion,
      integer_literal_too_large,
      incompatible_vector_dimensions,
      incompatible_matrix_dimensions,
      incompatible_matrix_vector_dimensions,
      expected_integer_or_float,
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
} error_info_t;

/**
 * \brief Represents a container for errors.
 */
typedef struct error_bag_t error_bag_t;

/**
 * \brief Prints an error to `stderr`.
 *
 * \param[in] error The error to be printed.
 */
void error_print(error_info_t error);

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
 * \brief Prints all errors contained in an error bag.
 *
 * \param[in] bag Pointer to the error bag to be used.
 */
void error_bag_print(const error_bag_t* bag);

/**
 * \brief Adds an error to the error bag.
 *
 * \param[in] bag Pointer to the error bag to be used.
 * \param[in] error The error to add to the bag.
 * \returns `true` if the error was added to the bag, `false` otherwise.
 */
bool error_bag_put(error_bag_t* bag, error_info_t error);

/**
 * \brief Takes an error out of the error bag.
 *
 * \param[in] bag Pointer to the error bag to be used.
 * \param[out] dst Pointer to memory to write the error into if the bag is not empty.
 * \returns `true` if an error was written to `dst`, `false` otherwise.
 */
bool error_bag_get(error_bag_t* restrict bag, error_info_t* restrict dst);

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

/**
 * \brief Adds an error to the error bag.
 *
 * \param[in] bag Pointer to the error bag to be used.
 * \param[in] path The path to the file that failed to open.
 */
void error_bag_put_compiler_file_open_failed(error_bag_t* bag, const char* path);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_lexer_unexpected_character(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_lexer_identifier_too_long(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_lexer_missing_single_quote(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_lexer_missing_double_quote(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_lexer_empty_character(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_lexer_missing_hex_digits(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_lexer_too_many_hex_digits(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_lexer_unknown_escape_sequence(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_lexer_ill_formed_integer(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_lexer_ill_formed_float(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_lexer_invalid_integer_suffix(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_parser_unexpected_token(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_parser_unknown_calling_convention(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_parser_expected_calling_convention(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] default_param_loc The location of the first default parameter.
 * \param[in] param_loc The location of invalid non-default parameter.
 */
void error_bag_put_parser_default_parameter_order(error_bag_t* bag, location_t default_param_loc, location_t param_loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_parser_missing_paren(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_parser_missing_bracket(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_parser_missing_unary_argument(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_parser_missing_callee(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_parser_missing_binary_argument(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_parser_inconsistent_matrix_dimensions(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] symbol_loc The location of the original symbol.
 * \param[in] new_symbol_loc The location of the colliding symbol.
 */
void error_bag_put_nameres_symbol_collision(error_bag_t* bag, location_t symbol_loc, location_t new_symbol_loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_nameres_undefined_symbol(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_nameres_expected_expression_symbol(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_nameres_expected_typename(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] shadowed_symbol_loc The location of the shadowed symbol.
 * \param[in] inner_symbol_loc The location of the symbol that shadows the other one.
 */
void error_bag_put_nameres_shadowed_symbol(error_bag_t* bag, location_t shadowed_symbol_loc, location_t inner_symbol_loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_nameres_no_member(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_nameres_private_member(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_expected_integer(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_expected_arithmetic(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_expected_bool(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_expected_mutable(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_expected_optional(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_expected_pointer(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_expected_array(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_expected_reference(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_expected_vector(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_expected_matrix(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_incompatible_return_type(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_too_many_function_parameters(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_too_few_function_parameters(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_no_member(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_private_member(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_illegal_conversion(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_integer_literal_too_large(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_incompatible_vector_dimensions(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_incompatible_matrix_dimensions(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_incompatible_matrix_vector_dimensions(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_typecheck_expected_integer_or_float(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_ctrlflow_break_outside_loop(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_ctrlflow_continue_outside_loop(error_bag_t* bag, location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void error_bag_put_ctrlflow_return_inside_defer(error_bag_t* bag, location_t loc);

TAU_EXTERN_C_END

#endif
