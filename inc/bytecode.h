#ifndef TAU_BYTECODE_H
#define TAU_BYTECODE_H

#include <stdint.h>

#include "opcode.h"
#include "ast.h"
#include "list.h"

typedef struct bytecode_label_s
{
  ast_node_t* node;
  ptrdiff_t offset;
} bytecode_label_t;

typedef struct bytecode_ref_s
{
  bytecode_label_t* label;
  ptrdiff_t offset;
} bytecode_ref_t;

typedef struct bytecode_s
{
  size_t capacity;  
  size_t size;  
  uint8_t* data;

  list_t* labels;
  list_t* refs;

  size_t sp;
} bytecode_t;

bytecode_label_t* bytecode_label_init(ast_node_t* node, ptrdiff_t offset);
void bytecode_label_free(bytecode_label_t* label);

bytecode_ref_t* bytecode_ref_init(bytecode_label_t* label, ptrdiff_t offset);
void bytecode_ref_free(bytecode_ref_t* ref);

bytecode_t* bytecode_init(void);
void bytecode_free(bytecode_t* bc);

void bytecode_expand(bytecode_t* bc);

void bytecode_emit_opcode(bytecode_t* bc, opcode_t op);
void bytecode_emit_bytes(bytecode_t* bc, void* buf, size_t size);

void bytecode_reg_fun(bytecode_t* bc, ast_decl_fun_t* node);
void bytecode_reg_var(bytecode_t* bc, ast_decl_var_t* node);
void bytecode_reg_param(bytecode_t* bc, ast_param_t* node, ptrdiff_t offset);

ptrdiff_t bytecode_label_offset(bytecode_t* bc, ast_node_t* node);

void bytecode_visit_expr_op_unary(bytecode_t* bc, ast_expr_op_un_t* node);
void bytecode_visit_expr_op_binary(bytecode_t* bc, ast_expr_op_bin_t* node);
void bytecode_visit_expr_op_call(bytecode_t* bc, ast_expr_op_call_t* node);
void bytecode_visit_expr_op(bytecode_t* bc, ast_expr_op_t* node);
void bytecode_visit_expr(bytecode_t* bc, ast_expr_t* node);

void bytecode_visit_stmt_if(bytecode_t* bc, ast_stmt_if_t* node);
void bytecode_visit_stmt_for(bytecode_t* bc, ast_stmt_for_t* node);
void bytecode_visit_stmt_while(bytecode_t* bc, ast_stmt_while_t* node);
void bytecode_visit_stmt_break(bytecode_t* bc, ast_stmt_break_t* node);
void bytecode_visit_stmt_continue(bytecode_t* bc, ast_stmt_continue_t* node);
void bytecode_visit_stmt_return(bytecode_t* bc, ast_stmt_return_t* node);
void bytecode_visit_stmt_yield(bytecode_t* bc, ast_stmt_yield_t* node);
void bytecode_visit_stmt_block(bytecode_t* bc, ast_stmt_block_t* node);
void bytecode_visit_stmt_expr(bytecode_t* bc, ast_stmt_expr_t* node);
void bytecode_visit_stmt(bytecode_t* bc, ast_stmt_t* node);

void bytecode_visit_decl_var(bytecode_t* bc, ast_decl_var_t* node);
void bytecode_visit_decl_fun(bytecode_t* bc, ast_decl_fun_t* node);
void bytecode_visit_decl_gen(bytecode_t* bc, ast_decl_gen_t* node);
void bytecode_visit_decl_mod(bytecode_t* bc, ast_decl_mod_t* node);
void bytecode_visit_decl_generic(bytecode_t* bc, ast_decl_generic_t* node);
void bytecode_visit_decl(bytecode_t* bc, ast_decl_t* node);

void bytecode_visit_prog(bytecode_t* bc, ast_prog_t* node);

#endif