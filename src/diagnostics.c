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

void report_error_failed_to_open_file(char* path)
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
