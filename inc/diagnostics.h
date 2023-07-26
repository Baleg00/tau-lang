#ifndef TAU_DIAGNOSTICS_H
#define TAU_DIAGNOSTICS_H

#include "location.h"
#include "typedefs.h"

void report_error_unexpected_character(location_t* loc);
void report_error_identifier_too_long(location_t* loc);
void report_error_missing_terminating_single_quote(location_t* loc);
void report_error_missing_terminating_double_quotes(location_t* loc);
void report_error_empty_character_literal(location_t* loc);
void report_error_missing_hex_digits_in_escape_sequence(location_t* loc);
void report_error_too_many_hex_digits_in_escape_sequence(location_t* loc);
void report_error_unknown_escape_sequence(location_t* loc);
void report_error_ill_formed_integer_literal(location_t* loc);
void report_error_ill_formed_floating_point_literal(location_t* loc);
void report_error_unexpected_token(location_t* loc);
void report_error_failed_to_open_file(const char* path);
void report_error_missing_default_parameter(location_t* param_loc, location_t* first_param_loc);
void report_error_parameter_redefinition(location_t* param_loc, location_t* redefined_loc);
void report_error_variable_redeclaration(location_t* var_loc, location_t* redeclared_loc);
void report_error_enumerator_redeclaration(location_t* enum_loc, location_t* redeclared_loc);
void report_error_symbol_redeclaration(location_t* loc);
void report_error_undefined_symbol(location_t* loc);
void report_error_symbol_is_not_an_expression(location_t* loc);
void report_error_undefined_typename(location_t* loc);
void report_error_symbol_is_not_a_typename(location_t* loc);
void report_warning_shadowed_variable(location_t* loc);
void report_warning_shadowed_symbol(location_t* loc);
void report_error_expected_reference_type(location_t* loc);
void report_error_expected_generator_type(location_t* loc);
void report_error_expected_arithmetic_type(location_t* loc);
void report_error_expected_bool_type(location_t* loc);
void report_error_expected_ptr_type(location_t* loc);
void report_error_expected_optional_type(location_t* loc);
void report_warning_mixed_signedness(location_t* loc);
void report_error_expected_integer_type(location_t* loc);
void report_error_incompatible_return_type(location_t* loc);
void report_error_too_many_arguments(location_t* loc);
void report_error_too_few_arguments(location_t* loc);
void report_error_expected_callable(location_t* loc);
void report_error_incompatible_param_type(location_t* loc);
void report_error_expected_subscriptable(location_t* loc);
void report_error_expected_owner(location_t* loc);
void report_error_expected_member(location_t* loc);
void report_error_no_member_with_name(location_t* loc);
void report_error_expected_ptr_to_owner(location_t* loc);
void report_error_type_mismatch(location_t* loc, typedesc_t* expected, typedesc_t* actual);
void report_error_missing_closing_parenthesis(location_t* loc);
void report_error_missing_closing_bracket(location_t* loc);
void report_error_missing_unary_argument(location_t* loc);
void report_error_missing_binary_argument(location_t* loc);
void report_error_missing_callee(location_t* loc);
void report_error_expected_module(location_t* loc);

#endif