/**
 * \file
 * 
 * \brief Abstract syntax tree binary operation expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_BIN_H
#define TAU_AST_EXPR_OP_BIN_H

#include "ast/expr/op/op.h"

/**
 * \brief Header for all AST binary operation expression nodes.
 */
#define AST_EXPR_OP_BIN_HEADER\
  AST_EXPR_OP_HEADER;\
  ast_node_t* lhs; /** The associated left-hand side expression node. */\
  ast_node_t* rhs /** The associated right-hand side expression node. */

/**
 * \brief AST binary operation expression node.
 */
typedef struct ast_expr_op_bin_t
{
  AST_EXPR_OP_BIN_HEADER;
} ast_expr_op_bin_t;

/**
 * \brief Initializes a new AST binary operation expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_op_bin_t* ast_expr_op_bin_init(void);

/**
 * \brief Frees all memory allocated by an AST binary operation expression node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_expr_op_bin_free(ast_expr_op_bin_t* node);

/**
 * \brief Performs name resolution pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_as_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_add_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_sub_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_mul_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_div_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_mod_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_bit_and_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_bit_or_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_bit_xor_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_bit_lsh_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_bit_rsh_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_logic_and_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_logic_or_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_eq_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_ne_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_lt_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_le_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_gt_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_ge_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_arit_add_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_arit_sub_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_arit_mul_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_arit_div_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_arit_mod_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_bit_and_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_bit_or_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_bit_xor_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_bit_lsh_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_bit_rsh_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_subs_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_as_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_add_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_sub_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_mul_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_div_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_mod_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_bit_and_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_bit_or_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_bit_xor_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_bit_lsh_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_bit_rsh_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_logic_and_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_logic_or_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_eq_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_ne_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_lt_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_le_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_gt_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_ge_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_arit_add_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_arit_sub_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_arit_mul_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_arit_div_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_arit_mod_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_bit_and_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_bit_or_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_bit_xor_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_bit_lsh_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_assign_bit_rsh_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_subs_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Writes a JSON dump of an AST binary operation expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_expr_op_bin_dump_json(FILE* stream, ast_expr_op_bin_t* node);

#endif