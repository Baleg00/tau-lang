/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/error.h"

#include "utils/common.h"
#include "utils/crumb.h"

struct tau_error_bag_t
{
  size_t capacity;
  size_t size;
  tau_error_info_t* data;
};

static void tau_error_print_helper_snippet(tau_location_t loc, const char* msg)
{
  tau_crumb_error_t* tau_crumb_error = tau_crumb_error_init(1, msg);
  tau_crumb_snippet_t* tau_crumb_snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(tau_crumb_error, tau_crumb_snippet);

  tau_crumb_error_print(tau_crumb_error);

  tau_crumb_error_free(tau_crumb_error);
}

static void tau_error_print_helper_snippet_with_label(tau_location_t loc, const char* msg, const char* label)
{
  tau_crumb_error_t* tau_crumb_error = tau_crumb_error_init(1, msg);
  tau_crumb_snippet_t* tau_crumb_snippet = tau_crumb_snippet_init(loc);

  tau_crumb_snippet_label(tau_crumb_snippet, label);
  tau_crumb_error_snippet(tau_crumb_error, tau_crumb_snippet);

  tau_crumb_error_print(tau_crumb_error);

  tau_crumb_error_free(tau_crumb_error);
}

static void tau_error_print_compiler_file_open_failed(tau_error_info_t error)
{
  tau_log_error("file", "Cannot open file: %s", error.file_open_failed.path);
}

static void tau_error_print_lexer_unexpected_character(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.unexpected_character.loc, "Unexpected character.");
}

static void tau_error_print_lexer_identifier_too_long(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.identifier_too_long.loc, "Identifier cannot exceed 255 characters.");
}

static void tau_error_print_lexer_missing_single_quote(tau_error_info_t error)
{
  tau_error_print_helper_snippet_with_label(error.missing_single_quote.loc, "Missing terminating single quote.", "Character literal starts here.");
}

static void tau_error_print_lexer_missing_double_quote(tau_error_info_t error)
{
  tau_error_print_helper_snippet_with_label(error.missing_single_quote.loc, "Missing terminating double quote.", "String literal starts here.");
}

static void tau_error_print_lexer_empty_character(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.empty_character.loc, "Empty character literal.");
}

static void tau_error_print_lexer_missing_hex_digits(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.missing_hex_digits.loc, "Missing hex digits in escape sequence.");
}

static void tau_error_print_lexer_too_many_hex_digits(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.too_many_hex_digits.loc, "Too many hex digits in escape sequence.");
}

static void tau_error_print_lexer_unknown_escape_sequence(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.unknown_escape_sequence.loc, "Unknown escape sequence.");
}

static void tau_error_print_lexer_ill_formed_integer(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.ill_formed_integer.loc, "Ill-formed integer literal.");
}

static void tau_error_print_lexer_ill_formed_float(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.ill_formed_float.loc, "Ill-formed float literal.");
}

static void tau_error_print_lexer_invalid_integer_suffix(tau_error_info_t error)
{
  tau_location_t loc = error.invalid_integer_suffix.loc;

  tau_crumb_error_t* tau_crumb_error = tau_crumb_error_init(1, "Invalid integer suffix.");
  tau_crumb_snippet_t* tau_crumb_snippet = tau_crumb_snippet_init(loc);

  tau_crumb_snippet_label(tau_crumb_snippet, "The suffix `%.*s` is not valid.", (int)loc.len, loc.ptr);
  tau_crumb_error_snippet(tau_crumb_error, tau_crumb_snippet);

  tau_crumb_error_print(tau_crumb_error);

  tau_crumb_error_free(tau_crumb_error);
}

static void tau_error_print_parser_unexpected_token(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.unexpected_token.loc, "Unexpected token.");
}

static void tau_error_print_parser_unknown_calling_convention(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.unknown_calling_convention.loc, "Unknown calling convention.");
}

static void tau_error_print_parser_expected_calling_convention(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_calling_convention.loc, "Expected calling convention.");
}

static void tau_error_print_parser_default_parameter_order(tau_error_info_t error)
{
  tau_crumb_error_t* tau_crumb_error = tau_crumb_error_init(1, "Default parameter followed by non-default parameter.");
  tau_crumb_snippet_t* tau_crumb_snippet_param = tau_crumb_snippet_init(error.default_parameter_order.param_loc);
  tau_crumb_snippet_t* tau_crumb_snippet_default_param = tau_crumb_snippet_init(error.default_parameter_order.default_param_loc);

  tau_crumb_snippet_label(tau_crumb_snippet_param, "Non-default parameter.");
  tau_crumb_snippet_subsnippet(tau_crumb_snippet_param, tau_crumb_snippet_default_param);
  tau_crumb_snippet_label(tau_crumb_snippet_default_param, "First default parameter appears here.");
  tau_crumb_error_snippet(tau_crumb_error, tau_crumb_snippet_param);

  tau_crumb_error_print(tau_crumb_error);

  tau_crumb_error_free(tau_crumb_error);
}

static void tau_error_print_parser_missing_paren(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.missing_paren.loc, "Missing closing parenthesis.");
}

static void tau_error_print_parser_missing_bracket(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.missing_bracket.loc, "Missing closing bracket.");
}

static void tau_error_print_parser_missing_unary_argument(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.missing_unary_argument.loc, "Missing unary argument.");
}

static void tau_error_print_parser_missing_binary_argument(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.missing_binary_argument.loc, "Missing binary argument.");
}

static void tau_error_print_parser_missing_callee(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.missing_callee.loc, "Missing callee.");
}

static void tau_error_print_parser_inconsistent_matrix_dimensions(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.inconsistent_matrix_dimensions.loc, "Inconsistent matrix dimensions.");
}

static void tau_error_print_nameres_symbol_collision(tau_error_info_t error)
{
  tau_location_t new_symbol_loc = error.tau_symbol_collision.new_symbol_loc;
  tau_location_t tau_symbol_loc = error.tau_symbol_collision.tau_symbol_loc;

  tau_crumb_error_t* tau_crumb_error = tau_crumb_error_init(1, "Symbol collision.");
  tau_crumb_snippet_t* tau_crumb_snippet_new_symbol = tau_crumb_snippet_init(new_symbol_loc);
  tau_crumb_snippet_t* tau_crumb_snippet_symbol = tau_crumb_snippet_init(tau_symbol_loc);

  tau_crumb_snippet_label(tau_crumb_snippet_new_symbol, "A symbol named `%.*s` already exists.", (int)new_symbol_loc.len, new_symbol_loc.ptr);
  tau_crumb_snippet_subsnippet(tau_crumb_snippet_new_symbol, tau_crumb_snippet_symbol);
  tau_crumb_snippet_label(tau_crumb_snippet_symbol, "First symbol named `%.*s` appears here.", (int)tau_symbol_loc.len, tau_symbol_loc.ptr);
  tau_crumb_error_snippet(tau_crumb_error, tau_crumb_snippet_new_symbol);

  tau_crumb_error_print(tau_crumb_error);

  tau_crumb_error_free(tau_crumb_error);
}

static void tau_error_print_nameres_undefined_symbol(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.undefined_symbol.loc, "Undefined symbol.");
}

static void tau_error_print_nameres_expected_expression_symbol(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_expression_symbol.loc, "Expected expression symbol.");
}

static void tau_error_print_nameres_expected_typename(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_typename.loc, "Expected typename.");
}

static void tau_error_print_nameres_shadowed_symbol(tau_error_info_t error)
{
  tau_location_t shadowed_symbol_loc = error.shadowed_symbol.shadowed_symbol_loc;
  tau_location_t inner_symbol_loc = error.shadowed_symbol.inner_symbol_loc;

  tau_crumb_error_t* tau_crumb_error = tau_crumb_error_init(1, "A symbol is shadowed by another one with the same name.");
  tau_crumb_snippet_t* tau_crumb_snippet_shadowed = tau_crumb_snippet_init(shadowed_symbol_loc);
  tau_crumb_snippet_t* tau_crumb_snippet_inner = tau_crumb_snippet_init(inner_symbol_loc);

  tau_crumb_snippet_label(tau_crumb_snippet_inner, "This symbol shadows another one in an outer scope.");
  tau_crumb_snippet_subsnippet(tau_crumb_snippet_inner, tau_crumb_snippet_shadowed);
  tau_crumb_snippet_label(tau_crumb_snippet_shadowed, "The shadowed symbol appears here.");
  tau_crumb_error_snippet(tau_crumb_error, tau_crumb_snippet_inner);

  tau_crumb_error_print(tau_crumb_error);

  tau_crumb_error_free(tau_crumb_error);
}

static void tau_error_print_typecheck_expected_integer(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_integer.loc, "Expected an integer.");
}

static void tau_error_print_typecheck_expected_arithmetic(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_arithmetic.loc, "Expected an arithmetic.");
}

static void tau_error_print_typecheck_expected_bool(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_bool.loc, "Expected a bool.");
}

static void tau_error_print_typecheck_expected_mutable(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_mutable.loc, "Expected a mutable.");
}

static void tau_error_print_typecheck_expected_optional(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_optional.loc, "Expected an optional.");
}

static void tau_error_print_typecheck_expected_pointer(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_pointer.loc, "Expected a pointer.");
}

static void tau_error_print_typecheck_expected_array(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_array.loc, "Expected an array.");
}

static void tau_error_print_typecheck_expected_reference(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_reference.loc, "Expected a reference.");
}

static void tau_error_print_typecheck_expected_vector(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_vector.loc, "Expected a vector.");
}

static void tau_error_print_typecheck_expected_matrix(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_matrix.loc, "Expected a matrix.");
}

static void tau_error_print_typecheck_expected_integer_or_float(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.expected_integer_or_float.loc, "Expected integer or float.");
}

static void tau_error_print_typecheck_incompatible_return_type(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.incompatible_return_type.loc, "Incompatible return type.");
}

static void tau_error_print_typecheck_too_many_function_parameters(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.too_many_function_parameters.loc, "Too many function parameters.");
}

static void tau_error_print_typecheck_too_few_function_parameters(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.too_few_function_parameters.loc, "Too few function parameters.");
}

static void tau_error_print_typecheck_no_member(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.no_member.loc, "No member with this name exists.");
}

static void tau_error_print_typecheck_private_member(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.private_member.loc, "Cannot access private member.");
}

static void tau_error_print_typecheck_illegal_conversion(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.illegal_conversion.loc, "Illegal type conversion.");
}

static void tau_error_print_typecheck_integer_literal_too_large(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.integer_literal_too_large.loc, "Integer literal too large.");
}

static void tau_error_print_typecheck_incompatible_vector_dimensions(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.incompatible_vector_dimensions.loc, "Incompatible vector dimensions.");
}

static void tau_error_print_typecheck_incompatible_matrix_dimensions(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.incompatible_matrix_dimensions.loc, "Incompatible matrix dimensions.");
}

static void tau_error_print_ctrlflow_break_outside_loop(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.break_outside_loop.loc, "Break statement not within a loop.");
}

static void tau_error_print_ctrlflow_continue_outside_loop(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.continue_outside_loop.loc, "Continue statement not within a loop.");
}

static void tau_error_print_ctrlflow_return_inside_defer(tau_error_info_t error)
{
  tau_error_print_helper_snippet(error.return_inside_defer.loc, "Return statement within a defer.");
}

void tau_error_print(tau_error_info_t error)
{
  switch (error.kind)
  {
  case TAU_ERROR_COMPILER_FILE_OPEN_FAILED:                tau_error_print_compiler_file_open_failed               (error); break;
  case TAU_ERROR_LEXER_UNEXPECTED_CHARACTER:               tau_error_print_lexer_unexpected_character              (error); break;
  case TAU_ERROR_LEXER_IDENTIFIER_TOO_LONG:                tau_error_print_lexer_identifier_too_long               (error); break;
  case TAU_ERROR_LEXER_MISSING_SINGLE_QUOTE:               tau_error_print_lexer_missing_single_quote              (error); break;
  case TAU_ERROR_LEXER_MISSING_DOUBLE_QUOTE:               tau_error_print_lexer_missing_double_quote              (error); break;
  case TAU_ERROR_LEXER_EMPTY_CHARACTER:                    tau_error_print_lexer_empty_character                   (error); break;
  case TAU_ERROR_LEXER_MISSING_HEX_DIGITS:                 tau_error_print_lexer_missing_hex_digits                (error); break;
  case TAU_ERROR_LEXER_TOO_MANY_HEX_DIGITS:                tau_error_print_lexer_too_many_hex_digits               (error); break;
  case TAU_ERROR_LEXER_UNKNOWN_ESCAPE_SEQUENCE:            tau_error_print_lexer_unknown_escape_sequence           (error); break;
  case TAU_ERROR_LEXER_ILL_FORMED_INTEGER:                 tau_error_print_lexer_ill_formed_integer                (error); break;
  case TAU_ERROR_LEXER_ILL_FORMED_FLOAT:                   tau_error_print_lexer_ill_formed_float                  (error); break;
  case TAU_ERROR_LEXER_INVALID_INTEGER_SUFFIX:             tau_error_print_lexer_invalid_integer_suffix            (error); break;
  case TAU_ERROR_PARSER_UNEXPECTED_TOKEN:                  tau_error_print_parser_unexpected_token                 (error); break;
  case TAU_ERROR_PARSER_UNKNOWN_CALLING_CONVENTION:        tau_error_print_parser_unknown_calling_convention       (error); break;
  case TAU_ERROR_PARSER_EXPECTED_CALLING_CONVENTION:       tau_error_print_parser_expected_calling_convention      (error); break;
  case TAU_ERROR_PARSER_DEFAULT_PARAMETER_ORDER:           tau_error_print_parser_default_parameter_order          (error); break;
  case TAU_ERROR_PARSER_MISSING_PAREN:                     tau_error_print_parser_missing_paren                    (error); break;
  case TAU_ERROR_PARSER_MISSING_BRACKET:                   tau_error_print_parser_missing_bracket                  (error); break;
  case TAU_ERROR_PARSER_MISSING_UNARY_ARGUMENT:            tau_error_print_parser_missing_unary_argument           (error); break;
  case TAU_ERROR_PARSER_MISSING_BINARY_ARGUMENT:           tau_error_print_parser_missing_binary_argument          (error); break;
  case TAU_ERROR_PARSER_MISSING_CALLEE:                    tau_error_print_parser_missing_callee                   (error); break;
  case TAU_ERROR_PARSER_INCONSISTENT_MATRIX_DIMENSIONS:    tau_error_print_parser_inconsistent_matrix_dimensions   (error); break;
  case TAU_ERROR_NAMERES_SYMBOL_COLLISION:                 tau_error_print_nameres_symbol_collision                (error); break;
  case TAU_ERROR_NAMERES_UNDEFINED_SYMBOL:                 tau_error_print_nameres_undefined_symbol                (error); break;
  case TAU_ERROR_NAMERES_EXPECTED_EXPRESSION_SYMBOL:       tau_error_print_nameres_expected_expression_symbol      (error); break;
  case TAU_ERROR_NAMERES_EXPECTED_TYPENAME:                tau_error_print_nameres_expected_typename               (error); break;
  case TAU_ERROR_NAMERES_SHADOWED_SYMBOL:                  tau_error_print_nameres_shadowed_symbol                 (error); break;
  case TAU_ERROR_TYPECHECK_EXPECTED_INTEGER:               tau_error_print_typecheck_expected_integer              (error); break;
  case TAU_ERROR_TYPECHECK_EXPECTED_ARITHMETIC:            tau_error_print_typecheck_expected_arithmetic           (error); break;
  case TAU_ERROR_TYPECHECK_EXPECTED_BOOL:                  tau_error_print_typecheck_expected_bool                 (error); break;
  case TAU_ERROR_TYPECHECK_EXPECTED_MUTABLE:               tau_error_print_typecheck_expected_mutable              (error); break;
  case TAU_ERROR_TYPECHECK_EXPECTED_OPTIONAL:              tau_error_print_typecheck_expected_optional             (error); break;
  case TAU_ERROR_TYPECHECK_EXPECTED_POINTER:               tau_error_print_typecheck_expected_pointer              (error); break;
  case TAU_ERROR_TYPECHECK_EXPECTED_ARRAY:                 tau_error_print_typecheck_expected_array                (error); break;
  case TAU_ERROR_TYPECHECK_EXPECTED_REFERENCE:             tau_error_print_typecheck_expected_reference            (error); break;
  case TAU_ERROR_TYPECHECK_EXPECTED_VECTOR:                tau_error_print_typecheck_expected_vector               (error); break;
  case TAU_ERROR_TYPECHECK_EXPECTED_MATRIX:                tau_error_print_typecheck_expected_matrix               (error); break;
  case TAU_ERROR_TYPECHECK_EXPECTED_INTEGER_OR_FLOAT:      tau_error_print_typecheck_expected_integer_or_float     (error); break;
  case TAU_ERROR_TYPECHECK_INCOMPATIBLE_RETURN_TYPE:       tau_error_print_typecheck_incompatible_return_type      (error); break;
  case TAU_ERROR_TYPECHECK_TOO_MANY_FUNCTION_PARAMETERS:   tau_error_print_typecheck_too_many_function_parameters  (error); break;
  case TAU_ERROR_TYPECHECK_TOO_FEW_FUNCTION_PARAMETERS:    tau_error_print_typecheck_too_few_function_parameters   (error); break;
  case TAU_ERROR_TYPECHECK_NO_MEMBER:                      tau_error_print_typecheck_no_member                     (error); break;
  case TAU_ERROR_TYPECHECK_PRIVATE_MEMBER:                 tau_error_print_typecheck_private_member                (error); break;
  case TAU_ERROR_TYPECHECK_ILLEGAL_CONVERSION:             tau_error_print_typecheck_illegal_conversion            (error); break;
  case TAU_ERROR_TYPECHECK_INTEGER_LITERAL_TOO_LARGE:      tau_error_print_typecheck_integer_literal_too_large     (error); break;
  case TAU_ERROR_TYPECHECK_INCOMPATIBLE_VECTOR_DIMENSIONS: tau_error_print_typecheck_incompatible_vector_dimensions(error); break;
  case TAU_ERROR_TYPECHECK_INCOMPATIBLE_MATRIX_DIMENSIONS: tau_error_print_typecheck_incompatible_matrix_dimensions(error); break;
  case TAU_ERROR_CTRLFLOW_BREAK_OUTSIDE_LOOP:              tau_error_print_ctrlflow_break_outside_loop             (error); break;
  case TAU_ERROR_CTRLFLOW_CONTINUE_OUTSIDE_LOOP:           tau_error_print_ctrlflow_continue_outside_loop          (error); break;
  case TAU_ERROR_CTRLFLOW_RETURN_INSIDE_DEFER:             tau_error_print_ctrlflow_return_inside_defer            (error); break;
  default: TAU_UNREACHABLE();
  }
}

tau_error_bag_t* tau_error_bag_init(size_t capacity)
{
  tau_error_bag_t* bag = (tau_error_bag_t*)malloc(sizeof(tau_error_bag_t));

  bag->capacity = capacity;
  bag->size = 0;
  bag->data = (tau_error_info_t*)malloc(bag->capacity * sizeof(tau_error_info_t));

  return bag;
}

void tau_error_bag_free(tau_error_bag_t* bag)
{
  free(bag->data);
  free(bag);
}

void tau_error_bag_print(const tau_error_bag_t* bag)
{
  for (size_t i = 0; i < bag->size; i++)
    tau_error_print(bag->data[i]);
}

bool tau_error_bag_put(tau_error_bag_t* bag, tau_error_info_t error)
{
  if (bag->size >= bag->capacity)
    return false;

  bag->data[bag->size++] = error;

  return true;
}

bool tau_error_bag_get(tau_error_bag_t* restrict bag, tau_error_info_t* restrict dst)
{
  if (bag->size == 0)
    return false;

  memcpy(dst, bag->data + --bag->size, sizeof(tau_error_info_t));

  return true;
}

bool tau_error_bag_empty(const tau_error_bag_t* bag)
{
  return bag->size == 0;
}

bool tau_error_bag_full(const tau_error_bag_t* bag)
{
  return bag->size == bag->capacity;
}

void tau_error_bag_put_compiler_file_open_failed(tau_error_bag_t* bag, const char* path)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_COMPILER_FILE_OPEN_FAILED,
    .file_open_failed = {
      .path = path
    }
  });
}

void tau_error_bag_put_lexer_unexpected_character(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_LEXER_UNEXPECTED_CHARACTER,
    .unexpected_character = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_lexer_identifier_too_long(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_LEXER_IDENTIFIER_TOO_LONG,
    .identifier_too_long = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_lexer_missing_single_quote(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_LEXER_MISSING_SINGLE_QUOTE,
    .missing_single_quote = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_lexer_missing_double_quote(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_LEXER_MISSING_DOUBLE_QUOTE,
    .missing_double_quote = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_lexer_empty_character(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_LEXER_EMPTY_CHARACTER,
    .empty_character = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_lexer_missing_hex_digits(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_LEXER_MISSING_HEX_DIGITS,
    .missing_hex_digits = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_lexer_too_many_hex_digits(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_LEXER_TOO_MANY_HEX_DIGITS,
    .too_many_hex_digits = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_lexer_unknown_escape_sequence(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_LEXER_UNKNOWN_ESCAPE_SEQUENCE,
    .unknown_escape_sequence = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_lexer_ill_formed_integer(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_LEXER_ILL_FORMED_INTEGER,
    .ill_formed_integer = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_lexer_ill_formed_float(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_LEXER_ILL_FORMED_FLOAT,
    .ill_formed_float = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_lexer_invalid_integer_suffix(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_LEXER_INVALID_INTEGER_SUFFIX,
    .invalid_integer_suffix = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_parser_unexpected_token(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_PARSER_UNEXPECTED_TOKEN,
    .unexpected_token = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_parser_unknown_calling_convention(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_PARSER_UNKNOWN_CALLING_CONVENTION,
    .unknown_calling_convention = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_parser_expected_calling_convention(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_PARSER_EXPECTED_CALLING_CONVENTION,
    .expected_calling_convention = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_parser_default_parameter_order(tau_error_bag_t* bag, tau_location_t default_param_loc, tau_location_t param_loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_PARSER_DEFAULT_PARAMETER_ORDER,
    .default_parameter_order = {
      .default_param_loc = default_param_loc,
      .param_loc = param_loc
    }
  });
}

void tau_error_bag_put_parser_missing_paren(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_PARSER_MISSING_PAREN,
    .missing_paren = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_parser_missing_bracket(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_PARSER_MISSING_BRACKET,
    .missing_bracket = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_parser_missing_unary_argument(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_PARSER_MISSING_UNARY_ARGUMENT,
    .missing_unary_argument = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_parser_missing_callee(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_PARSER_MISSING_CALLEE,
    .missing_callee = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_parser_missing_binary_argument(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_PARSER_MISSING_BINARY_ARGUMENT,
    .missing_binary_argument = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_parser_inconsistent_matrix_dimensions(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_PARSER_INCONSISTENT_MATRIX_DIMENSIONS,
    .inconsistent_matrix_dimensions = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_nameres_symbol_collision(tau_error_bag_t* bag, tau_location_t tau_symbol_loc, tau_location_t new_symbol_loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_NAMERES_SYMBOL_COLLISION,
    .tau_symbol_collision = {
      .tau_symbol_loc = tau_symbol_loc,
      .new_symbol_loc = new_symbol_loc
    }
  });
}

void tau_error_bag_put_nameres_undefined_symbol(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_NAMERES_UNDEFINED_SYMBOL,
    .undefined_symbol = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_nameres_expected_expression_symbol(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_NAMERES_EXPECTED_EXPRESSION_SYMBOL,
    .expected_expression_symbol = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_nameres_expected_typename(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_NAMERES_EXPECTED_TYPENAME,
    .expected_typename = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_nameres_shadowed_symbol(tau_error_bag_t* bag, tau_location_t shadowed_symbol_loc, tau_location_t inner_symbol_loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_NAMERES_SHADOWED_SYMBOL,
    .shadowed_symbol = {
      .shadowed_symbol_loc = shadowed_symbol_loc,
      .inner_symbol_loc = inner_symbol_loc
    }
  });
}

void tau_error_bag_put_nameres_no_member(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_NAMERES_NO_MEMBER,
    .no_member = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_nameres_private_member(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_NAMERES_PRIVATE_MEMBER,
    .private_member = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_expected_integer(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_EXPECTED_INTEGER,
    .expected_integer = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_expected_arithmetic(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_EXPECTED_ARITHMETIC,
    .expected_arithmetic = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_expected_bool(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_EXPECTED_BOOL,
    .expected_bool = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_expected_mutable(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_EXPECTED_MUTABLE,
    .expected_mutable = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_expected_optional(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_EXPECTED_OPTIONAL,
    .expected_optional = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_expected_pointer(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_EXPECTED_POINTER,
    .expected_pointer = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_expected_array(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_EXPECTED_ARRAY,
    .expected_array = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_expected_reference(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_EXPECTED_REFERENCE,
    .expected_reference = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_expected_vector(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_EXPECTED_VECTOR,
    .expected_vector = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_expected_matrix(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_EXPECTED_MATRIX,
    .expected_matrix = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_incompatible_return_type(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_INCOMPATIBLE_RETURN_TYPE,
    .incompatible_return_type = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_too_many_function_parameters(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_TOO_MANY_FUNCTION_PARAMETERS,
    .too_many_function_parameters = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_too_few_function_parameters(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_TOO_FEW_FUNCTION_PARAMETERS,
    .too_few_function_parameters = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_no_member(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_NO_MEMBER,
    .no_member = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_private_member(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_PRIVATE_MEMBER,
    .private_member = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_illegal_conversion(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_ILLEGAL_CONVERSION,
    .illegal_conversion = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_integer_literal_too_large(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_INTEGER_LITERAL_TOO_LARGE,
    .integer_literal_too_large = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_incompatible_vector_dimensions(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_INCOMPATIBLE_VECTOR_DIMENSIONS,
    .incompatible_vector_dimensions = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_incompatible_matrix_dimensions(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_INCOMPATIBLE_MATRIX_DIMENSIONS,
    .incompatible_matrix_dimensions = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_typecheck_expected_integer_or_float(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_TYPECHECK_EXPECTED_INTEGER_OR_FLOAT,
    .expected_integer_or_float = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_ctrlflow_break_outside_loop(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_CTRLFLOW_BREAK_OUTSIDE_LOOP,
    .break_outside_loop = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_ctrlflow_continue_outside_loop(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_CTRLFLOW_CONTINUE_OUTSIDE_LOOP,
    .continue_outside_loop = {
      .loc = loc
    }
  });
}

void tau_error_bag_put_ctrlflow_return_inside_defer(tau_error_bag_t* bag, tau_location_t loc)
{
  tau_error_bag_put(bag, (tau_error_info_t){
    .kind = TAU_ERROR_CTRLFLOW_RETURN_INSIDE_DEFER,
    .return_inside_defer = {
      .loc = loc
    }
  });
}

