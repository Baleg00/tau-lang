/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/diagnostics.h"

#include <stdarg.h>
#include <stdlib.h>

#include "utils/common.h"
#include "utils/crumb.h"
#include "utils/io/log.h"

void report_error_unexpected_character(location_t loc)
{
  crumb_error_t* error = crumb_error_init(1, "Unexpected character.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_identifier_too_long(location_t loc)
{
  crumb_error_t* error = crumb_error_init(2, "Identifier cannot exceed 255 characters.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_terminating_single_quote(location_t loc)
{
  crumb_error_t* error = crumb_error_init(3, "Missing terminating single quote `'`.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_snippet_label(snippet, "Character literal starts here.");
  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_terminating_double_quotes(location_t loc)
{
  crumb_error_t* error = crumb_error_init(4, "Missing terminating double quote `\"`.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_snippet_label(snippet, "String literal starts here.");
  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_empty_character_literal(location_t loc)
{
  crumb_error_t* error = crumb_error_init(5, "Empty character literal.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_hex_digits_in_escape_sequence(location_t loc)
{
  crumb_error_t* error = crumb_error_init(6, "Missing hex digits in escape sequence.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_too_many_hex_digits_in_escape_sequence(location_t loc)
{
  crumb_error_t* error = crumb_error_init(7, "Too many hex digits in escape sequence.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_unknown_escape_sequence(location_t loc)
{
  crumb_error_t* error = crumb_error_init(8, "Unknown escape sequence.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_ill_formed_integer_literal(location_t loc)
{
  crumb_error_t* error = crumb_error_init(9, "Ill-formed integer literal.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_invalid_integer_suffix(location_t loc)
{
  crumb_error_t* error = crumb_error_init(10, "Invalid integer suffix.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_snippet_label(snippet, "The suffix `%.*s` is not valid.", loc.len, loc.ptr);
  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_ill_formed_floating_point_literal(location_t loc)
{
  crumb_error_t* error = crumb_error_init(11, "Ill-formed float literal.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_unexpected_token(location_t loc)
{
  crumb_error_t* error = crumb_error_init(12, "Unexpected token.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

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

  crumb_error_t* error = crumb_error_init(13, "Non-default after default parameter.");
  crumb_snippet_t* snippet1 = crumb_snippet_init(param_node_id_loc);
  crumb_snippet_t* snippet2 = crumb_snippet_init(first_default_node_id_loc);

  crumb_snippet_label(snippet1, "Non-default parameter.");
  crumb_snippet_subsnippet(snippet1, snippet2);
  crumb_snippet_label(snippet2, "First default parameter appears here.");
  crumb_error_snippet(error, snippet1);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_parameter_redefinition(ast_decl_param_t* param_node, ast_decl_param_t* redef_node)
{
  location_t redef_node_id_loc = token_location(redef_node->id->tok);
  location_t param_node_id_loc = token_location(param_node->id->tok);

  crumb_error_t* error = crumb_error_init(13, "Parameter redefinition.");
  crumb_snippet_t* snippet1 = crumb_snippet_init(redef_node_id_loc);
  crumb_snippet_t* snippet2 = crumb_snippet_init(param_node_id_loc);

  crumb_snippet_label(snippet1, "A parameter named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_snippet_subsnippet(snippet1, snippet2);
  crumb_snippet_label(snippet2, "First parameter named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_error_snippet(error, snippet1);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_generic_parameter_redefinition(ast_decl_generic_param_t* param_node, ast_decl_generic_param_t* redef_node)
{
  location_t redef_node_id_loc = token_location(redef_node->id->tok);
  location_t param_node_id_loc = token_location(param_node->id->tok);

  crumb_error_t* error = crumb_error_init(13, "Generic parameter redefinition.");
  crumb_snippet_t* snippet1 = crumb_snippet_init(redef_node_id_loc);
  crumb_snippet_t* snippet2 = crumb_snippet_init(param_node_id_loc);

  crumb_snippet_label(snippet1, "A parameter named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_snippet_subsnippet(snippet1, snippet2);
  crumb_snippet_label(snippet2, "First parameter named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_error_snippet(error, snippet1);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_variable_redefinition(ast_decl_var_t* var_node, ast_decl_var_t* redef_node)
{
  location_t redef_node_id_loc = token_location(redef_node->id->tok);
  location_t var_node_id_loc = token_location(var_node->id->tok);

  crumb_error_t* error = crumb_error_init(13, "Variable redefinition.");
  crumb_snippet_t* snippet1 = crumb_snippet_init(redef_node_id_loc);
  crumb_snippet_t* snippet2 = crumb_snippet_init(var_node_id_loc);

  crumb_snippet_label(snippet1, "A variable named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_snippet_subsnippet(snippet1, snippet2);
  crumb_snippet_label(snippet2, "First variable named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_error_snippet(error, snippet1);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_struct_redefinition(ast_decl_struct_t* struct_node, ast_decl_t* redef_node)
{
  location_t redef_node_id_loc = token_location(redef_node->id->tok);
  location_t struct_node_id_loc = token_location(struct_node->id->tok);

  crumb_error_t* error = crumb_error_init(13, "Struct type redefinition.");
  crumb_snippet_t* snippet1 = crumb_snippet_init(redef_node_id_loc);
  crumb_snippet_t* snippet2 = crumb_snippet_init(struct_node_id_loc);

  crumb_snippet_label(snippet1, "A struct type named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_snippet_subsnippet(snippet1, snippet2);
  crumb_snippet_label(snippet2, "First struct type named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_error_snippet(error, snippet1);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_union_redefinition(ast_decl_union_t* union_node, ast_decl_t* redef_node)
{
  location_t redef_node_id_loc = token_location(redef_node->id->tok);
  location_t union_node_id_loc = token_location(union_node->id->tok);

  crumb_error_t* error = crumb_error_init(13, "Union type redefinition.");
  crumb_snippet_t* snippet1 = crumb_snippet_init(redef_node_id_loc);
  crumb_snippet_t* snippet2 = crumb_snippet_init(union_node_id_loc);

  crumb_snippet_label(snippet1, "A union type named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_snippet_subsnippet(snippet1, snippet2);
  crumb_snippet_label(snippet2, "First union type named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_error_snippet(error, snippet1);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_enum_redefinition(ast_decl_enum_t* enum_node, ast_decl_t* redef_node)
{
  location_t redef_node_id_loc = token_location(redef_node->id->tok);
  location_t enum_node_id_loc = token_location(enum_node->id->tok);

  crumb_error_t* error = crumb_error_init(13, "Enum type redefinition.");
  crumb_snippet_t* snippet1 = crumb_snippet_init(redef_node_id_loc);
  crumb_snippet_t* snippet2 = crumb_snippet_init(enum_node_id_loc);

  crumb_snippet_label(snippet1, "An enum type named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_snippet_subsnippet(snippet1, snippet2);
  crumb_snippet_label(snippet2, "First enum type named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_error_snippet(error, snippet1);
  crumb_error_print(error);
  crumb_error_free(error);

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

  crumb_error_t* error = crumb_error_init(13, "Enum constant redefinition.");
  crumb_snippet_t* snippet1 = crumb_snippet_init(redef_node_id_loc);
  crumb_snippet_t* snippet2 = crumb_snippet_init(enum_node_id_loc);

  crumb_snippet_label(snippet1, "An enum constant named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_snippet_subsnippet(snippet1, snippet2);
  crumb_snippet_label(snippet2, "First enum constant named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  crumb_error_snippet(error, snippet1);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_symbol_redeclaration(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Symbol redeclaration.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_undefined_symbol(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Undefined symbol.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_symbol_is_not_an_expression(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Symbol is not an expression.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_undefined_typename(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Undefined typename.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_symbol_is_not_a_typename(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Symbol is not a typename.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_warning_shadowed_variable(ast_decl_var_t* var_node, ast_decl_var_t* shadowing_node)
{
  location_t shadowing_node_id_loc = token_location(shadowing_node->id->tok);
  location_t var_node_id_loc = token_location(var_node->id->tok);

  crumb_error_t* error = crumb_error_init(13, "A variable is shadowed by another one with the same name.");
  crumb_snippet_t* snippet1 = crumb_snippet_init(shadowing_node_id_loc);
  crumb_snippet_t* snippet2 = crumb_snippet_init(var_node_id_loc);

  crumb_snippet_label(snippet1, "This variable shadows another one in an outer scope.");
  crumb_snippet_label(snippet2, "The shadowed variable is declared here.");
  crumb_snippet_subsnippet(snippet1, snippet2);
  crumb_error_snippet(error, snippet1);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
}

void report_error_expected_reference_type(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Expected reference type.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_arithmetic_type(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Expected arithmetic type.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_bool_type(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Expected arithmetic type.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_pointer_type(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Expected pointer type.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_optional_type(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Expected optional type.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_expected_mutable_type(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Expected mutable type.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_warning_mixed_signedness(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Mixed signedness.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
}

void report_error_expected_integer_type(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Expected integer type.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_incompatible_return_type(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Incompatible return type.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_too_many_arguments(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Too many arguments.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_too_few_arguments(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Too few arguments.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_no_member_with_name(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "There is no member with this name.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_private_member(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Trying to access private member.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_type_mismatch(location_t loc, typedesc_t* UNUSED(expected), typedesc_t* UNUSED(actual))
{
  crumb_error_t* error = crumb_error_init(13, "Type mismatch.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_closing_parenthesis(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Missing closing parenthesis.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_closing_bracket(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Missing closing bracket.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_unary_argument(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Missing argument for unary operation.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_binary_argument(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Missing argument for binary operation.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_missing_callee(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Missing callee.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_unknown_callconv(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Unknown calling convention.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_break_outside_loop(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Break statement is not within a loop.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_continue_outside_loop(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Continue statement is not within a loop.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_return_outside_function(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Return statement is not within a function.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void report_error_return_inside_defer(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Return statement is within a defer.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}

NORETURN void report_error_literal_out_of_range(location_t loc)
{
  crumb_error_t* error = crumb_error_init(13, "Literal out of range.");
  crumb_snippet_t* snippet = crumb_snippet_init(loc);

  crumb_error_snippet(error, snippet);
  crumb_error_print(error);
  crumb_error_free(error);

  DEBUGBREAK();
  exit(EXIT_FAILURE);
}
