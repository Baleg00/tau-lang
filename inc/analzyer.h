#ifndef TAU_ANALYZER_H
#define TAU_ANALYZER_H

#include "arena.h"
#include "list.h"
#include "stack.h"
#include "typedefs.h"
#include "typetable.h"

struct analyzer_s
{
  arena_t* arena;

  symtable_t* symtable;
  typetable_t* typetable;

  stack_t* ret_types;
};

void analyzer_init(arena_t* arena, analyzer_t* analyzer);
void analyzer_free(analyzer_t* analyzer);

void analyzer_visit_expr_op_is(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node);
void analyzer_visit_expr_op_as(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node);
void analyzer_visit_expr_op_unary(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_un_t* node);
void analyzer_visit_expr_op_binary(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node);
void analyzer_visit_expr_op_call(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_call_t* node);
void analyzer_visit_expr_op_member(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node);
void analyzer_visit_expr_op(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_t* node);
ast_node_t* analyzer_visit_expr(analyzer_t* analyzer, symtable_t* scope, ast_expr_t* node);

ast_node_t* analyzer_visit_type_member(analyzer_t* analyzer, symtable_t* scope, ast_type_member_t* node);
ast_node_t* analyzer_visit_type(analyzer_t* analyzer, symtable_t* scope, ast_type_t* node);

void analyzer_visit_stmt_if(analyzer_t* analyzer, symtable_t* scope, ast_stmt_if_t* node);
void analyzer_visit_stmt_for(analyzer_t* analyzer, symtable_t* scope, ast_stmt_for_t* node);
void analyzer_visit_stmt_while(analyzer_t* analyzer, symtable_t* scope, ast_stmt_while_t* node);
void analyzer_visit_stmt_break(analyzer_t* analyzer, symtable_t* scope, ast_stmt_break_t* node);
void analyzer_visit_stmt_continue(analyzer_t* analyzer, symtable_t* scope, ast_stmt_continue_t* node);
typedesc_t* analyzer_visit_stmt_return(analyzer_t* analyzer, symtable_t* scope, ast_stmt_return_t* node);
typedesc_t* analyzer_visit_stmt_yield(analyzer_t* analyzer, symtable_t* scope, ast_stmt_yield_t* node);
void analyzer_visit_stmt_block(analyzer_t* analyzer, symtable_t* scope, ast_stmt_block_t* node);
typedesc_t* analyzer_visit_stmt_expr(analyzer_t* analyzer, symtable_t* scope, ast_stmt_expr_t* node);
void analyzer_visit_stmt(analyzer_t* analyzer, symtable_t* scope, ast_stmt_t* node);

void analyzer_visit_decl_var(analyzer_t* analyzer, symtable_t* scope, ast_decl_var_t* node);
void analyzer_visit_decl_loop_var(analyzer_t* analyzer, symtable_t* scope, ast_decl_loop_var_t* node);
void analyzer_visit_decl_fun(analyzer_t* analyzer, symtable_t* scope, ast_decl_fun_t* node);
void analyzer_visit_decl_gen(analyzer_t* analyzer, symtable_t* scope, ast_decl_gen_t* node);
void analyzer_visit_decl_struct(analyzer_t* analyzer, symtable_t* scope, ast_decl_struct_t* node);
void analyzer_visit_decl_union(analyzer_t* analyzer, symtable_t* scope, ast_decl_union_t* node);
void analyzer_visit_decl_enum(analyzer_t* analyzer, symtable_t* scope, ast_decl_enum_t* node);
void analyzer_visit_decl_mod(analyzer_t* analyzer, symtable_t* scope, ast_decl_mod_t* node);
void analyzer_visit_decl_generic(analyzer_t* analyzer, symtable_t* scope, ast_decl_generic_t* node);
void analyzer_visit_decl(analyzer_t* analyzer, symtable_t* scope, ast_decl_t* node);

void analyzer_visit_decl_param(analyzer_t* analyzer, symtable_t* scope, ast_decl_param_t* node);
void analyzer_visit_decl_param_default(analyzer_t* analyzer, symtable_t* scope, ast_decl_param_default_t* node);
void analyzer_visit_decl_param_variadic(analyzer_t* analyzer, symtable_t* scope, ast_decl_param_variadic_t* node);
void analyzer_visit_decl_param_generic(analyzer_t* analyzer, symtable_t* scope, ast_decl_param_generic_t* node);

void analyzer_visit_decl_enum_constant(analyzer_t* analyzer, symtable_t* scope, symbol_t* enum_sym, ast_decl_enum_constant_t* node);

void analyzer_visit_prog(analyzer_t* analyzer, symtable_t* scope, ast_prog_t* node);

void analyzer_analyze(analyzer_t* analyzer, ast_node_t* root, symtable_t* symtable, typetable_t* typetable);

#endif