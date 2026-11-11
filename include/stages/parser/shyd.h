/**
 * \file
 * 
 * \brief Shunting yard algorithm.
 * 
 * \details The shunting yard algorithm, devised by Edsger Dijkstra, is a method
 * for parsing expressions specified in infix notation into postfix notation,
 * also known as Reverse Polish Notation. The shunting yard algorithm serves as
 * a crucial component in the syntax analysis, enabling the compiler to
 * efficiently evaluate expressions and determine their order of operations
 * without the ambiguity inherent in infix notation.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_SHYD_H
#define TAU_SHYD_H

#include "ast/ast.h"
#include "stages/lexer/token/token.h"
#include "stages/parser/parser.h"
#include "utils/common.h"
#include "utils/collections/queue.h"
#include "utils/collections/stack.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Enumeration of element kinds.
 */
typedef enum tau_shyd_elem_kind_t
{
  TAU_SHYD_TERM,
  TAU_SHYD_TYPE,
  TAU_SHYD_OP,
  TAU_SHYD_PAREN_OPEN,
  TAU_SHYD_BRACKET_OPEN,
} tau_shyd_elem_kind_t;

/**
 * \brief Shunting yard element.
 */
typedef struct tau_shyd_elem_t
{
  tau_shyd_elem_kind_t kind;
  tau_token_t* tok;
  tau_ast_node_t* node;
  op_kind_t op;
} tau_shyd_elem_t;

/**
 * \brief Shunting yard context.
 */
typedef struct tau_shyd_ctx_t
{
  tau_parser_t* par;       ///< Pointer to the parser to be used.
  tau_queue_t* out_queue;  ///< Queue of shunting yard output elements.
  tau_stack_t* op_stack;   ///< Stack of shunting yard operator elements.
  tau_stack_t* node_stack; ///< Stack of result AST nodes.
  bool prev_term;      ///< `true` if the previously parsed element was a term, `false` otherwise.
} tau_shyd_ctx_t;

/**
 * \brief Initializes a new shunting yard context.
 * 
 * \param[in] par Pointer to the parser to be used.
 * \returns Pointer to the newly initialized shunting yard context.
 */
tau_shyd_ctx_t* tau_shyd_ctx_init(tau_parser_t* par);

/**
 * \brief Frees all memory associated with a shunting yard context.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be freed.
 */
void tau_shyd_ctx_free(tau_shyd_ctx_t* ctx);

/**
 * \brief Initializes a new shunting yard element.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] kind Element kind.
 * \returns Pointer to the newly initialized shunting yard element.
 */
tau_shyd_elem_t* tau_shyd_elem_init(tau_shyd_ctx_t* ctx, tau_shyd_elem_kind_t kind);

/**
 * \brief Frees all memory associated with a shunting yard element.
 * 
 * \param[in] elem Pointer to the shunting yard element to be freed.
 */
void tau_shyd_elem_free(tau_shyd_elem_t* elem);

/**
 * \brief Move elements from the operator stack into the output queue until an
 * element of a specific kind is encountered.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \param[in] kind Element kind to flush until.
 * \returns `true` if an element of the specified kind was encountered, `false`
 * otherwise.
 */
bool tau_shyd_op_flush_until_elem(tau_shyd_ctx_t* ctx, tau_shyd_elem_kind_t kind);

/**
 * \brief Move elements from the operator stack into the output queue according
 * to the next operator and precedence rules.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \param[in] op Next operator kind.
 */
void tau_shyd_op_flush_for_op(tau_shyd_ctx_t* ctx, op_kind_t op);

/**
 * \brief Parses an expression that contains a type (e.g. sizeof, alignof).
 * 
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \return `true` if a typed expression was parsed successfully, `false` otherwise.
 */
bool tau_shyd_parse_expr_typed(tau_shyd_ctx_t* ctx);

/**
 * \brief Parses a left parenthesis.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \return `true` if a left parenthesis was parsed successfully, `false` otherwise.
 */
bool tau_shyd_parse_punct_paren_left(tau_shyd_ctx_t* ctx);

/**
 * \brief Parses a right parenthesis.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \return `true` if a right parenthesis was parsed successfully, `false` otherwise.
 */
bool tau_shyd_parse_punct_paren_right(tau_shyd_ctx_t* ctx);

/**
 * \brief Parses a left bracket.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \return `true` if a left bracket was parsed successfully, `false` otherwise.
 */
bool tau_shyd_parse_punct_bracket_left(tau_shyd_ctx_t* ctx);

/**
 * \brief Parses a right bracket.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \return `true` if a right bracket was parsed successfully, `false` otherwise.
 */
bool tau_shyd_parse_punct_bracket_right(tau_shyd_ctx_t* ctx);

/**
 * \brief Parses a literal vector or matrix expression.
 *
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \return `true` if a literal was parsed successfully, `false` otherwise.
 */
bool tau_shyd_parse_lit_vec_or_mat(tau_shyd_ctx_t* ctx);

/**
 * \brief Parses a call operation expression.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \return `true` if a call operation expression was parsed successfully,
 * `false` otherwise.
 */
bool tau_shyd_parse_expr_op_call(tau_shyd_ctx_t* ctx);

/**
 * \brief Parses a generic specialization operation expression.
 *
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \return `true` if a generic specialization operation expression was parsed
 * successfully, `false` otherwise.
 */
bool tau_shyd_parse_expr_op_spec(tau_shyd_ctx_t* ctx);

/**
 * \brief Parses an expression term.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \return `true` if a term was parsed successfully, `false` otherwise.
 */
bool tau_shyd_parse_expr_term(tau_shyd_ctx_t* ctx);

/**
 * \brief Parses an operation expression.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \return `true` if an operation expression was parsed successfully,
 * `false` otherwise.
 */
bool tau_shyd_parse_expr_op(tau_shyd_ctx_t* ctx);

/**
 * \brief Parses the next element of an expression.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be used.
 * \returns `true` if the expression may have more terms to be parsed, `false`
 * otherwise.
 */
bool tau_shyd_parse_postfix_next(tau_shyd_ctx_t* ctx);

/**
 * \brief Parses an expression and stores the resulting elements in the output
 * queue in postfix order.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be used.
 */
void tau_shyd_parse_postfix(tau_shyd_ctx_t* ctx);

/**
 * \brief Creates an AST unary operator expression node and pushes it onto the
 * node stack.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] elem Pointer to the shunting yard unary operator element.
 */
void tau_shyd_ast_expr_term(tau_shyd_ctx_t* ctx, tau_shyd_elem_t* elem);

/**
 * \brief Creates an AST unary operator expression node and pushes it onto the
 * node stack.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] elem Pointer to the shunting yard unary operator element.
 */
void tau_shyd_ast_expr_op_un(tau_shyd_ctx_t* ctx, tau_shyd_elem_t* elem);

/**
 * \brief Creates an AST binary operator expression node and pushes it onto the
 * node stack.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] elem Pointer to the shunting yard binary operator element.
 */
void tau_shyd_ast_expr_op_bin(tau_shyd_ctx_t* ctx, tau_shyd_elem_t* elem);

/**
 * \brief Creates an AST call operator expression node and pushes it onto the
 * node stack.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] elem Pointer to the shunting yard call operator element.
 */
void tau_shyd_ast_expr_op_call(tau_shyd_ctx_t* ctx, tau_shyd_elem_t* elem);

/**
 * \brief Creates an AST generic specialization operator expression node and
 * pushes it onto the node stack.
 *
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] elem Pointer to the shunting yard specialization operator element.
 */
void tau_shyd_ast_expr_op_spec(tau_shyd_ctx_t* ctx, tau_shyd_elem_t* elem);

/**
 * \brief Creates an AST operator expression node and pushes it onto the node
 * stack.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] elem Pointer to the shunting yard operator element.
 */
void tau_shyd_ast_expr_op(tau_shyd_ctx_t* ctx, tau_shyd_elem_t* elem);

/**
 * \brief Parses an expression.
 * 
 * \param[in] par Pointer to the parser to be used.
 * \returns Expression node.
 */
tau_ast_node_t* tau_shyd_parse_expr(tau_parser_t* par);

TAU_EXTERN_C_END

#endif
