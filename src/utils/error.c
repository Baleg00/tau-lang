/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/error.h"

#include "utils/common.h"
#include "utils/crumb.h"

struct error_bag_t
{
  size_t capacity;
  size_t size;
  error_t* data;
};

static void error_print_helper_snippet(location_t loc, const char* msg)
{
  crumb_error_t* crumb_error = crumb_error_init(1, msg);
  crumb_snippet_t* crumb_snippet = crumb_snippet_init(loc);

  crumb_error_snippet(crumb_error, crumb_snippet);

  crumb_error_print(crumb_error);

  crumb_error_free(crumb_error);
}

static void error_print_helper_snippet_with_label(location_t loc, const char* msg, const char* label)
{
  crumb_error_t* crumb_error = crumb_error_init(1, msg);
  crumb_snippet_t* crumb_snippet = crumb_snippet_init(loc);

  crumb_snippet_label(crumb_snippet, label);
  crumb_error_snippet(crumb_error, crumb_snippet);

  crumb_error_print(crumb_error);

  crumb_error_free(crumb_error);
}

static void error_print_compiler_file_open_failed(error_t error)
{
  log_error("file", "Cannot open file: %s", error.file_open_failed.path);
}

static void error_print_lexer_unexpected_character(error_t error)
{
  error_print_helper_snippet(error.unexpected_character.loc, "Unexpected character.");
}

static void error_print_lexer_identifier_too_long(error_t error)
{
  error_print_helper_snippet(error.identifier_too_long.loc, "Identifier cannot exceed 255 characters.");
}

static void error_print_lexer_missing_single_quote(error_t error)
{
  error_print_helper_snippet_with_label(error.missing_single_quote.loc, "Missing terminating single quote.", "Character literal starts here.");
}

static void error_print_lexer_missing_double_quote(error_t error)
{
  error_print_helper_snippet_with_label(error.missing_single_quote.loc, "Missing terminating double quote.", "String literal starts here.");
}

static void error_print_lexer_empty_character(error_t error)
{
  error_print_helper_snippet(error.empty_character.loc, "Empty character literal.");
}

static void error_print_lexer_missing_hex_digits(error_t error)
{
  error_print_helper_snippet(error.missing_hex_digits.loc, "Missing hex digits in escape sequence.");
}

static void error_print_lexer_too_many_hex_digits(error_t error)
{
  error_print_helper_snippet(error.too_many_hex_digits.loc, "Too many hex digits in escape sequence.");
}

static void error_print_lexer_unknown_escape_sequence(error_t error)
{
  error_print_helper_snippet(error.unknown_escape_sequence.loc, "Unknown escape sequence.");
}

static void error_print_lexer_ill_formed_integer(error_t error)
{
  error_print_helper_snippet(error.ill_formed_integer.loc, "Ill-formed integer literal.");
}

static void error_print_lexer_ill_formed_float(error_t error)
{
  error_print_helper_snippet(error.ill_formed_float.loc, "Ill-formed float literal.");
}

static void error_print_lexer_invalid_integer_suffix(error_t error)
{
  location_t loc = error.invalid_integer_suffix.loc;

  crumb_error_t* crumb_error = crumb_error_init(1, "Invalid integer suffix.");
  crumb_snippet_t* crumb_snippet = crumb_snippet_init(loc);

  crumb_snippet_label(crumb_snippet, "The suffix `%.*s` is not valid.", (int)loc.len, loc.ptr);
  crumb_error_snippet(crumb_error, crumb_snippet);

  crumb_error_print(crumb_error);

  crumb_error_free(crumb_error);
}

static void error_print_parser_unexpected_token(error_t error)
{
  error_print_helper_snippet(error.unexpected_token.loc, "Unexpected token.");
}

static void error_print_parser_unknown_calling_convention(error_t error)
{
  error_print_helper_snippet(error.unknown_calling_convention.loc, "Unknown calling convention.");
}

static void error_print_parser_expected_calling_convention(error_t error)
{
  error_print_helper_snippet(error.expected_calling_convention.loc, "Expected calling convention.");
}

static void error_print_parser_default_parameter_order(error_t error)
{
  crumb_error_t* crumb_error = crumb_error_init(1, "Default parameter followed by non-default parameter.");
  crumb_snippet_t* crumb_snippet_param = crumb_snippet_init(error.default_parameter_order.param_loc);
  crumb_snippet_t* crumb_snippet_default_param = crumb_snippet_init(error.default_parameter_order.default_param_loc);

  crumb_snippet_label(crumb_snippet_param, "Non-default parameter.");
  crumb_snippet_subsnippet(crumb_snippet_param, crumb_snippet_default_param);
  crumb_snippet_label(crumb_snippet_default_param, "First default parameter appears here.");
  crumb_error_snippet(crumb_error, crumb_snippet_param);

  crumb_error_print(crumb_error);

  crumb_error_free(crumb_error);
}

static void error_print_parser_missing_paren(error_t error)
{
  error_print_helper_snippet(error.missing_paren.loc, "Missing closing parenthesis.");
}

static void error_print_parser_missing_bracket(error_t error)
{
  error_print_helper_snippet(error.missing_bracket.loc, "Missing closing bracket.");
}

static void error_print_parser_missing_unary_argument(error_t error)
{
  error_print_helper_snippet(error.missing_unary_argument.loc, "Missing unary argument.");
}

static void error_print_parser_missing_binary_argument(error_t error)
{
  error_print_helper_snippet(error.missing_binary_argument.loc, "Missing binary argument.");
}

static void error_print_parser_missing_callee(error_t error)
{
  error_print_helper_snippet(error.missing_callee.loc, "Missing callee.");
}

static void error_print_nameres_symbol_collision(error_t error)
{
  location_t new_symbol_loc = error.symbol_collision.new_symbol_loc;
  location_t symbol_loc = error.symbol_collision.symbol_loc;

  crumb_error_t* crumb_error = crumb_error_init(1, "Symbol collision.");
  crumb_snippet_t* crumb_snippet_new_symbol = crumb_snippet_init(new_symbol_loc);
  crumb_snippet_t* crumb_snippet_symbol = crumb_snippet_init(symbol_loc);

  crumb_snippet_label(crumb_snippet_new_symbol, "A symbol named `%.*s` already exists.", (int)new_symbol_loc.len, new_symbol_loc.ptr);
  crumb_snippet_subsnippet(crumb_snippet_new_symbol, crumb_snippet_symbol);
  crumb_snippet_label(crumb_snippet_symbol, "First symbol named `%.*s` appears here.", (int)symbol_loc.len, symbol_loc.ptr);
  crumb_error_snippet(crumb_error, crumb_snippet_new_symbol);

  crumb_error_print(crumb_error);

  crumb_error_free(crumb_error);
}

static void error_print_nameres_undefined_symbol(error_t error)
{
  error_print_helper_snippet(error.undefined_symbol.loc, "Undefined symbol.");
}

static void error_print_nameres_expected_expression_symbol(error_t error)
{
  error_print_helper_snippet(error.expected_expression_symbol.loc, "Expected expression symbol.");
}

static void error_print_nameres_expected_typename(error_t error)
{
  error_print_helper_snippet(error.expected_typename.loc, "Expected typename.");
}

static void error_print_nameres_shadowed_symbol(error_t error)
{
  location_t shadowed_symbol_loc = error.shadowed_symbol.shadowed_symbol_loc;
  location_t inner_symbol_loc = error.shadowed_symbol.inner_symbol_loc;

  crumb_error_t* crumb_error = crumb_error_init(1, "A symbol is shadowed by another one with the same name.");
  crumb_snippet_t* crumb_snippet_shadowed = crumb_snippet_init(shadowed_symbol_loc);
  crumb_snippet_t* crumb_snippet_inner = crumb_snippet_init(inner_symbol_loc);

  crumb_snippet_label(crumb_snippet_inner, "This symbol shadows another one in an outer scope.");
  crumb_snippet_subsnippet(crumb_snippet_inner, crumb_snippet_shadowed);
  crumb_snippet_label(crumb_snippet_shadowed, "The shadowed symbol appears here.");
  crumb_error_snippet(crumb_error, crumb_snippet_inner);

  crumb_error_print(crumb_error);

  crumb_error_free(crumb_error);
}

static void error_print_typecheck_expected_integer(error_t error)
{
  error_print_helper_snippet(error.expected_integer.loc, "Expected an integer.");
}

static void error_print_typecheck_expected_arithmetic(error_t error)
{
  error_print_helper_snippet(error.expected_arithmetic.loc, "Expected an arithmetic.");
}

static void error_print_typecheck_expected_bool(error_t error)
{
  error_print_helper_snippet(error.expected_bool.loc, "Expected a bool.");
}

static void error_print_typecheck_expected_mutable(error_t error)
{
  error_print_helper_snippet(error.expected_mutable.loc, "Expected a mutable.");
}

static void error_print_typecheck_expected_optional(error_t error)
{
  error_print_helper_snippet(error.expected_optional.loc, "Expected an optional.");
}

static void error_print_typecheck_expected_pointer(error_t error)
{
  error_print_helper_snippet(error.expected_pointer.loc, "Expected a pointer.");
}

static void error_print_typecheck_expected_array(error_t error)
{
  error_print_helper_snippet(error.expected_array.loc, "Expected an array.");
}

static void error_print_typecheck_expected_reference(error_t error)
{
  error_print_helper_snippet(error.expected_reference.loc, "Expected a reference.");
}

static void error_print_typecheck_expected_vector(error_t error)
{
  error_print_helper_snippet(error.expected_vector.loc, "Expected a vector.");
}

static void error_print_typecheck_expected_integer_or_float(error_t error)
{
  error_print_helper_snippet(error.expected_integer_or_float.loc, "Expected integer or float.");
}

static void error_print_typecheck_incompatible_return_type(error_t error)
{
  error_print_helper_snippet(error.incompatible_return_type.loc, "Incompatible return type.");
}

static void error_print_typecheck_too_many_function_parameters(error_t error)
{
  error_print_helper_snippet(error.too_many_function_parameters.loc, "Too many function parameters.");
}

static void error_print_typecheck_too_few_function_parameters(error_t error)
{
  error_print_helper_snippet(error.too_few_function_parameters.loc, "Too few function parameters.");
}

static void error_print_typecheck_no_member(error_t error)
{
  error_print_helper_snippet(error.no_member.loc, "No member with this name exists.");
}

static void error_print_typecheck_private_member(error_t error)
{
  error_print_helper_snippet(error.private_member.loc, "Cannot access private member.");
}

static void error_print_typecheck_illegal_conversion(error_t error)
{
  error_print_helper_snippet(error.illegal_conversion.loc, "Illegal type conversion.");
}

static void error_print_typecheck_integer_literal_too_large(error_t error)
{
  error_print_helper_snippet(error.integer_literal_too_large.loc, "Integer literal too large.");
}

static void error_print_typecheck_incompatible_vector_dimensions(error_t error)
{
  error_print_helper_snippet(error.incompatible_vector_dimensions.loc, "Incompatible vector dimensions.");
}

static void error_print_ctrlflow_break_outside_loop(error_t error)
{
  error_print_helper_snippet(error.break_outside_loop.loc, "Break statement not within a loop.");
}

static void error_print_ctrlflow_continue_outside_loop(error_t error)
{
  error_print_helper_snippet(error.continue_outside_loop.loc, "Continue statement not within a loop.");
}

static void error_print_ctrlflow_return_inside_defer(error_t error)
{
  error_print_helper_snippet(error.return_inside_defer.loc, "Return statement within a defer.");
}

void error_print(error_t error)
{
  switch (error.kind)
  {
  case ERROR_COMPILER_FILE_OPEN_FAILED:                error_print_compiler_file_open_failed               (error); break;
  case ERROR_LEXER_UNEXPECTED_CHARACTER:               error_print_lexer_unexpected_character              (error); break;
  case ERROR_LEXER_IDENTIFIER_TOO_LONG:                error_print_lexer_identifier_too_long               (error); break;
  case ERROR_LEXER_MISSING_SINGLE_QUOTE:               error_print_lexer_missing_single_quote              (error); break;
  case ERROR_LEXER_MISSING_DOUBLE_QUOTE:               error_print_lexer_missing_double_quote              (error); break;
  case ERROR_LEXER_EMPTY_CHARACTER:                    error_print_lexer_empty_character                   (error); break;
  case ERROR_LEXER_MISSING_HEX_DIGITS:                 error_print_lexer_missing_hex_digits                (error); break;
  case ERROR_LEXER_TOO_MANY_HEX_DIGITS:                error_print_lexer_too_many_hex_digits               (error); break;
  case ERROR_LEXER_UNKNOWN_ESCAPE_SEQUENCE:            error_print_lexer_unknown_escape_sequence           (error); break;
  case ERROR_LEXER_ILL_FORMED_INTEGER:                 error_print_lexer_ill_formed_integer                (error); break;
  case ERROR_LEXER_ILL_FORMED_FLOAT:                   error_print_lexer_ill_formed_float                  (error); break;
  case ERROR_LEXER_INVALID_INTEGER_SUFFIX:             error_print_lexer_invalid_integer_suffix            (error); break;
  case ERROR_PARSER_UNEXPECTED_TOKEN:                  error_print_parser_unexpected_token                 (error); break;
  case ERROR_PARSER_UNKNOWN_CALLING_CONVENTION:        error_print_parser_unknown_calling_convention       (error); break;
  case ERROR_PARSER_EXPECTED_CALLING_CONVENTION:       error_print_parser_expected_calling_convention      (error); break;
  case ERROR_PARSER_DEFAULT_PARAMETER_ORDER:           error_print_parser_default_parameter_order          (error); break;
  case ERROR_PARSER_MISSING_PAREN:                     error_print_parser_missing_paren                    (error); break;
  case ERROR_PARSER_MISSING_BRACKET:                   error_print_parser_missing_bracket                  (error); break;
  case ERROR_PARSER_MISSING_UNARY_ARGUMENT:            error_print_parser_missing_unary_argument           (error); break;
  case ERROR_PARSER_MISSING_BINARY_ARGUMENT:           error_print_parser_missing_binary_argument          (error); break;
  case ERROR_PARSER_MISSING_CALLEE:                    error_print_parser_missing_callee                   (error); break;
  case ERROR_NAMERES_SYMBOL_COLLISION:                 error_print_nameres_symbol_collision                (error); break;
  case ERROR_NAMERES_UNDEFINED_SYMBOL:                 error_print_nameres_undefined_symbol                (error); break;
  case ERROR_NAMERES_EXPECTED_EXPRESSION_SYMBOL:       error_print_nameres_expected_expression_symbol      (error); break;
  case ERROR_NAMERES_EXPECTED_TYPENAME:                error_print_nameres_expected_typename               (error); break;
  case ERROR_NAMERES_SHADOWED_SYMBOL:                  error_print_nameres_shadowed_symbol                 (error); break;
  case ERROR_TYPECHECK_EXPECTED_INTEGER:               error_print_typecheck_expected_integer              (error); break;
  case ERROR_TYPECHECK_EXPECTED_ARITHMETIC:            error_print_typecheck_expected_arithmetic           (error); break;
  case ERROR_TYPECHECK_EXPECTED_BOOL:                  error_print_typecheck_expected_bool                 (error); break;
  case ERROR_TYPECHECK_EXPECTED_MUTABLE:               error_print_typecheck_expected_mutable              (error); break;
  case ERROR_TYPECHECK_EXPECTED_OPTIONAL:              error_print_typecheck_expected_optional             (error); break;
  case ERROR_TYPECHECK_EXPECTED_POINTER:               error_print_typecheck_expected_pointer              (error); break;
  case ERROR_TYPECHECK_EXPECTED_ARRAY:                 error_print_typecheck_expected_array                (error); break;
  case ERROR_TYPECHECK_EXPECTED_REFERENCE:             error_print_typecheck_expected_reference            (error); break;
  case ERROR_TYPECHECK_EXPECTED_VECTOR:                error_print_typecheck_expected_vector               (error); break;
  case ERROR_TYPECHECK_EXPECTED_INTEGER_OR_FLOAT:      error_print_typecheck_expected_integer_or_float     (error); break;
  case ERROR_TYPECHECK_INCOMPATIBLE_RETURN_TYPE:       error_print_typecheck_incompatible_return_type      (error); break;
  case ERROR_TYPECHECK_TOO_MANY_FUNCTION_PARAMETERS:   error_print_typecheck_too_many_function_parameters  (error); break;
  case ERROR_TYPECHECK_TOO_FEW_FUNCTION_PARAMETERS:    error_print_typecheck_too_few_function_parameters   (error); break;
  case ERROR_TYPECHECK_NO_MEMBER:                      error_print_typecheck_no_member                     (error); break;
  case ERROR_TYPECHECK_PRIVATE_MEMBER:                 error_print_typecheck_private_member                (error); break;
  case ERROR_TYPECHECK_ILLEGAL_CONVERSION:             error_print_typecheck_illegal_conversion            (error); break;
  case ERROR_TYPECHECK_INTEGER_LITERAL_TOO_LARGE:      error_print_typecheck_integer_literal_too_large     (error); break;
  case ERROR_TYPECHECK_INCOMPATIBLE_VECTOR_DIMENSIONS: error_print_typecheck_incompatible_vector_dimensions(error); break;
  case ERROR_CTRLFLOW_BREAK_OUTSIDE_LOOP:              error_print_ctrlflow_break_outside_loop             (error); break;
  case ERROR_CTRLFLOW_CONTINUE_OUTSIDE_LOOP:           error_print_ctrlflow_continue_outside_loop          (error); break;
  case ERROR_CTRLFLOW_RETURN_INSIDE_DEFER:             error_print_ctrlflow_return_inside_defer            (error); break;
  default: UNREACHABLE();
  }
}

error_bag_t* error_bag_init(size_t capacity)
{
  error_bag_t* bag = (error_bag_t*)malloc(sizeof(error_bag_t));

  bag->capacity = capacity;
  bag->size = 0;
  bag->data = (error_t*)malloc(bag->capacity * sizeof(error_t));

  return bag;
}

void error_bag_free(error_bag_t* bag)
{
  free(bag->data);
  free(bag);
}

void error_bag_print(const error_bag_t* bag)
{
  for (size_t i = 0; i < bag->size; i++)
    error_print(bag->data[i]);
}

bool error_bag_put(error_bag_t* bag, error_t error)
{
  if (bag->size >= bag->capacity)
    return false;

  bag->data[bag->size++] = error;

  return true;
}

bool error_bag_get(error_bag_t* restrict bag, error_t* restrict dst)
{
  if (bag->size == 0)
    return false;

  memcpy(dst, bag->data + --bag->size, sizeof(error_t));

  return true;
}

bool error_bag_empty(const error_bag_t* bag)
{
  return bag->size == 0;
}

bool error_bag_full(const error_bag_t* bag)
{
  return bag->size == bag->capacity;
}

void error_bag_put_compiler_file_open_failed(error_bag_t* bag, const char* path)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_COMPILER_FILE_OPEN_FAILED,
    .file_open_failed = {
      .path = path
    }
  });
}

void error_bag_put_lexer_unexpected_character(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_LEXER_UNEXPECTED_CHARACTER,
    .unexpected_character = {
      .loc = loc
    }
  });
}

void error_bag_put_lexer_identifier_too_long(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_LEXER_IDENTIFIER_TOO_LONG,
    .identifier_too_long = {
      .loc = loc
    }
  });
}

void error_bag_put_lexer_missing_single_quote(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_LEXER_MISSING_SINGLE_QUOTE,
    .missing_single_quote = {
      .loc = loc
    }
  });
}

void error_bag_put_lexer_missing_double_quote(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_LEXER_MISSING_DOUBLE_QUOTE,
    .missing_double_quote = {
      .loc = loc
    }
  });
}

void error_bag_put_lexer_empty_character(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_LEXER_EMPTY_CHARACTER,
    .empty_character = {
      .loc = loc
    }
  });
}

void error_bag_put_lexer_missing_hex_digits(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_LEXER_MISSING_HEX_DIGITS,
    .missing_hex_digits = {
      .loc = loc
    }
  });
}

void error_bag_put_lexer_too_many_hex_digits(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_LEXER_TOO_MANY_HEX_DIGITS,
    .too_many_hex_digits = {
      .loc = loc
    }
  });
}

void error_bag_put_lexer_unknown_escape_sequence(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_LEXER_UNKNOWN_ESCAPE_SEQUENCE,
    .unknown_escape_sequence = {
      .loc = loc
    }
  });
}

void error_bag_put_lexer_ill_formed_integer(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_LEXER_ILL_FORMED_INTEGER,
    .ill_formed_integer = {
      .loc = loc
    }
  });
}

void error_bag_put_lexer_ill_formed_float(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_LEXER_ILL_FORMED_FLOAT,
    .ill_formed_float = {
      .loc = loc
    }
  });
}

void error_bag_put_lexer_invalid_integer_suffix(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_LEXER_INVALID_INTEGER_SUFFIX,
    .invalid_integer_suffix = {
      .loc = loc
    }
  });
}

void error_bag_put_parser_unexpected_token(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_PARSER_UNEXPECTED_TOKEN,
    .unexpected_token = {
      .loc = loc
    }
  });
}

void error_bag_put_parser_unknown_calling_convention(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_PARSER_UNKNOWN_CALLING_CONVENTION,
    .unknown_calling_convention = {
      .loc = loc
    }
  });
}

void error_bag_put_parser_expected_calling_convention(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_PARSER_EXPECTED_CALLING_CONVENTION,
    .expected_calling_convention = {
      .loc = loc
    }
  });
}

void error_bag_put_parser_default_parameter_order(error_bag_t* bag, location_t default_param_loc, location_t param_loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_PARSER_DEFAULT_PARAMETER_ORDER,
    .default_parameter_order = {
      .default_param_loc = default_param_loc,
      .param_loc = param_loc
    }
  });
}

void error_bag_put_parser_missing_paren(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_PARSER_MISSING_PAREN,
    .missing_paren = {
      .loc = loc
    }
  });
}

void error_bag_put_parser_missing_bracket(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_PARSER_MISSING_BRACKET,
    .missing_bracket = {
      .loc = loc
    }
  });
}

void error_bag_put_parser_missing_unary_argument(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_PARSER_MISSING_UNARY_ARGUMENT,
    .missing_unary_argument = {
      .loc = loc
    }
  });
}

void error_bag_put_parser_missing_callee(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_PARSER_MISSING_CALLEE,
    .missing_callee = {
      .loc = loc
    }
  });
}

void error_bag_put_parser_missing_binary_argument(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_PARSER_MISSING_BINARY_ARGUMENT,
    .missing_binary_argument = {
      .loc = loc
    }
  });
}

void error_bag_put_nameres_symbol_collision(error_bag_t* bag, location_t symbol_loc, location_t new_symbol_loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_NAMERES_SYMBOL_COLLISION,
    .symbol_collision = {
      .symbol_loc = symbol_loc,
      .new_symbol_loc = new_symbol_loc
    }
  });
}

void error_bag_put_nameres_undefined_symbol(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_NAMERES_UNDEFINED_SYMBOL,
    .undefined_symbol = {
      .loc = loc
    }
  });
}

void error_bag_put_nameres_expected_expression_symbol(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_NAMERES_EXPECTED_EXPRESSION_SYMBOL,
    .expected_expression_symbol = {
      .loc = loc
    }
  });
}

void error_bag_put_nameres_expected_typename(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_NAMERES_EXPECTED_TYPENAME,
    .expected_typename = {
      .loc = loc
    }
  });
}

void error_bag_put_nameres_shadowed_symbol(error_bag_t* bag, location_t shadowed_symbol_loc, location_t inner_symbol_loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_NAMERES_SHADOWED_SYMBOL,
    .shadowed_symbol = {
      .shadowed_symbol_loc = shadowed_symbol_loc,
      .inner_symbol_loc = inner_symbol_loc
    }
  });
}

void error_bag_put_nameres_no_member(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_NAMERES_NO_MEMBER,
    .no_member = {
      .loc = loc
    }
  });
}

void error_bag_put_nameres_private_member(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_NAMERES_PRIVATE_MEMBER,
    .private_member = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_expected_integer(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_EXPECTED_INTEGER,
    .expected_integer = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_expected_arithmetic(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_EXPECTED_ARITHMETIC,
    .expected_arithmetic = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_expected_bool(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_EXPECTED_BOOL,
    .expected_bool = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_expected_mutable(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_EXPECTED_MUTABLE,
    .expected_mutable = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_expected_optional(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_EXPECTED_OPTIONAL,
    .expected_optional = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_expected_pointer(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_EXPECTED_POINTER,
    .expected_pointer = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_expected_array(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_EXPECTED_ARRAY,
    .expected_array = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_expected_reference(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_EXPECTED_REFERENCE,
    .expected_reference = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_expected_vector(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_EXPECTED_VECTOR,
    .expected_vector = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_incompatible_return_type(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_INCOMPATIBLE_RETURN_TYPE,
    .incompatible_return_type = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_too_many_function_parameters(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_TOO_MANY_FUNCTION_PARAMETERS,
    .too_many_function_parameters = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_too_few_function_parameters(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_TOO_FEW_FUNCTION_PARAMETERS,
    .too_few_function_parameters = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_no_member(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_NO_MEMBER,
    .no_member = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_private_member(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_PRIVATE_MEMBER,
    .private_member = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_illegal_conversion(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_ILLEGAL_CONVERSION,
    .illegal_conversion = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_integer_literal_too_large(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_INTEGER_LITERAL_TOO_LARGE,
    .integer_literal_too_large = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_incompatible_vector_dimensions(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_INCOMPATIBLE_VECTOR_DIMENSIONS,
    .incompatible_vector_dimensions = {
      .loc = loc
    }
  });
}

void error_bag_put_typecheck_expected_integer_or_float(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_TYPECHECK_EXPECTED_INTEGER_OR_FLOAT,
    .expected_integer_or_float = {
      .loc = loc
    }
  });
}

void error_bag_put_ctrlflow_break_outside_loop(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_CTRLFLOW_BREAK_OUTSIDE_LOOP,
    .break_outside_loop = {
      .loc = loc
    }
  });
}

void error_bag_put_ctrlflow_continue_outside_loop(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_CTRLFLOW_CONTINUE_OUTSIDE_LOOP,
    .continue_outside_loop = {
      .loc = loc
    }
  });
}

void error_bag_put_ctrlflow_return_inside_defer(error_bag_t* bag, location_t loc)
{
  error_bag_put(bag, (error_t){
    .kind = ERROR_CTRLFLOW_RETURN_INSIDE_DEFER,
    .return_inside_defer = {
      .loc = loc
    }
  });
}

