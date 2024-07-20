/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/diagnostics.h"

#include <stdlib.h>
#include <stdarg.h>

#include "utils/common.h"
#include "utils/crumb.h"
#include "utils/io/log.h"

void report_error_unexpected_character(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Unexpected character.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_identifier_too_long(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Identifier cannot exceed 255 characters.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_terminating_single_quote(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing terminating single quote (').", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_terminating_double_quotes(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing terminating double quotes (\").", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_empty_character_literal(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Empty character literal.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_hex_digits_in_escape_sequence(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Hexadecimal escape sequence without hex digits.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_too_many_hex_digits_in_escape_sequence(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Too many hex digits in escape sequence.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_unknown_escape_sequence(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Unknown escape sequence.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_ill_formed_integer_literal(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Ill-formed integer literal.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_invalid_integer_suffix(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Invalid integer suffix.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_ill_formed_floating_point_literal(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Ill-formed float literal.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_unexpected_token(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Unexpected token.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_failed_to_open_file(const char* path)
{
  log_fatal("file", "Cannot open file: %s", path);
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_non_default_after_default_parameter(ast_decl_param_t* param_node, ast_decl_param_t* first_default_node)
{
  location_t param_node_id_loc = token_location(param_node->id->tok);
  location_t first_default_node_id_loc = token_location(first_default_node->id->tok);

  crumb_item_t items[] = {
    crumb_snippet(&param_node_id_loc, "A non-default parameter cannot appear after a default parameter.", NULL, "Parameter without a default value.", NULL),
    crumb_snippet(&first_default_node_id_loc, "", NULL, "First default parameter appears here.", NULL)
  };
  crumb_error(COUNTOF(items), items[0], items[1]);
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_parameter_redefinition(ast_decl_param_t* param_node, ast_decl_param_t* redef_node)
{
  location_t redef_node_id_loc = token_location(redef_node->id->tok);
  location_t param_node_id_loc = token_location(param_node->id->tok);

  crumb_item_t items[] = {
    crumb_snippet(&redef_node_id_loc, "Two parameters cannot share the same name in a function declaration.", NULL, "A parameter with this name already exists.", NULL),
    crumb_snippet(&param_node_id_loc, "", NULL, "First parameter with this name appears here.", NULL)
  };
  crumb_error(COUNTOF(items), items[0], items[1]);
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_variable_redefinition(ast_decl_var_t* var_node, ast_decl_var_t* redef_node)
{
  location_t redef_node_id_loc = token_location(redef_node->id->tok);
  location_t var_node_id_loc = token_location(var_node->id->tok);

  crumb_item_t items[] = {
    crumb_snippet(&redef_node_id_loc, "Two variables cannot share the same name in a scope.", NULL, "A variable with this name already exists.", NULL),
    crumb_snippet(&var_node_id_loc, "", NULL, "First variable with this name appears here.", NULL)
  };
  crumb_error(COUNTOF(items), items[0], items[1]);
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_struct_redefinition(ast_decl_struct_t* struct_node, ast_decl_t* redef_node)
{
  location_t redef_node_id_loc = token_location(redef_node->id->tok);
  location_t struct_node_id_loc = token_location(struct_node->id->tok);

  crumb_item_t items[] = {
    crumb_snippet(&redef_node_id_loc, "Two declarations cannot share the same name in a module.", NULL, "A struct with this name already exists.", NULL),
    crumb_snippet(&struct_node_id_loc, "", NULL, "Struct with this name appears here.", NULL)
  };
  crumb_error(COUNTOF(items), items[0], items[1]);
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_union_redefinition(ast_decl_union_t* union_node, ast_decl_t* redef_node)
{
  location_t redef_node_id_loc = token_location(redef_node->id->tok);
  location_t union_node_id_loc = token_location(union_node->id->tok);

  crumb_item_t items[] = {
    crumb_snippet(&redef_node_id_loc, "Two declarations cannot share the same name in a module.", NULL, "A union with this name already exists.", NULL),
    crumb_snippet(&union_node_id_loc, "", NULL, "Union with this name appears here.", NULL)
  };
  crumb_error(COUNTOF(items), items[0], items[1]);
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_enum_redefinition(ast_decl_enum_t* enum_node, ast_decl_t* redef_node)
{
  location_t redef_node_id_loc = token_location(redef_node->id->tok);
  location_t enum_node_id_loc = token_location(enum_node->id->tok);

  crumb_item_t items[] = {
    crumb_snippet(&redef_node_id_loc, "Two declarations cannot share the same name in a module.", NULL, "An enum with this name already exists.", NULL),
    crumb_snippet(&enum_node_id_loc, "", NULL, "Enum with this name appears here.", NULL)
  };
  crumb_error(COUNTOF(items), items[0], items[1]);
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_type_redefinition(ast_decl_t* node, ast_decl_t* redef_node)
{
  switch (node->kind)
  {
  case AST_DECL_STRUCT: report_error_struct_redefinition((ast_decl_struct_t*)node, redef_node); break;
  case AST_DECL_UNION:  report_error_union_redefinition ((ast_decl_union_t* )node, redef_node); break;
  case AST_DECL_ENUM:   report_error_enum_redefinition  ((ast_decl_enum_t*  )node, redef_node); break;
  default: UNREACHABLE();
  }
}

void report_error_enum_constant_redefinition(ast_decl_enum_constant_t* enum_node, ast_decl_enum_constant_t* redef_node)
{
  location_t redef_node_id_loc = token_location(redef_node->id->tok);
  location_t enum_node_id_loc = token_location(enum_node->id->tok);

  crumb_item_t items[] = {
    crumb_snippet(&redef_node_id_loc, "Two enum constants cannot share the same name in an enum.", NULL, "An enum constant with this name already exists.", NULL),
    crumb_snippet(&enum_node_id_loc, "", NULL, "First enum constant with this name appears here.", NULL)
  };
  crumb_error(COUNTOF(items), items[0], items[1]);
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_symbol_redeclaration(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Redeclaration of symbol.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_undefined_symbol(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Undefined symbol.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_symbol_is_not_an_expression(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Symbol is not an expression.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_undefined_typename(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Undefined typename.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_symbol_is_not_a_typename(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Symbol is not a typename.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_warning_shadowed_variable(ast_decl_var_t* var_node, ast_decl_var_t* shadowing_node)
{
  location_t shadowing_node_id_loc = token_location(shadowing_node->id->tok);
  location_t var_node_id_loc = token_location(var_node->id->tok);

  crumb_item_t items[] = {
    crumb_snippet(&shadowing_node_id_loc, "A variable is shadowed by another one with the same name.", NULL, "This variable shadows another one from an outer scope.", NULL),
    crumb_snippet(&var_node_id_loc, "", NULL, "The shadowed variable is declared here.", NULL)
  };
  crumb_warn(COUNTOF(items), items[0], items[1]);
  DEBUGBREAK();
}

void report_error_expected_reference_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected reference type.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_arithmetic_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected arithmetic type.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_bool_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected bool type.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_pointer_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected pointer type.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_optional_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected nullable type.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_mutable_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected mutable type.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_constant_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected constant type.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_warning_mixed_signedness(location_t* loc)
{
  crumb_warn(1, crumb_snippet(loc, "Mixed signedness.", NULL, "", NULL));
  DEBUGBREAK();
}

void report_error_expected_integer_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected integer type.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_incompatible_return_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Incompatible return type.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_too_many_arguments(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Too many arguments.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_too_few_arguments(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Too few arguments.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_callable(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expression does not evaluate to a callable.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_incompatible_param_type(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Incompatible parameter type.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_subscriptable(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expression is not subscriptable.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_owner(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expression does not have members.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_member(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected member name.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_no_member_with_name(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "There is no member with this name.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_private_member(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Trying to access private member.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_ptr_to_owner(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected pointer to structure or union.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_type_mismatch(location_t* loc, typedesc_t* UNUSED(expected), typedesc_t* UNUSED(actual))
{
  crumb_error(1, crumb_snippet(loc, "Type mismatch.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_closing_parenthesis(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing closing parenthesis.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_closing_bracket(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing closing bracket.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_unary_argument(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing argument for unary operation.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_binary_argument(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing argument for binary operation.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_callee(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Missing callee.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_module(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Expected module.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_unknown_callconv(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Unknown calling convention.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_break_outside_loop(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Break statement is not within a loop.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_continue_outside_loop(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Continue statement is not within a loop.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_return_outside_function(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Return statement is not within a function.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_return_inside_defer(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Return statement is within a defer.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

NORETURN void report_error_literal_out_of_range(location_t* loc)
{
  crumb_error(1, crumb_snippet(loc, "Literal out of range.", NULL, "", NULL));
  DEBUGBREAK();
  exit(EXIT_FAILURE);
}
