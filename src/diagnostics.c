#include "diagnostics.h"

#include <stdlib.h>
#include <stdarg.h>

#include "util.h"
#include "log.h"
#include "crumb.h"

#include "typedesc.h"

#include "memtrace.h"

void report_error_unexpected_character(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Unexpected character.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_identifier_too_long(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Identifier cannot exceed 255 characters.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_missing_terminating_single_quote(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing terminating single quote (').", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_missing_terminating_double_quotes(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing terminating double quotes (\").", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_empty_character_literal(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Empty character literal.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_missing_hex_digits_in_escape_sequence(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Hexadecimal escape sequence without hex digits.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_too_many_hex_digits_in_escape_sequence(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Too many hex digits in escape sequence.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_unknown_escape_sequence(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Unknown escape sequence.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_ill_formed_integer_literal(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Ill-formed integer literal.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_invalid_integer_suffix(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Invalid integer suffix.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_ill_formed_floating_point_literal(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Ill-formed float literal.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_unexpected_token(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Unexpected token.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_failed_to_open_file(const char* path)
{
  log_fatal("file", "Cannot open file: %s", path);
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_missing_default_parameter(location_t* param_loc, location_t* first_param_loc)
{
  crumb_item_t items[] = {
    crumb_snippet(param_loc, "Parameter missing default value.", NULL, "", NULL),
    crumb_message("A default parameter must only be followed by default parameters.", NULL),
    crumb_snippet(first_param_loc, "", NULL, "First default parameter appears here.", NULL)
  };
  crumb_error(countof(items), items[0], items[1], items[2]);
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_parameter_redefinition(location_t* param_loc, location_t* redefined_loc)
{
  crumb_item_t items[] = {
    crumb_snippet(param_loc, "Parameter redefinition.", NULL, "A parameter with this name already exists.", NULL),
    crumb_snippet(redefined_loc, "", NULL, "First parameter with this name appears here.", NULL)
  };
  crumb_error(countof(items), items[0], items[1]);
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_variable_redeclaration(location_t* var_loc, location_t* redeclared_loc)
{
  crumb_item_t items[] = {
    crumb_snippet(var_loc, "Variable redeclaration.", NULL, "A variable with this name already exists.", NULL),
    crumb_snippet(redeclared_loc, "", NULL, "First variable with this name appears here.", NULL)
  };
  crumb_error(countof(items), items[0], items[1]);
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_enumerator_redeclaration(location_t* enum_loc, location_t* redeclared_loc)
{
  crumb_item_t items[] = {
    crumb_snippet(enum_loc, "Enumerator redeclaration.", NULL, "An enumerator with this name already exists.", NULL),
    crumb_snippet(redeclared_loc, "", NULL, "First enumerator with this name appears here.", NULL)
  };
  crumb_error(countof(items), items[0], items[1]);
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_symbol_redeclaration(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Redeclaration of symbol.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_undefined_symbol(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Undefined symbol.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_symbol_is_not_an_expression(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Symbol is not an expression.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_undefined_typename(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Undefined typename.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_symbol_is_not_a_typename(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Symbol is not a typename.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_warning_shadowed_variable(location_t* loc)
{
  crumb_warn(1, crumb_snippet(loc, "Variable shadows another.", NULL, "", NULL));
  debugbreak();
}

void report_warning_shadowed_symbol(location_t* loc)
{
  crumb_warn(1, crumb_snippet(loc, "Symbol shadows another.", NULL, "", NULL));
  debugbreak();
}

void report_error_expected_reference_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected reference type.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_generator_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected generator type.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_arithmetic_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected arithmetic type.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_bool_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected bool type.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_ptr_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected pointer type.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_optional_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected nullable type.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_mutable_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected mutable type.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_constant_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected constant type.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_warning_mixed_signedness(location_t* loc)
{
  crumb_warn(1, crumb_snippet(loc, "Mixed signedness.", NULL, "", NULL));
  debugbreak();
}

void report_error_expected_integer_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected integer type.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_incompatible_return_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Incompatible return type.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_too_many_arguments(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Too many arguments.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_too_few_arguments(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Too few arguments.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_callable(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expression does not evaluate to a callable.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_incompatible_param_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Incompatible parameter type.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_subscriptable(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expression is not subscriptable.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_owner(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expression does not have members.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_member(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected member name.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_no_member_with_name(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "There is no member with this name.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_ptr_to_owner(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected pointer to structure or union.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_type_mismatch(location_t* loc, typedesc_t* expected, typedesc_t* actual)
{
  unused(expected);
  unused(actual);

  crumb_error(1, crumb_snippet(loc, "Type mismatch.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_missing_closing_parenthesis(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing closing parenthesis.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_missing_closing_bracket(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing closing bracket.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_missing_unary_argument(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing argument for unary operation.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_missing_binary_argument(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing argument for binary operation.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_missing_callee(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing callee.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_module(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected module.", NULL, "", NULL));
  debugbreak();
  exit(EXIT_FAILURE);
}
