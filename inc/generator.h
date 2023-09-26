/**
 * \file generator.h
 * 
 * \brief LLVM IR generator interface.
 * 
 * \details The LLVM IR generator is responsible for converting the AST into
 * LLVM Intermediate Representation (IR) code. It traverses the AST and emits
 * corresponding LLVM IR instructions that capture the semantics and behavior of
 * the source code. This IR code is a low-level representation that can be
 * optimized and compiled into machine code for execution on a target
 * architecture using the LLVM compiler infrastructure.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_GENERATOR_H
#define TAU_GENERATOR_H

#include <llvm-c/Types.h>

#include "ast.h"
#include "typetable.h"

/**
 * \brief Represents a generator for LLVM IR code.
 */
typedef struct generator_s generator_t;

/**
 * \brief Initializes a new generator.
 *
 * \returns A pointer to the newly initialized generator.
 */
generator_t* generator_init(void);

/**
 * \brief Frees the resources associated with the generator.
 *
 * \param[in] gen Pointer to the generator to be freed.
 */
void generator_free(generator_t* gen);

/**
 * \brief Visits and generates code for a pointer type in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] node Pointer to the AST node representing the pointer type.
 */
void generator_visit_type_ptr(generator_t* gen, ast_type_ptr_t* node);

/**
 * \brief Visits and generates code for an array type in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] node Pointer to the AST node representing the array type.
 */
void generator_visit_type_array(generator_t* gen, ast_type_array_t* node);

/**
 * \brief Visits and generates code for a type in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] node Pointer to the AST node representing the type.
 */
void generator_visit_type(generator_t* gen, ast_type_t* node);

/**
 * \brief Visits and generates code for an integer literal expression in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the integer literal
 * expression.
 */
void generator_visit_expr_lit_int(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_lit_t* node);

/**
 * \brief Visits and generates code for a floating-point literal expression in
 * the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the floating-point literal
 * expression.
 */
void generator_visit_expr_lit_flt(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_lit_t* node);

/**
 * \brief Visits and generates code for a string literal expression in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the string literal
 * expression.
 */
void generator_visit_expr_lit_str(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_lit_t* node);

/**
 * \brief Visits and generates code for a boolean literal expression in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the boolean literal
 * expression.
 */
void generator_visit_expr_lit_bool(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_lit_t* node);

/**
 * \brief Visits and generates code for a unary operation expression in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the unary operation
 * expression.
 */
void generator_visit_expr_op_unary(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node);

/**
 * \brief Visits and generates code for a binary operation expression in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the binary operation
 * expression.
 */
void generator_visit_expr_op_binary(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node);

/**
 * \brief Visits and generates code for a call expression in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the call expression.
 */
void generator_visit_expr_op_call(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_call_t* node);

/**
 * \brief Visits and generates code for a variable declaration expression in the
 * AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the variable declaration
 * expression.
 */
void generator_visit_expr_decl_var(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_decl_t* node);

/**
 * \brief Visits and generates code for a parameter declaration expression in the
 * AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the parameter declaration
 * expression.
 */
void generator_visit_expr_decl_param(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_decl_t* node);

/**
 * \brief Visits and generates code for a function declaration expression in the
 * AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the function declaration
 * expression.
 */
void generator_visit_expr_decl_fun(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_decl_t* node);

/**
 * \brief Visits and generates code for a declaration expression in the
 * AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the declaration expression.
 */
void generator_visit_expr_decl(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_decl_t* node);

/**
 * \brief Visits and generates code for an expression in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the expression.
 */
void generator_visit_expr(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_t* node);

/**
 * \brief Visits and generates code for an if-else statement in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the if-else statement.
 */
void generator_visit_stmt_if(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_if_t* node);

/**
 * \brief Visits and generates code for a return statement in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the return statement.
 */
void generator_visit_stmt_return(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_return_t* node);

/**
 * \brief Visits and generates code for a block statement in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the block statement.
 */
void generator_visit_stmt_block(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_block_t* node);

/**
 * \brief Visits and generates code for a statement in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the statement.
 */
void generator_visit_stmt(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_t* node);

/**
 * \brief Visits and generates code for a variable declaration in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the variable declaration.
 */
void generator_visit_decl_var(generator_t* gen, ast_decl_fun_t* fun_node, ast_decl_var_t* node);

/**
 * \brief Visits and generates code for a function declaration in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] node Pointer to the AST node representing the function declaration.
 */
void generator_visit_decl_fun(generator_t* gen, ast_decl_fun_t* node);

/**
 * \brief Visits and generates code for a parameter declaration in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] fun_node Pointer to the AST node representing the parent function.
 * \param[in] node Pointer to the AST node representing the parameter declaration.
 * \param[in] idx The index of the parameter in the function's parameter list.
 */
void generator_visit_decl_param(generator_t* gen, ast_decl_fun_t* fun_node, ast_decl_param_t* node, size_t idx);

/**
 * \brief Visits and generates code for a declaration in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] node Pointer to the AST node representing the declaration.
 */
void generator_visit_decl(generator_t* gen, ast_decl_t* node);

/**
 * \brief Visits and generates code for a program in the AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in] node Pointer to the AST node representing the program.
 */
void generator_visit_prog(generator_t* gen, ast_prog_t* node);

/**
 * \brief Generates code for the entire program AST.
 *
 * \param[in] gen Pointer to the generator.
 * \param[in,out] context LLVM context.
 * \param[in,out] module LLVM module.
 * \param[in] typetable Type table containing type information.
 * \param[in,out] node Pointer to the root of the program AST.
 */
void generator_generate(generator_t* gen, LLVMContextRef context, LLVMModuleRef module, typetable_t* typetable, ast_node_t* node);

#endif