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
#include "utils/collections/vector.h"

TAU_EXTERN_C_BEGIN

/**
 * \see ast/stmt/while.h
 */
typedef struct ast_stmt_while_t ast_stmt_while_t;

/**
 * \see ast/stmt/do_while.h
 */
typedef struct ast_stmt_do_while_t ast_stmt_do_while_t;

/**
 * \see ast/stmt/loop.h
 */
typedef struct ast_stmt_loop_t ast_stmt_loop_t;

/**
 * \see ast/stmt/for.h
 */
typedef struct ast_stmt_for_t ast_stmt_for_t;

/**
 * \see ast/stmt/defer.h
 */
typedef struct ast_stmt_defer_t ast_stmt_defer_t;

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
  vector_t* stmts; // Stack of AST statements (for, while, defer) currently being visited.
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

/**
 * \brief Marks the beginning of a new while statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in] node Pointer to the AST while statement node.
 */
void ctrlflow_ctx_while_begin(ctrlflow_ctx_t* ctx, ast_stmt_while_t* node);

/**
 * \brief Marks the end of the current while statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 */
void ctrlflow_ctx_while_end(ctrlflow_ctx_t* ctx);

/**
 * \brief Marks the beginning of a new do-while statement.
 *
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in] node Pointer to the AST while statement node.
 */
void ctrlflow_ctx_do_while_begin(ctrlflow_ctx_t* ctx, ast_stmt_do_while_t* node);

/**
 * \brief Marks the end of the current do-while statement.
 *
 * \param[in] ctx Pointer to the control flow analysis context.
 */
void ctrlflow_ctx_do_while_end(ctrlflow_ctx_t* ctx);

/**
 * \brief Marks the beginning of a new do-while statement.
 *
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in] node Pointer to the AST while statement node.
 */
void ctrlflow_ctx_loop_begin(ctrlflow_ctx_t* ctx, ast_stmt_loop_t* node);

/**
 * \brief Marks the end of the current do-while statement.
 *
 * \param[in] ctx Pointer to the control flow analysis context.
 */
void ctrlflow_ctx_loop_end(ctrlflow_ctx_t* ctx);

/**
 * \brief Marks the beginning of a new for statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in] node Pointer to the AST for statement node.
 */
void ctrlflow_ctx_for_begin(ctrlflow_ctx_t* ctx, ast_stmt_for_t* node);

/**
 * \brief Marks the end of the current for statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 */
void ctrlflow_ctx_for_end(ctrlflow_ctx_t* ctx);

/**
 * \brief Marks the beginning of a new defer statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in] node Pointer to the AST defer statement node.
 */
void ctrlflow_ctx_defer_begin(ctrlflow_ctx_t* ctx, ast_stmt_defer_t* node);

/**
 * \brief Marks the end of the current defer statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 */
void ctrlflow_ctx_defer_end(ctrlflow_ctx_t* ctx);

TAU_EXTERN_C_END

#endif
