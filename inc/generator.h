#ifndef TAU_GENERATOR_H
#define TAU_GENERATOR_H

#include <llvm-c/Types.h>

#include "ast.h"

typedef struct generator_s generator_t;

generator_t* generator_init(void);

void generator_free(generator_t* gen);

void generator_visit_type_ptr(generator_t* gen, ast_type_ptr_t* node);
void generator_visit_type_array(generator_t* gen, ast_type_array_t* node);
void generator_visit_type(generator_t* gen, ast_type_t* node);

void generator_visit_expr_lit(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_expr_lit_t* node);
void generator_visit_expr_op_unary(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node);
void generator_visit_expr_op_binary(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node);
void generator_visit_expr_op_call(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_expr_op_call_t* node);
void generator_visit_expr_decl(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_expr_decl_t* node);
void generator_visit_expr(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_expr_t* node);

void generator_visit_stmt_if(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_stmt_if_t* node);
void generator_visit_stmt_return(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_stmt_return_t* node);
void generator_visit_stmt_block(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_stmt_block_t* node);
void generator_visit_stmt(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_stmt_t* node);

void generator_visit_decl_var(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_decl_var_t* node);
void generator_visit_decl_fun(generator_t* gen, ast_decl_fun_t* node);
void generator_visit_decl_param(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_decl_param_t* node, size_t idx);
void generator_visit_decl(generator_t* gen, ast_decl_t* node);

void generator_visit_prog(generator_t* gen, ast_prog_t* node);

LLVMModuleRef generator_generate(generator_t* gen, ast_node_t* node);

#endif