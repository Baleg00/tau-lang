/**
 * \file
 * 
 * \brief Control flow analysis pass.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_CTRLFLOW_H
#define TAU_CTRLFLOW_H

#include "utils/collections/stack.h"

/**
 * \see ast/stmt/block.h
 */
typedef struct ast_stmt_block_t ast_stmt_block_t;

/**
 * \brief Control flow analysis context.
 */
typedef struct ctrlflow_ctx_t
{
  stack_t* blocks; // Stack of AST block statements currently being visited.
} ctrlflow_ctx_t;

/**
 * \brief Initializes a new control flow analysis context.
 * 
 * \returns Pointer to the newly initialized control flow analysis context.
 */
ctrlflow_ctx_t* ctrlflow_ctx_init(void);

/**
 * \brief Frees all memory allocated by a control flow analysis context.
 * 
 * \param[in] ctx Pointer to the control flow analysis context to be freed.
 */
void ctrlflow_ctx_free(ctrlflow_ctx_t* ctx);

/**
 * \brief Marks the beginning of a new block statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in] node Pointer to the AST block statement node.
 */
void ctrlflow_ctx_block_begin(ctrlflow_ctx_t* ctx, ast_stmt_block_t* node);

/**
 * \brief Marks the end of the current block statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 */
void ctrlflow_ctx_block_end(ctrlflow_ctx_t* ctx);

/**
 * \brief Gets the current block statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \returns Pointer to the current block statement, or `NULL` if there isn't one.
 */
ast_stmt_block_t* ctrlflow_ctx_block_cur(ctrlflow_ctx_t* ctx);

#endif