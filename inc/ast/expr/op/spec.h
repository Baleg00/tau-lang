/**
 * \file
 *
 * \brief Abstract syntax tree generic specialization operation expression node interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_SPEC_H
#define TAU_AST_EXPR_OP_SPEC_H

#include "ast/expr/op/op.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST generic specialization operation expression node.
 */
typedef struct ast_expr_op_spec_t
{
  AST_EXPR_OP_HEADER;
  ast_node_t* generic; ///< Pointer to the generic declaration node being specialized.
  vector_t* params;    ///< Vector of generic parameter expressions.
} ast_expr_op_spec_t;

/**
 * \brief Initializes a new AST generic specialization operation expression node.
 *
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_op_spec_t* ast_expr_op_spec_init(void);

/**
 * \brief Frees all memory allocated by an AST generic specialization operation expression node.
 *
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_expr_op_spec_free(ast_expr_op_spec_t* node);

/**
 * \brief Performs name resolution pass on an AST generic specialization operation expression node.
 *
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_spec_nameres(nameres_ctx_t* ctx, ast_expr_op_spec_t* node);

/**
 * \brief Performs type check pass on an AST generic specialization operation expression node.
 *
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_spec_typecheck(typecheck_ctx_t* ctx, ast_expr_op_spec_t* node);

/**
 * \brief Performs code generation pass on an AST generic specialization operation expression node.
 *
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_spec_codegen(codegen_ctx_t* ctx, ast_expr_op_spec_t* node);

/**
 * \brief Writes a JSON dump of an AST generic specialization operation expression node into a stream.
 *
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_expr_op_spec_dump_json(FILE* stream, ast_expr_op_spec_t* node);

TAU_EXTERN_C_END

#endif
