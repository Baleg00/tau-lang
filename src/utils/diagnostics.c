/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/diagnostics.h"

#include "utils/common.h"
#include "utils/crumb.h"

void tau_report_error_unexpected_character(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(1, "Unexpected character.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_identifier_too_long(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(2, "Identifier cannot exceed 255 characters.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_missing_terminating_single_quote(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(3, "Missing terminating single quote `'`.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_snippet_label(snippet, "Character literal starts here.");
  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_missing_terminating_double_quotes(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(4, "Missing terminating double quote `\"`.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_snippet_label(snippet, "String literal starts here.");
  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_empty_character_literal(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(5, "Empty character literal.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_missing_hex_digits_in_escape_sequence(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(6, "Missing hex digits in escape sequence.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_too_many_hex_digits_in_escape_sequence(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(7, "Too many hex digits in escape sequence.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_unknown_escape_sequence(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(8, "Unknown escape sequence.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_ill_formed_integer_literal(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(9, "Ill-formed integer literal.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_invalid_integer_suffix(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(10, "Invalid integer suffix.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_snippet_label(snippet, "The suffix `%.*s` is not valid.", loc.len, loc.ptr);
  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_ill_formed_floating_point_literal(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(11, "Ill-formed float literal.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_unexpected_token(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(12, "Unexpected token.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_failed_to_open_file(const char* path)
{
  tau_log_fatal("file", "Cannot open file: %s", path);
  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_non_default_after_default_parameter(tau_ast_decl_param_t* param_node, tau_ast_decl_param_t* first_default_node)
{
  tau_location_t param_node_id_loc = tau_token_location(param_node->id->tok);
  tau_location_t first_default_node_id_loc = tau_token_location(first_default_node->id->tok);

  tau_crumb_error_t* error = tau_crumb_error_init(13, "Non-default after default parameter.");
  tau_crumb_snippet_t* snippet1 = tau_crumb_snippet_init(param_node_id_loc);
  tau_crumb_snippet_t* snippet2 = tau_crumb_snippet_init(first_default_node_id_loc);

  tau_crumb_snippet_label(snippet1, "Non-default parameter.");
  tau_crumb_snippet_subsnippet(snippet1, snippet2);
  tau_crumb_snippet_label(snippet2, "First default parameter appears here.");
  tau_crumb_error_snippet(error, snippet1);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_parameter_redefinition(tau_ast_decl_param_t* param_node, tau_ast_decl_param_t* redef_node)
{
  tau_location_t redef_node_id_loc = tau_token_location(redef_node->id->tok);
  tau_location_t param_node_id_loc = tau_token_location(param_node->id->tok);

  tau_crumb_error_t* error = tau_crumb_error_init(13, "Parameter redefinition.");
  tau_crumb_snippet_t* snippet1 = tau_crumb_snippet_init(redef_node_id_loc);
  tau_crumb_snippet_t* snippet2 = tau_crumb_snippet_init(param_node_id_loc);

  tau_crumb_snippet_label(snippet1, "A parameter named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_snippet_subsnippet(snippet1, snippet2);
  tau_crumb_snippet_label(snippet2, "First parameter named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_error_snippet(error, snippet1);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_generic_parameter_redefinition(tau_ast_decl_generic_param_t* param_node, tau_ast_decl_generic_param_t* redef_node)
{
  tau_location_t redef_node_id_loc = tau_token_location(redef_node->id->tok);
  tau_location_t param_node_id_loc = tau_token_location(param_node->id->tok);

  tau_crumb_error_t* error = tau_crumb_error_init(13, "Generic parameter redefinition.");
  tau_crumb_snippet_t* snippet1 = tau_crumb_snippet_init(redef_node_id_loc);
  tau_crumb_snippet_t* snippet2 = tau_crumb_snippet_init(param_node_id_loc);

  tau_crumb_snippet_label(snippet1, "A parameter named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_snippet_subsnippet(snippet1, snippet2);
  tau_crumb_snippet_label(snippet2, "First parameter named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_error_snippet(error, snippet1);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_variable_redefinition(tau_ast_decl_var_t* var_node, tau_ast_decl_var_t* redef_node)
{
  tau_location_t redef_node_id_loc = tau_token_location(redef_node->id->tok);
  tau_location_t var_node_id_loc = tau_token_location(var_node->id->tok);

  tau_crumb_error_t* error = tau_crumb_error_init(13, "Variable redefinition.");
  tau_crumb_snippet_t* snippet1 = tau_crumb_snippet_init(redef_node_id_loc);
  tau_crumb_snippet_t* snippet2 = tau_crumb_snippet_init(var_node_id_loc);

  tau_crumb_snippet_label(snippet1, "A variable named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_snippet_subsnippet(snippet1, snippet2);
  tau_crumb_snippet_label(snippet2, "First variable named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_error_snippet(error, snippet1);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_struct_redefinition(tau_ast_decl_struct_t* struct_node, tau_ast_decl_t* redef_node)
{
  tau_location_t redef_node_id_loc = tau_token_location(redef_node->id->tok);
  tau_location_t struct_node_id_loc = tau_token_location(struct_node->id->tok);

  tau_crumb_error_t* error = tau_crumb_error_init(13, "Struct type redefinition.");
  tau_crumb_snippet_t* snippet1 = tau_crumb_snippet_init(redef_node_id_loc);
  tau_crumb_snippet_t* snippet2 = tau_crumb_snippet_init(struct_node_id_loc);

  tau_crumb_snippet_label(snippet1, "A struct type named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_snippet_subsnippet(snippet1, snippet2);
  tau_crumb_snippet_label(snippet2, "First struct type named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_error_snippet(error, snippet1);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_union_redefinition(tau_ast_decl_union_t* union_node, tau_ast_decl_t* redef_node)
{
  tau_location_t redef_node_id_loc = tau_token_location(redef_node->id->tok);
  tau_location_t union_node_id_loc = tau_token_location(union_node->id->tok);

  tau_crumb_error_t* error = tau_crumb_error_init(13, "Union type redefinition.");
  tau_crumb_snippet_t* snippet1 = tau_crumb_snippet_init(redef_node_id_loc);
  tau_crumb_snippet_t* snippet2 = tau_crumb_snippet_init(union_node_id_loc);

  tau_crumb_snippet_label(snippet1, "A union type named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_snippet_subsnippet(snippet1, snippet2);
  tau_crumb_snippet_label(snippet2, "First union type named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_error_snippet(error, snippet1);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_enum_redefinition(tau_ast_decl_enum_t* enum_node, tau_ast_decl_t* redef_node)
{
  tau_location_t redef_node_id_loc = tau_token_location(redef_node->id->tok);
  tau_location_t enum_node_id_loc = tau_token_location(enum_node->id->tok);

  tau_crumb_error_t* error = tau_crumb_error_init(13, "Enum type redefinition.");
  tau_crumb_snippet_t* snippet1 = tau_crumb_snippet_init(redef_node_id_loc);
  tau_crumb_snippet_t* snippet2 = tau_crumb_snippet_init(enum_node_id_loc);

  tau_crumb_snippet_label(snippet1, "An enum type named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_snippet_subsnippet(snippet1, snippet2);
  tau_crumb_snippet_label(snippet2, "First enum type named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_error_snippet(error, snippet1);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_type_redefinition(tau_ast_decl_t* node, tau_ast_decl_t* redef_node)
{
  switch (node->kind)
  {
  case TAU_AST_DECL_STRUCT: tau_report_error_struct_redefinition((tau_ast_decl_struct_t*)node, redef_node); break;
  case TAU_AST_DECL_UNION:  tau_report_error_union_redefinition ((tau_ast_decl_union_t* )node, redef_node); break;
  case TAU_AST_DECL_ENUM:   tau_report_error_enum_redefinition  ((tau_ast_decl_enum_t*  )node, redef_node); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_report_error_enum_constant_redefinition(tau_ast_decl_enum_constant_t* enum_node, tau_ast_decl_enum_constant_t* redef_node)
{
  tau_location_t redef_node_id_loc = tau_token_location(redef_node->id->tok);
  tau_location_t enum_node_id_loc = tau_token_location(enum_node->id->tok);

  tau_crumb_error_t* error = tau_crumb_error_init(13, "Enum constant redefinition.");
  tau_crumb_snippet_t* snippet1 = tau_crumb_snippet_init(redef_node_id_loc);
  tau_crumb_snippet_t* snippet2 = tau_crumb_snippet_init(enum_node_id_loc);

  tau_crumb_snippet_label(snippet1, "An enum constant named `%.*s` already exists.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_snippet_subsnippet(snippet1, snippet2);
  tau_crumb_snippet_label(snippet2, "First enum constant named `%.*s` appears here.", (int)redef_node_id_loc.len, redef_node_id_loc.ptr);
  tau_crumb_error_snippet(error, snippet1);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_symbol_redeclaration(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Symbol redeclaration.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_undefined_symbol(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Undefined symbol.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_symbol_is_not_an_expression(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Symbol is not an expression.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_undefined_typename(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Undefined typename.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_symbol_is_not_a_typename(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Symbol is not a typename.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_warning_shadowed_variable(tau_ast_decl_var_t* var_node, tau_ast_decl_var_t* shadowing_node)
{
  tau_location_t shadowing_node_id_loc = tau_token_location(shadowing_node->id->tok);
  tau_location_t var_node_id_loc = tau_token_location(var_node->id->tok);

  tau_crumb_error_t* error = tau_crumb_error_init(13, "A variable is shadowed by another one with the same name.");
  tau_crumb_snippet_t* snippet1 = tau_crumb_snippet_init(shadowing_node_id_loc);
  tau_crumb_snippet_t* snippet2 = tau_crumb_snippet_init(var_node_id_loc);

  tau_crumb_snippet_label(snippet1, "This variable shadows another one in an outer scope.");
  tau_crumb_snippet_label(snippet2, "The shadowed variable is declared here.");
  tau_crumb_snippet_subsnippet(snippet1, snippet2);
  tau_crumb_error_snippet(error, snippet1);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
}

void tau_report_error_expected_reference_type(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Expected reference type.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_expected_arithmetic_type(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Expected arithmetic type.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_expected_bool_type(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Expected arithmetic type.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_expected_pointer_type(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Expected pointer type.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_expected_optional_type(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Expected optional type.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_expected_mutable_type(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Expected mutable type.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_warning_mixed_signedness(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Mixed signedness.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
}

void tau_report_error_expected_integer_type(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Expected integer type.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_incompatible_return_type(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Incompatible return type.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_too_many_arguments(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Too many arguments.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_too_few_arguments(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Too few arguments.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_no_member_with_name(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "There is no member with this name.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_private_member(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Trying to access private member.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_type_mismatch(tau_location_t loc, tau_typedesc_t* TAU_UNUSED(expected), tau_typedesc_t* TAU_UNUSED(actual))
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Type mismatch.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_missing_closing_parenthesis(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Missing closing parenthesis.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_missing_closing_bracket(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Missing closing bracket.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_missing_unary_argument(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Missing argument for unary operation.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_missing_binary_argument(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Missing argument for binary operation.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_missing_callee(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Missing callee.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_unknown_callconv(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Unknown calling convention.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_break_outside_loop(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Break statement is not within a loop.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_continue_outside_loop(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Continue statement is not within a loop.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_return_outside_function(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Return statement is not within a function.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

void tau_report_error_return_inside_defer(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Return statement is within a defer.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}

TAU_NORETURN void tau_report_error_literal_out_of_range(tau_location_t loc)
{
  tau_crumb_error_t* error = tau_crumb_error_init(13, "Literal out of range.");
  tau_crumb_snippet_t* snippet = tau_crumb_snippet_init(loc);

  tau_crumb_error_snippet(error, snippet);
  tau_crumb_error_print(error);
  tau_crumb_error_free(error);

  TAU_DEBUGBREAK();
  exit(EXIT_FAILURE);
}
