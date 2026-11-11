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
typedef enum tau_error_kind_t
{
  TAU_ERROR_COMPILER_FILE_OPEN_FAILED,

  TAU_ERROR_LEXER_UNEXPECTED_CHARACTER,
  TAU_ERROR_LEXER_IDENTIFIER_TOO_LONG,
  TAU_ERROR_LEXER_MISSING_SINGLE_QUOTE,
  TAU_ERROR_LEXER_MISSING_DOUBLE_QUOTE,
  TAU_ERROR_LEXER_EMPTY_CHARACTER,
  TAU_ERROR_LEXER_MISSING_HEX_DIGITS,
  TAU_ERROR_LEXER_TOO_MANY_HEX_DIGITS,
  TAU_ERROR_LEXER_UNKNOWN_ESCAPE_SEQUENCE,
  TAU_ERROR_LEXER_ILL_FORMED_INTEGER,
  TAU_ERROR_LEXER_ILL_FORMED_FLOAT,
  TAU_ERROR_LEXER_INVALID_INTEGER_SUFFIX,

  TAU_ERROR_PARSER_UNEXPECTED_TOKEN,
  TAU_ERROR_PARSER_UNKNOWN_CALLING_CONVENTION,
  TAU_ERROR_PARSER_EXPECTED_CALLING_CONVENTION,
  TAU_ERROR_PARSER_DEFAULT_PARAMETER_ORDER,
  TAU_ERROR_PARSER_MISSING_PAREN,
  TAU_ERROR_PARSER_MISSING_BRACKET,
  TAU_ERROR_PARSER_MISSING_UNARY_ARGUMENT,
  TAU_ERROR_PARSER_MISSING_BINARY_ARGUMENT,
  TAU_ERROR_PARSER_MISSING_CALLEE,
  TAU_ERROR_PARSER_INCONSISTENT_MATRIX_DIMENSIONS,

  TAU_ERROR_NAMERES_SYMBOL_COLLISION,
  TAU_ERROR_NAMERES_UNDEFINED_SYMBOL,
  TAU_ERROR_NAMERES_EXPECTED_EXPRESSION_SYMBOL,
  TAU_ERROR_NAMERES_EXPECTED_TYPENAME,
  TAU_ERROR_NAMERES_SHADOWED_SYMBOL,
  TAU_ERROR_NAMERES_NO_MEMBER,
  TAU_ERROR_NAMERES_PRIVATE_MEMBER,

  TAU_ERROR_TYPECHECK_EXPECTED_INTEGER,
  TAU_ERROR_TYPECHECK_EXPECTED_ARITHMETIC,
  TAU_ERROR_TYPECHECK_EXPECTED_BOOL,
  TAU_ERROR_TYPECHECK_EXPECTED_MUTABLE,
  TAU_ERROR_TYPECHECK_EXPECTED_OPTIONAL,
  TAU_ERROR_TYPECHECK_EXPECTED_POINTER,
  TAU_ERROR_TYPECHECK_EXPECTED_ARRAY,
  TAU_ERROR_TYPECHECK_EXPECTED_REFERENCE,
  TAU_ERROR_TYPECHECK_EXPECTED_VECTOR,
  TAU_ERROR_TYPECHECK_EXPECTED_MATRIX,
  TAU_ERROR_TYPECHECK_EXPECTED_INTEGER_OR_FLOAT,
  TAU_ERROR_TYPECHECK_INCOMPATIBLE_RETURN_TYPE,
  TAU_ERROR_TYPECHECK_TOO_MANY_FUNCTION_PARAMETERS,
  TAU_ERROR_TYPECHECK_TOO_FEW_FUNCTION_PARAMETERS,
  TAU_ERROR_TYPECHECK_NO_MEMBER,
  TAU_ERROR_TYPECHECK_PRIVATE_MEMBER,
  TAU_ERROR_TYPECHECK_ILLEGAL_CONVERSION,
  TAU_ERROR_TYPECHECK_INTEGER_LITERAL_TOO_LARGE,
  TAU_ERROR_TYPECHECK_INCOMPATIBLE_VECTOR_DIMENSIONS,
  TAU_ERROR_TYPECHECK_INCOMPATIBLE_MATRIX_DIMENSIONS,

  TAU_ERROR_CTRLFLOW_BREAK_OUTSIDE_LOOP,
  TAU_ERROR_CTRLFLOW_CONTINUE_OUTSIDE_LOOP,
  TAU_ERROR_CTRLFLOW_RETURN_INSIDE_DEFER,
} tau_error_kind_t;

/**
 * \brief Represents a compiler error.
 */
typedef struct tau_error_info_t
{
  tau_error_kind_t kind;

  union
  {
    struct
    {
      const char* path;
    } file_open_failed;

    struct
    {
      tau_location_t loc;
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
      expected_integer_or_float,
      break_outside_loop,
      continue_outside_loop,
      return_inside_defer;

    struct
    {
      tau_location_t default_param_loc;
      tau_location_t param_loc;
    } default_parameter_order;

    struct
    {
      tau_location_t tau_symbol_loc;
      tau_location_t new_symbol_loc;
    } tau_symbol_collision;

    struct
    {
      tau_location_t shadowed_symbol_loc;
      tau_location_t inner_symbol_loc;
    } shadowed_symbol;
  };
} tau_error_info_t;

/**
 * \brief Represents a container for errors.
 */
typedef struct tau_error_bag_t tau_error_bag_t;

/**
 * \brief Prints an error to `stderr`.
 *
 * \param[in] error The error to be printed.
 */
void tau_error_print(tau_error_info_t error);

/**
 * \brief Initializes a new error bag.
 *
 * \param[in] capacity The capacity of the bag.
 * \returns Pointer to the newly initialized error bag.
 */
tau_error_bag_t* tau_error_bag_init(size_t capacity);

/**
 * \brief Frees all memory associated with an error bag.
 *
 * \param[in] bag Pointer to the error bag to be freed.
 */
void tau_error_bag_free(tau_error_bag_t* bag);

/**
 * \brief Prints all errors contained in an error bag.
 *
 * \param[in] bag Pointer to the error bag to be used.
 */
void tau_error_bag_print(const tau_error_bag_t* bag);

/**
 * \brief Adds an error to the error bag.
 *
 * \param[in] bag Pointer to the error bag to be used.
 * \param[in] error The error to add to the bag.
 * \returns `true` if the error was added to the bag, `false` otherwise.
 */
bool tau_error_bag_put(tau_error_bag_t* bag, tau_error_info_t error);

/**
 * \brief Takes an error out of the error bag.
 *
 * \param[in] bag Pointer to the error bag to be used.
 * \param[out] dst Pointer to memory to write the error into if the bag is not empty.
 * \returns `true` if an error was written to `dst`, `false` otherwise.
 */
bool tau_error_bag_get(tau_error_bag_t* restrict bag, tau_error_info_t* restrict dst);

/**
 * \brief Checks whether the error bag is empty.
 *
 * \param[in] bag Pointer to the error bag to be used.
 * \returns `true` if the error bag is empty, `false` otherwise.
 */
bool tau_error_bag_empty(const tau_error_bag_t* bag);

/**
 * \brief Checks whether the error bag is full.
 *
 * \param[in] bag Pointer to the error bag to be used.
 * \returns `true` if the error bag is full, `false` otherwise.
 */
bool tau_error_bag_full(const tau_error_bag_t* bag);

/**
 * \brief Adds an error to the error bag.
 *
 * \param[in] bag Pointer to the error bag to be used.
 * \param[in] path The path to the file that failed to open.
 */
void tau_error_bag_put_compiler_file_open_failed(tau_error_bag_t* bag, const char* path);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_lexer_unexpected_character(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_lexer_identifier_too_long(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_lexer_missing_single_quote(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_lexer_missing_double_quote(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_lexer_empty_character(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_lexer_missing_hex_digits(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_lexer_too_many_hex_digits(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_lexer_unknown_escape_sequence(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_lexer_ill_formed_integer(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_lexer_ill_formed_float(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_lexer_invalid_integer_suffix(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_parser_unexpected_token(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_parser_unknown_calling_convention(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_parser_expected_calling_convention(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] default_param_loc The location of the first default parameter.
 * \param[in] param_loc The location of invalid non-default parameter.
 */
void tau_error_bag_put_parser_default_parameter_order(tau_error_bag_t* bag, tau_location_t default_param_loc, tau_location_t param_loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_parser_missing_paren(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_parser_missing_bracket(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_parser_missing_unary_argument(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_parser_missing_callee(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_parser_missing_binary_argument(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_parser_inconsistent_matrix_dimensions(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] tau_symbol_loc The location of the original symbol.
 * \param[in] new_symbol_loc The location of the colliding symbol.
 */
void tau_error_bag_put_nameres_symbol_collision(tau_error_bag_t* bag, tau_location_t tau_symbol_loc, tau_location_t new_symbol_loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_nameres_undefined_symbol(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_nameres_expected_expression_symbol(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_nameres_expected_typename(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] shadowed_symbol_loc The location of the shadowed symbol.
 * \param[in] inner_symbol_loc The location of the symbol that shadows the other one.
 */
void tau_error_bag_put_nameres_shadowed_symbol(tau_error_bag_t* bag, tau_location_t shadowed_symbol_loc, tau_location_t inner_symbol_loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_nameres_no_member(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_nameres_private_member(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_expected_integer(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_expected_arithmetic(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_expected_bool(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_expected_mutable(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_expected_optional(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_expected_pointer(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_expected_array(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_expected_reference(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_expected_vector(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_expected_matrix(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_incompatible_return_type(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_too_many_function_parameters(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_too_few_function_parameters(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_no_member(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_private_member(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_illegal_conversion(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_integer_literal_too_large(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_incompatible_vector_dimensions(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_incompatible_matrix_dimensions(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_typecheck_expected_integer_or_float(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_ctrlflow_break_outside_loop(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_ctrlflow_continue_outside_loop(tau_error_bag_t* bag, tau_location_t loc);

/**
 * \brief Adds a specific error to the error bag.
 *
 * \param[in] bag Pointer to the bag to be used.
 * \param[in] loc The location of the error.
 */
void tau_error_bag_put_ctrlflow_return_inside_defer(tau_error_bag_t* bag, tau_location_t loc);

TAU_EXTERN_C_END

#endif
