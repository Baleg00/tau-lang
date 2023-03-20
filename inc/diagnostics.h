#ifndef TAU_DIAGNOSTICS_H
#define TAU_DIAGNOSTICS_H

#include "location.h"
#include "types.h"

void report_error_unexpected_character(location_t* loc);

void report_error_missing_terminating_character(location_t* loc, char ch);

void report_error_empty_character_literal(location_t* loc);

void report_error_escape_no_hex_digits(location_t* loc);

void report_error_unknown_escape_sequence(location_t* loc);

void report_error_ill_formed_integer_literal(location_t* loc);
void report_error_ill_formed_float_literal(location_t* loc);

void report_error_unexpected_token(location_t* loc);

void report_error_failed_to_open_file(char* path);

void report_error_empty_generic_parameter_list(location_t* loc);
void report_error_missing_default_parameter(location_t* loc);

void report_error_parameter_redefinition(location_t* loc);
void report_error_variable_redeclaration(location_t* loc);
void report_error_enumerator_redeclaration(location_t* loc);
void report_error_symbol_redeclaration(location_t* loc);

void report_error_undefined_symbol(location_t* loc);
void report_error_symbol_is_not_an_expression(location_t* loc);

void report_error_undefined_typename(location_t* loc);
void report_error_symbol_is_not_a_typename(location_t* loc);

void report_warning_shadowed_variable(location_t* loc);
void report_warning_shadowed_symbol(location_t* loc);

void report_error_expected_arithmetic_type(location_t* loc);
void report_error_expected_bool_type(location_t* loc);
void report_error_expected_ptr_type(location_t* loc);
void report_error_expected_nullable_type(location_t* loc);
void report_error_mixed_signedness(location_t* loc);
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
void report_error_type_mismatch(location_t* loc, type_t* expected, type_t* actual);

#endif