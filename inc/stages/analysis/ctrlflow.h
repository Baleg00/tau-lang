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
#include "utils/error.h"

TAU_EXTERN_C_BEGIN

/**
 * \see ast/stmt/while.h
 */
typedef struct tau_ast_stmt_while_t tau_ast_stmt_while_t;

/**
 * \see ast/stmt/do_while.h
 */
typedef struct tau_ast_stmt_do_while_t tau_ast_stmt_do_while_t;

/**
 * \see ast/stmt/loop.h
 */
typedef struct tau_ast_stmt_loop_t tau_ast_stmt_loop_t;

/**
 * \see ast/stmt/for.h
 */
typedef struct tau_ast_stmt_for_t tau_ast_stmt_for_t;

/**
 * \see ast/stmt/defer.h
 */
typedef struct tau_ast_stmt_defer_t tau_ast_stmt_defer_t;

/**
 * \see ast/stmt/block.h
 */
typedef struct tau_ast_stmt_block_t tau_ast_stmt_block_t;

/**
 * \brief Control flow analysis context.
 */
typedef struct tau_ctrlflow_ctx_t
{
  tau_stack_t* blocks; ///< Stack of AST block statements currently being visited.
  tau_vector_t* stmts; ///< Stack of AST statements (for, while, defer) currently being visited.
  tau_error_bag_t* errors; ///< Associated error bag to add errors to.
} tau_ctrlflow_ctx_t;

/**
 * \brief Initializes a new control flow analysis context.
 *
 * \param[in] errors Pointer to the error bag to be used.
 * \returns Pointer to the newly initialized control flow analysis context.
 */
tau_ctrlflow_ctx_t* tau_ctrlflow_ctx_init(tau_error_bag_t* errors);

/**
 * \brief Frees all memory allocated by a control flow analysis context.
 * 
 * \param[in] ctx Pointer to the control flow analysis context to be freed.
 */
void tau_ctrlflow_ctx_free(tau_ctrlflow_ctx_t* ctx);

/**
 * \brief Marks the beginning of a new block statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in] node Pointer to the AST block statement node.
 */
void tau_ctrlflow_ctx_block_begin(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_block_t* node);

/**
 * \brief Marks the end of the current block statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 */
void tau_ctrlflow_ctx_block_end(tau_ctrlflow_ctx_t* ctx);

/**
 * \brief Gets the current block statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \returns Pointer to the current block statement, or `NULL` if there isn't one.
 */
tau_ast_stmt_block_t* tau_ctrlflow_ctx_block_cur(tau_ctrlflow_ctx_t* ctx);

/**
 * \brief Marks the beginning of a new while statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in] node Pointer to the AST while statement node.
 */
void tau_ctrlflow_ctx_while_begin(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_while_t* node);

/**
 * \brief Marks the end of the current while statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 */
void tau_ctrlflow_ctx_while_end(tau_ctrlflow_ctx_t* ctx);

/**
 * \brief Marks the beginning of a new do-while statement.
 *
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in] node Pointer to the AST while statement node.
 */
void tau_ctrlflow_ctx_do_while_begin(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_do_while_t* node);

/**
 * \brief Marks the end of the current do-while statement.
 *
 * \param[in] ctx Pointer to the control flow analysis context.
 */
void tau_ctrlflow_ctx_do_while_end(tau_ctrlflow_ctx_t* ctx);

/**
 * \brief Marks the beginning of a new do-while statement.
 *
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in] node Pointer to the AST while statement node.
 */
void tau_ctrlflow_ctx_loop_begin(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_loop_t* node);

/**
 * \brief Marks the end of the current do-while statement.
 *
 * \param[in] ctx Pointer to the control flow analysis context.
 */
void tau_ctrlflow_ctx_loop_end(tau_ctrlflow_ctx_t* ctx);

/**
 * \brief Marks the beginning of a new for statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in] node Pointer to the AST for statement node.
 */
void tau_ctrlflow_ctx_for_begin(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_for_t* node);

/**
 * \brief Marks the end of the current for statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 */
void tau_ctrlflow_ctx_for_end(tau_ctrlflow_ctx_t* ctx);

/**
 * \brief Marks the beginning of a new defer statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in] node Pointer to the AST defer statement node.
 */
void tau_ctrlflow_ctx_defer_begin(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_defer_t* node);

/**
 * \brief Marks the end of the current defer statement.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 */
void tau_ctrlflow_ctx_defer_end(tau_ctrlflow_ctx_t* ctx);

TAU_EXTERN_C_END

#endif
