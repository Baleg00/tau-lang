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

void analyzer_visit_expr_op_is(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_expr_op_as(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_expr_op_unary(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_expr_op_binary(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_expr_op_call(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_expr_op_member(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_expr_op(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
ast_node_t* analyzer_visit_expr(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);

ast_node_t* analyzer_visit_type_member(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
ast_node_t* analyzer_visit_type(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);

void analyzer_visit_stmt_if(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_stmt_for(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_stmt_while(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_stmt_break(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_stmt_continue(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
typedesc_t* analyzer_visit_stmt_return(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
typedesc_t* analyzer_visit_stmt_yield(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_stmt_block(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
typedesc_t* analyzer_visit_stmt_expr(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_stmt(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);

void analyzer_visit_decl_var(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_decl_fun(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_decl_gen(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_decl_struct(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_decl_union(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_decl_enum(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_decl_mod(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);
void analyzer_visit_decl(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);

void analyzer_visit_decl_param(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);

void analyzer_visit_decl_enum_constant(analyzer_t* analyzer, symtable_t* scope, symbol_t* enum_sym, ast_node_t* node);

void analyzer_visit_prog(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node);

void analyzer_analyze(analyzer_t* analyzer, ast_node_t* root, symtable_t* symtable, typetable_t* typetable);

#endif