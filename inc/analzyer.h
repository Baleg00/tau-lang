#ifndef TAU_ANALYZER_H
#define TAU_ANALYZER_H

#include "ast.h"
#include "symtable.h"
#include "types.h"
#include "stack.h"

typedef struct analyzer_s
{
  symtable_t* root;
  stack_t* ret_types;
} analyzer_t;

analyzer_t* analyzer_init(void);
void analyzer_free(analyzer_t* analyzer);

void analyzer_visit_param(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_variadic_param(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_generic_param(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);

void analyzer_visit_loop_var(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);

void analyzer_visit_enumerator(analyzer_t* analyzer, symtable_t* table, symbol_t* enum_sym, ast_node_t* node);

type_t* analyzer_visit_expr_op_is(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
type_t* analyzer_visit_expr_op_as(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
type_t* analyzer_visit_expr_op_unary(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
type_t* analyzer_visit_expr_op_binary(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
type_t* analyzer_visit_expr_op_call(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
type_t* analyzer_visit_expr_op_member(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
type_t* analyzer_visit_expr_op(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
type_t* analyzer_visit_expr(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);

ast_node_t* analyzer_visit_type(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);

void analyzer_visit_stmt_if(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_stmt_for(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_stmt_while(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_stmt_break(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_stmt_continue(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
type_t* analyzer_visit_stmt_return(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
type_t* analyzer_visit_stmt_yield(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_stmt_block(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
type_t* analyzer_visit_stmt_expr(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_stmt(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);

void analyzer_visit_decl_var(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_decl_fun(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_decl_gen(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_decl_struct_member(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_decl_struct(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_decl_union_member(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_decl_union(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_decl_enum(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_decl_mod_member(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_decl_mod(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);
void analyzer_visit_decl(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);

void analyzer_visit_prog(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);

void analyzer_visit(analyzer_t* analyzer, symtable_t* table, ast_node_t* node);

symtable_t* analyzer_analyze(analyzer_t* analyzer, ast_node_t* root);

#endif