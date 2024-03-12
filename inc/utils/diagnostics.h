/**
 * \file
 * 
 * \brief Diagnostics interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_DIAGNOSTICS_H
#define TAU_DIAGNOSTICS_H

#include "ast/ast.h"
#include "stages/analysis/types/typedesc/typedesc.h"
#include "stages/lexer/location.h"
#include "utils/common.h"

/**
 * \brief Reports an error for encountering an unexpected character.
 * 
 * \param[in] loc Pointer to the location of the unexpected character.
 */
noreturn void report_error_unexpected_character(location_t* loc);

/**
 * \brief Reports an error for encountering an identifier that is too long.
 * 
 * \param[in] loc Pointer to the location of the identifier.
 */
noreturn void report_error_identifier_too_long(location_t* loc);

/**
 * \brief Reports an error for a missing terminating single quote of a character
 * literal.
 * 
 * \param[in] loc Pointer to the location of the character literal.
 */
noreturn void report_error_missing_terminating_single_quote(location_t* loc);

/**
 * \brief Reports an error for a missing terminating double quotes of a string
 * literal.
 * 
 * \param[in] loc Pointer to the location of the string literal.
 */
noreturn void report_error_missing_terminating_double_quotes(location_t* loc);

/**
 * \brief Reports an error for an empty character literal.
 * 
 * \param[in] loc Pointer to the location of the character literal.
 */
noreturn void report_error_empty_character_literal(location_t* loc);

/**
 * \brief Reports an error for missing hex digits in an escape sequence.
 * 
 * \param[in] loc Pointer to the location of the escape sequence.
 */
noreturn void report_error_missing_hex_digits_in_escape_sequence(location_t* loc);

/**
 * \brief Reports an error for too many hex digits in an escape sequence.
 * 
 * \param[in] loc Pointer to the location of the escape sequence.
 */
noreturn void report_error_too_many_hex_digits_in_escape_sequence(location_t* loc);

/**
 * \brief Reports an error for an unknown escape sequence.
 * 
 * \param[in] loc Pointer to the location of the escape sequence.
 */
noreturn void report_error_unknown_escape_sequence(location_t* loc);

/**
 * \brief Reports an error for an ill-formed integer literal.
 * 
 * \param[in] loc Pointer to the location of the integer literal.
 */
noreturn void report_error_ill_formed_integer_literal(location_t* loc);

/**
 * \brief Reports an error for an invalid integer suffix.
 * 
 * \param[in] loc Pointer to the location of the integer literal.
 */
noreturn void report_error_invalid_integer_suffix(location_t* loc);

/**
 * \brief Reports an error for an ill-formed floating point literal.
 * 
 * \param[in] loc Pointer to the location of the floating-point literal.
 */
noreturn void report_error_ill_formed_floating_point_literal(location_t* loc);

/**
 * \brief Reports an error for encountering an unexpected token.
 * 
 * \param[in] loc Pointer to the location of the token.
 */
noreturn void report_error_unexpected_token(location_t* loc);

/**
 * \brief Reports an error for failing to open a file.
 * 
 * \param[in] path The path of the file that failed to open.
 */
noreturn void report_error_failed_to_open_file(const char* path);

/**
 * \brief Reports an error for declaring a non-default parameter after a default
 * parameter.
 * 
 * \param[in] param_node Pointer to the non-default AST parameter declaration node.
 * \param[in] first_default_node Pointer to the first default AST parameter declaration node.
 */
noreturn void report_error_non_default_after_default_parameter(ast_decl_param_t* param_node, ast_decl_param_t* first_default_node);

/**
 * \brief Reports an error for redefining a parameter.
 * 
 * \param[in] param_node Pointer to the redefined AST parameter declaration node.
 * \param[in] redef_node Pointer to the redefining AST parameter declaration node.
 */
noreturn void report_error_parameter_redefinition(ast_decl_param_t* param_node, ast_decl_param_t* redef_node);

/**
 * \brief Reports an error for redefining a variable.
 * 
 * \param[in] var_node Pointer to the redefined AST variable declaration node.
 * \param[in] redef_node Pointer to the redefining AST variable declaration node.
 */
noreturn void report_error_variable_redefinition(ast_decl_var_t* var_node, ast_decl_var_t* redef_node);

/**
 * \brief Reports an error for redefining a struct.
 * 
 * \param[in] struct_node Pointer to the redefined AST struct declaration node.
 * \param[in] redef_node Pointer to the redefining AST declaration node.
 */
noreturn void report_error_struct_redefinition(ast_decl_struct_t* struct_node, ast_decl_t* redef_node);

/**
 * \brief Reports an error for redefining a union.
 * 
 * \param[in] union_node Pointer to the redefined AST union declaration node.
 * \param[in] redef_node Pointer to the redefining AST declaration node.
 */
noreturn void report_error_union_redefinition(ast_decl_union_t* union_node, ast_decl_t* redef_node);

/**
 * \brief Reports an error for redefining an enum.
 * 
 * \param[in] enum_node Pointer to the redefined AST enum declaration node.
 * \param[in] redef_node Pointer to the redefining AST declaration node.
 */
noreturn void report_error_enum_redefinition(ast_decl_enum_t* enum_node, ast_decl_t* redef_node);

/**
 * \brief Reports an error for redefining a type.
 * 
 * \param[in] node Pointer to the redefined AST declaration node.
 * \param[in] redef_node Pointer to the redefining AST declaration node.
 */
noreturn void report_error_type_redefinition(ast_decl_t* node, ast_decl_t* redef_node);

/**
 * \brief Reports an error for redefining an enum constant.
 * 
 * \param[in] enum_node Pointer to the redefined AST enum constant node.
 * \param[in] redef_node Pointer to the redefining AST enum constant node.
 */
noreturn void report_error_enum_constant_redefinition(ast_decl_enum_constant_t* enum_node, ast_decl_enum_constant_t* redef_node);

/**
 * \brief Reports an error for redeclaring a symbol.
 * 
 * \param[in] loc Pointer to the location of the redeclaration.
 */
noreturn void report_error_symbol_redeclaration(location_t* loc);

/**
 * \brief Reports an error for an undefined symbol.
 * 
 * \param[in] loc Pointer to the location of the symbol.
 */
noreturn void report_error_undefined_symbol(location_t* loc);

/**
 * \brief Reports an error for a symbol that is not an expression.
 * 
 * \param[in] loc Pointer to the location of the symbol.
 */
noreturn void report_error_symbol_is_not_an_expression(location_t* loc);

/**
 * \brief Reports an error for an undefined typename.
 * 
 * \param[in] loc Pointer to the location of the typename.
 */
noreturn void report_error_undefined_typename(location_t* loc);

/**
 * \brief Reports an error for a symbol that is not a typename.
 * 
 * \param[in] loc Pointer to the location of the symbol.
 */
noreturn void report_error_symbol_is_not_a_typename(location_t* loc);

/**
 * \brief Reports a warning for a shadowed variable.
 * 
 * \param[in] var_node Pointer to the shadowed AST variable declaration node.
 * \param[in] shadowing_node Pointer to the shadowing AST variable declaration node.
 */
void report_warning_shadowed_variable(ast_decl_var_t* var_node, ast_decl_var_t* shadowing_node);

/**
 * \brief Reports an error for an expected reference type.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_reference_type(location_t* loc);

/**
 * \brief Reports an error for an expected arithmetic type.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_arithmetic_type(location_t* loc);

/**
 * \brief Reports an error for an expected boolean type.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_bool_type(location_t* loc);

/**
 * \brief Reports an error for an expected pointer type.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_pointer_type(location_t* loc);

/**
 * \brief Reports an error for an expected optional type.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_optional_type(location_t* loc);

/**
 * \brief Reports an error for an expected mutable type.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_mutable_type(location_t* loc);

/**
 * \brief Reports an error for an expected constant type.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_constant_type(location_t* loc);

/**
 * \brief Reports a warning for mixed signedness.
 * 
 * \param[in] loc Pointer to the location where the warning occurred.
 */
void report_warning_mixed_signedness(location_t* loc);

/**
 * \brief Reports an error for an expected integer type.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_integer_type(location_t* loc);

/**
 * \brief Reports an error for an incompatible return type.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_incompatible_return_type(location_t* loc);

/**
 * \brief Reports an error for too many arguments.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_too_many_arguments(location_t* loc);

/**
 * \brief Reports an error for too few arguments.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_too_few_arguments(location_t* loc);

/**
 * \brief Reports an error for an expected callable.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_callable(location_t* loc);

/**
 * \brief Reports an error for an incompatible parameter type.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_incompatible_param_type(location_t* loc);

/**
 * \brief Reports an error for an expected subscriptable type.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_subscriptable(location_t* loc);

/**
 * \brief Reports an error for an expected owner.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_owner(location_t* loc);

/**
 * \brief Reports an error for an expected member.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_member(location_t* loc);

/**
 * \brief Reports an error for no member with the given name.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_no_member_with_name(location_t* loc);

/**
 * \brief Reports an error for accessing a private member.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_private_member(location_t* loc);

/**
 * \brief Reports an error for expecting a pointer to owner type.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_ptr_to_owner(location_t* loc);

/**
 * \brief Reports an error for a type mismatch.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 * \param[in] expected Pointer to the expected type descriptor.
 * \param[in] actual Pointer to the actual type descriptor.
 */
noreturn void report_error_type_mismatch(location_t* loc, typedesc_t* expected, typedesc_t* actual);

/**
 * \brief Reports an error for a missing closing parenthesis.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_missing_closing_parenthesis(location_t* loc);

/**
 * \brief Reports an error for a missing closing bracket.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_missing_closing_bracket(location_t* loc);

/**
 * \brief Reports an error for a missing unary argument.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_missing_unary_argument(location_t* loc);

/**
 * \brief Reports an error for a missing binary argument.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_missing_binary_argument(location_t* loc);

/**
 * \brief Reports an error for a missing callee.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_missing_callee(location_t* loc);

/**
 * \brief Reports an error for an expected module.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_expected_module(location_t* loc);

/**
 * \brief Reports an error for an unknown calling convention.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_unknown_callconv(location_t* loc);

/**
 * \brief Reports an error for encountering a break statement outside of a loop.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_break_outside_loop(location_t* loc);

/**
 * \brief Reports an error for encountering a continue statement outside of a loop.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_continue_outside_loop(location_t* loc);

/**
 * \brief Reports an error for encountering a return statement outside of a function.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_return_outside_function(location_t* loc);

/**
 * \brief Reports an error for encountering a return statement inside of a defer block.
 * 
 * \param[in] loc Pointer to the location where the error occurred.
 */
noreturn void report_error_return_inside_defer(location_t* loc);

#endif
