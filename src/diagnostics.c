#include "diagnostics.h"

#include <stdlib.h>
#include <stdarg.h>

#include "util.h"
#include "log.h"
#include "crumb.h"
#include "memtrace.h"

void report_error_unexpected_character(location_t* loc)
{
  crumb_error(loc, "Unexpected character.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_identifier_too_long(location_t* loc)
{
  crumb_error(loc, "Identifier cannot exceed 255 characters.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_missing_terminating_character(location_t* loc, char ch)
{
  crumb_error(loc, "Missing terminating %c character.", ch);
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_empty_character_literal(location_t* loc)
{
  crumb_error(loc, "Empty character literal.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_escape_no_hex_digits(location_t* loc)
{
  crumb_error(loc, "\\x used with no following hex digits.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_escape_too_many_hex_digits(location_t* loc)
{
  crumb_error(loc, "\\x followed by more than 2 hex digits.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_unknown_escape_sequence(location_t* loc)
{
  crumb_error(loc, "Unknown escape sequence.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_ill_formed_integer_literal(location_t* loc)
{
  crumb_error(loc, "Ill-formed integer literal.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_ill_formed_float_literal(location_t* loc)
{
  crumb_error(loc, "Ill-formed float literal.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_unexpected_token(location_t* loc)
{
  crumb_error(loc, "Unexpected token.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_failed_to_open_file(const char* path)
{
  log_fatal("file", "Cannot open file: %s", path);
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_empty_generic_parameter_list(location_t* loc)
{
  crumb_error(loc, "Empty generic parameter list.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_missing_default_parameter(location_t* loc)
{
  crumb_error(loc, "Missing default parameter.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_parameter_redefinition(location_t* loc)
{
  crumb_error(loc, "Redefinition of parameter.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_variable_redeclaration(location_t* loc)
{
  crumb_error(loc, "Redeclaration of variable.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_enumerator_redeclaration(location_t* loc)
{
  crumb_error(loc, "Redeclaration of enumerator.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_symbol_redeclaration(location_t* loc)
{
  crumb_error(loc, "Redeclaration of symbol.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_undefined_symbol(location_t* loc)
{
  crumb_error(loc, "Undefined symbol.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_symbol_is_not_an_expression(location_t* loc)
{
  crumb_error(loc, "Symbol is not an expression.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_undefined_typename(location_t* loc)
{
  crumb_error(loc, "Undefined typename.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_symbol_is_not_a_typename(location_t* loc)
{
  crumb_error(loc, "Symbol is not a typename.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_warning_shadowed_variable(location_t* loc)
{
  crumb_warn(loc, "Variable shadows another.");
}

void report_warning_shadowed_symbol(location_t* loc)
{
  crumb_warn(loc, "Symbol shadows another.");
}

void report_error_expected_arithmetic_type(location_t* loc)
{
  crumb_error(loc, "Expected arithmetic type.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_bool_type(location_t* loc)
{
  crumb_error(loc, "Expected bool type.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_ptr_type(location_t* loc)
{
  crumb_error(loc, "Expected pointer type.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_nullable_type(location_t* loc)
{
  crumb_error(loc, "Expected nullable type.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_mixed_signedness(location_t* loc)
{
  crumb_error(loc, "Mixed signedness.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_integer_type(location_t* loc)
{
  crumb_error(loc, "Expected integer type.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_incompatible_return_type(location_t* loc)
{
  crumb_error(loc, "Incompatible return type.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_too_many_arguments(location_t* loc)
{
  crumb_error(loc, "Too many arguments.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_too_few_arguments(location_t* loc)
{
  crumb_error(loc, "Too few arguments.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_callable(location_t* loc)
{
  crumb_error(loc, "Expression does not evaluate to a callable.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_incompatible_param_type(location_t* loc)
{
  crumb_error(loc, "Incompatible parameter type.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_subscriptable(location_t* loc)
{
  crumb_error(loc, "Expression is not subscriptable.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_owner(location_t* loc)
{
  crumb_error(loc, "Expression does not have members.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_member(location_t* loc)
{
  crumb_error(loc, "Expected member name.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_no_member_with_name(location_t* loc)
{
  crumb_error(loc, "There is no member with this name.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_expected_ptr_to_owner(location_t* loc)
{
  crumb_error(loc, "Expected pointer to structure or union.");
  debugbreak();
  exit(EXIT_FAILURE);
}

void report_error_type_mismatch(location_t* loc, type_t* expected, type_t* actual)
{
  crumb_error(loc, "Type mismatch.");
  debugbreak();
  exit(EXIT_FAILURE);
}
