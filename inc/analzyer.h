/**
 * \file analyzer.h
 * 
 * \brief Semantic analyzer interface.
 * 
 * \details A semantic analyzer is responsible for checking and ensuring the
 * correctness of the meaning or semantics of a program. It performs deeper
 * analysis beyond syntax checking to catch errors related to variable types,
 * scoping rules, function signatures, and more. This phase verifies that the
 * program adheres to the language's specifications and enforces the rules for
 * type safety, variable declaration and usage, function calls, and other
 * high-level language constructs. If any semantic errors are detected, such as
 * type mismatches or undeclared variables, the semantic analyzer generates error
 * messages to guide the programmer in correcting the issues before proceeding
 * to code generation.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_ANALYZER_H
#define TAU_ANALYZER_H

#include "symtable.h"
#include "typebuilder.h"
#include "typetable.h"

/**
 * \brief Represents a semantic analyzer.
 */
typedef struct analyzer_t analyzer_t;

/**
 * \brief Initializes a new semantic analyzer.
 *
 * \returns A pointer to the newly initialized analyzer.
 */
analyzer_t* analyzer_init(void);

/**
 * \brief Frees the resources associated with the semantic analyzer.
 *
 * \param[in] analyzer Pointer to the analyzer to be freed.
 */
void analyzer_free(analyzer_t* analyzer);

/**
 * \brief Visits and analyzes a binary expression with the `is` operator.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the binary expression node.
 */
void analyzer_visit_expr_op_is(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node);

/**
 * \brief Visits and analyzes a binary expression with the `as` operator.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the binary expression node.
 */
void analyzer_visit_expr_op_as(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node);

/**
 * \brief Visits and analyzes a unary expression.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the unary expression node.
 */
void analyzer_visit_expr_op_unary(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_un_t* node);

/**
 * \brief Visits and analyzes a binary expression.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the binary expression node.
 */
void analyzer_visit_expr_op_binary(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node);

/**
 * \brief Visits and analyzes a call expression.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the call expression node.
 */
void analyzer_visit_expr_op_call(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_call_t* node);

/**
 * \brief Visits and analyzes a member access expression.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the member access expression node.
 */
void analyzer_visit_expr_op_member(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node);

/**
 * \brief Visits and analyzes an operation expression.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the operation expression node.
 */
void analyzer_visit_expr_op(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_t* node);

/**
 * \brief Visits and analyzes an expression.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the expression node.
 * \returns A pointer to a node the original should be replaced with.
 */
ast_node_t* analyzer_visit_expr(analyzer_t* analyzer, symtable_t* scope, ast_expr_t* node);

/**
 * \brief Visits and analyzes a member type access operation.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the member type access node.
 * \returns A pointer to a node the original should be replaced with.
 */
ast_node_t* analyzer_visit_type_member(analyzer_t* analyzer, symtable_t* scope, ast_type_member_t* node);

/**
 * \brief Visits and analyzes a mutable type.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the mutable type node.
 */
void analyzer_visit_type_mut(analyzer_t* analyzer, symtable_t* scope, ast_type_mut_t* node);

/**
 * \brief Visits and analyzes a constant type.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the constant type node.
 */
void analyzer_visit_type_const(analyzer_t* analyzer, symtable_t* scope, ast_type_const_t* node);

/**
 * \brief Visits and analyzes a pointer type.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the pointer type node.
 */
void analyzer_visit_type_ptr(analyzer_t* analyzer, symtable_t* scope, ast_type_ptr_t* node);

/**
 * \brief Visits and analyzes an array type.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the array type node.
 */
void analyzer_visit_type_array(analyzer_t* analyzer, symtable_t* scope, ast_type_array_t* node);

/**
 * \brief Visits and analyzes a reference type.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the reference type node.
 */
void analyzer_visit_type_ref(analyzer_t* analyzer, symtable_t* scope, ast_type_ref_t* node);

/**
 * \brief Visits and analyzes an optional type.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the optional type node.
 */
void analyzer_visit_type_opt(analyzer_t* analyzer, symtable_t* scope, ast_type_opt_t* node);

/**
 * \brief Visits and analyzes a function type.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the function type node.
 */
void analyzer_visit_type_fun(analyzer_t* analyzer, symtable_t* scope, ast_type_fun_t* node);

/**
 * \brief Visits and analyzes a generator type.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the generator type node.
 */
void analyzer_visit_type_gen(analyzer_t* analyzer, symtable_t* scope, ast_type_gen_t* node);

/**
 * \brief Visits and analyzes a type.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the type node.
 * \returns A pointer to a node the original should be replaced with.
 */
ast_node_t* analyzer_visit_type(analyzer_t* analyzer, symtable_t* scope, ast_type_t* node);

/**
 * \brief Visits and analyzes an if-else statment.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the if-else statement node.
 */
void analyzer_visit_stmt_if(analyzer_t* analyzer, symtable_t* scope, ast_stmt_if_t* node);

/**
 * \brief Visits and analyzes a for-loop statment.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the for-loop statement node.
 */
void analyzer_visit_stmt_for(analyzer_t* analyzer, symtable_t* scope, ast_stmt_for_t* node);

/**
 * \brief Visits and analyzes a while-loop statment.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the while-loop statement node.
 */
void analyzer_visit_stmt_while(analyzer_t* analyzer, symtable_t* scope, ast_stmt_while_t* node);

/**
 * \brief Visits and analyzes a break statment.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the break statement node.
 */
void analyzer_visit_stmt_break(analyzer_t* analyzer, symtable_t* scope, ast_stmt_break_t* node);

/**
 * \brief Visits and analyzes a continue statment.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the continue statement node.
 */
void analyzer_visit_stmt_continue(analyzer_t* analyzer, symtable_t* scope, ast_stmt_continue_t* node);

/**
 * \brief Visits and analyzes a return statment.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the return statement node.
 * \returns A pointer to the type descriptor of the returned expression.
 */
typedesc_t* analyzer_visit_stmt_return(analyzer_t* analyzer, symtable_t* scope, ast_stmt_return_t* node);

/**
 * \brief Visits and analyzes a yield statment.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the yield statement node.
 * \returns A pointer to the type descriptor of the yielded expression.
 */
typedesc_t* analyzer_visit_stmt_yield(analyzer_t* analyzer, symtable_t* scope, ast_stmt_yield_t* node);

/**
 * \brief Visits and analyzes a block statment.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the block statement node.
 */
void analyzer_visit_stmt_block(analyzer_t* analyzer, symtable_t* scope, ast_stmt_block_t* node);

/**
 * \brief Visits and analyzes an expression statment.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the expression statement node.
 * \returns A pointer to the type descriptor of the expression.
 */
typedesc_t* analyzer_visit_stmt_expr(analyzer_t* analyzer, symtable_t* scope, ast_stmt_expr_t* node);

/**
 * \brief Visits and analyzes a statment.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the statement node.
 */
void analyzer_visit_stmt(analyzer_t* analyzer, symtable_t* scope, ast_stmt_t* node);

/**
 * \brief Visits and analyzes a variable declaration.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the variable declaration node.
 */
void analyzer_visit_decl_var(analyzer_t* analyzer, symtable_t* scope, ast_decl_var_t* node);

/**
 * \brief Visits and analyzes a parameter declaration.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the parameter declaration node.
 */
void analyzer_visit_decl_param(analyzer_t* analyzer, symtable_t* scope, ast_decl_param_t* node);

/**
 * \brief Visits and analyzes a function declaration.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the function declaration node.
 */
void analyzer_visit_decl_fun(analyzer_t* analyzer, symtable_t* scope, ast_decl_fun_t* node);

/**
 * \brief Visits and analyzes a generator declaration.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the generator declaration node.
 */
void analyzer_visit_decl_gen(analyzer_t* analyzer, symtable_t* scope, ast_decl_gen_t* node);

/**
 * \brief Visits and analyzes a structure declaration.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the structure declaration node.
 */
void analyzer_visit_decl_struct(analyzer_t* analyzer, symtable_t* scope, ast_decl_struct_t* node);

/**
 * \brief Visits and analyzes a union declaration.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the union declaration node.
 */
void analyzer_visit_decl_union(analyzer_t* analyzer, symtable_t* scope, ast_decl_union_t* node);

/**
 * \brief Visits and analyzes an enumeration declaration.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the enumeration declaration node.
 */
void analyzer_visit_decl_enum(analyzer_t* analyzer, symtable_t* scope, ast_decl_enum_t* node);

/**
 * \brief Visits and analyzes an enumeration constant declaration.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the enumeration constant declaration node.
 */
void analyzer_visit_decl_enum_constant(analyzer_t* analyzer, symtable_t* scope, symbol_t* enum_sym, ast_decl_enum_constant_t* node);

/**
 * \brief Visits and analyzes a module declaration.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the module declaration node.
 */
void analyzer_visit_decl_mod(analyzer_t* analyzer, symtable_t* scope, ast_decl_mod_t* node);

/**
 * \brief Visits and analyzes a declaration.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the declaration node.
 */
void analyzer_visit_decl(analyzer_t* analyzer, symtable_t* scope, ast_decl_t* node);

/**
 * \brief Visits and analyzes a program.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in] scope Pointer to the current symbol table scope.
 * \param[in] node Pointer to the program node.
 */
void analyzer_visit_prog(analyzer_t* analyzer, symtable_t* scope, ast_prog_t* node);

/**
 * \brief Analyzes a program.
 *
 * \param[in] analyzer Pointer to the semantic analyzer.
 * \param[in,out] symtable Pointer to the symbol table to be used.
 * \param[in,out] typetable Pointer to the type table to be used.
 * \param[in,out] builder Pointer to the type builder to be used.
 * \param[in,out] node Pointer to the root program node.
 */
void analyzer_analyze(analyzer_t* analyzer, symtable_t* symtable, typetable_t* typetable, typebuilder_t* builder, ast_node_t* node);

#endif