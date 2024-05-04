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

#include <stddef.h>
#include <stdbool.h>

#include "ast/ast.h"
#include "stages/lexer/token.h"
#include "stages/parser/parser.h"
#include "utils/collections/queue.h"
#include "utils/collections/stack.h"

/**
 * \brief Enumeration of element kinds.
 */
typedef enum shyd_elem_kind_t
{
  SHYD_TERM,
  SHYD_TYPE,
  SHYD_OP,
  SHYD_PAREN_OPEN,
  SHYD_BRACKET_OPEN,
} shyd_elem_kind_t;

/**
 * \brief Shunting yard element.
 */
typedef struct shyd_elem_t
{
  shyd_elem_kind_t kind;
  token_t* tok;
  ast_node_t* node;
  op_kind_t op;
} shyd_elem_t;

/**
 * \brief Shunting yard context.
 */
typedef struct shyd_ctx_t
{
  parser_t* par; // Pointer to the parser to be used.

  queue_t* out_queue; // Output queue.
  stack_t* op_stack; // Operator stack.

  bool prev_term; // `true` if the previously parsed element was a term, `false` otherwise.
} shyd_ctx_t;

/**
 * \brief Initializes a new shunting yard context.
 * 
 * \param[in] par Pointer to the parser to be used.
 * \returns Pointer to the newly initialized shunting yard context.
 */
shyd_ctx_t* shyd_init(parser_t* par);

/**
 * \brief Frees all memory associated with a shunting yard context.
 * 
 * \param[in] ctx Pointer to the shunting yard context to be freed.
 */
void shyd_free(shyd_ctx_t* ctx);

/**
 * \brief Initializes a new shunting yard element.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] kind Element kind.
 * \returns Pointer to the newly initialized shunting yard element.
 */
shyd_elem_t* shyd_elem_init(shyd_ctx_t* ctx, shyd_elem_kind_t kind);

/**
 * \brief Frees all memory associated with a shunting yard element.
 * 
 * \param[in] elem Pointer to the shunting yard element to be freed.
 */
void shyd_elem_free(shyd_elem_t* elem);

/**
 * \brief Move elements from the operator stack into the output queue until an
 * element of a specific kind is encountered.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] kind Element kind to flush until.
 * \returns `true` if an element of the specified kind was encountered, `false`
 * otherwise.
 */
bool shyd_op_flush_until_elem(shyd_ctx_t* ctx, shyd_elem_kind_t kind);

/**
 * \brief Move elements from the operator stack into the output queue according
 * to the next operator and precedence rules.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] op Kind of the next operator.
 */
void shyd_op_flush_for_op(shyd_ctx_t* ctx, op_kind_t op);

/**
 * \brief Parses an expression that contains a type.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \return `true` if a typed exression was parsed successfully, `false` otherwise.
 */
bool shyd_parse_typed_expr(shyd_ctx_t* ctx);

/**
 * \brief Parses a left parenthesis.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \return `true` if a left parenthesis was parsed successfully, `false` otherwise.
 */
bool shyd_parse_paren_left(shyd_ctx_t* ctx);

/**
 * \brief Parses a right parenthesis.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \return `true` if a right parenthesis was parsed successfully, `false` otherwise.
 */
bool shyd_parse_paren_right(shyd_ctx_t* ctx);

/**
 * \brief Parses a left bracket.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \return `true` if a left bracket was parsed successfully, `false` otherwise.
 */
bool shyd_parse_bracket_left(shyd_ctx_t* ctx);

/**
 * \brief Parses a right bracket.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \return `true` if a right bracket was parsed successfully, `false` otherwise.
 */
bool shyd_parse_bracket_right(shyd_ctx_t* ctx);

/**
 * \brief Parses a call expression.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \return `true` if a call expression was parsed successfully, `false` otherwise.
 */
bool shyd_parse_call(shyd_ctx_t* ctx);

/**
 * \brief Parses an expression term.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \return `true` if a term was parsed successfully, `false` otherwise.
 */
bool shyd_parse_term(shyd_ctx_t* ctx);

/**
 * \brief Parses an operator.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \return `true` if an operator was parsed successfully, `false` otherwise.
 */
bool shyd_parse_op(shyd_ctx_t* ctx);

/**
 * \brief Parse the next element of an expression.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \returns `true` if the expression may have more terms to be parsed, `false`
 * otherwise.
 */
bool shyd_parse_postfix_next(shyd_ctx_t* ctx);

/**
 * \brief Parse an expression and store the resulting elements in the output
 * queue in postfix order.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 */
void shyd_parse_postfix(shyd_ctx_t* ctx);

/**
 * \brief Creates an AST unary operator expression node and pushes it onto the
 * node stack.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] elem Pointer to the shunting yard unary operator element.
 * \param[in] node_stack Pointer to the AST node stack.
 */
void shyd_ast_term(shyd_ctx_t* ctx, shyd_elem_t* elem, stack_t* node_stack);

/**
 * \brief Creates an AST unary operator expression node and pushes it onto the
 * node stack.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] elem Pointer to the shunting yard unary operator element.
 * \param[in] node_stack Pointer to the AST node stack.
 */
void shyd_ast_op_un(shyd_ctx_t* ctx, shyd_elem_t* elem, stack_t* node_stack);

/**
 * \brief Creates an AST binary operator expression node and pushes it onto the
 * node stack.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] elem Pointer to the shunting yard binary operator element.
 * \param[in] node_stack Pointer to the AST node stack.
 */
void shyd_ast_op_bin(shyd_ctx_t* ctx, shyd_elem_t* elem, stack_t* node_stack);

/**
 * \brief Creates an AST call operator expression node and pushes it onto the
 * node stack.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] elem Pointer to the shunting yard call operator element.
 * \param[in] node_stack Pointer to the AST node stack.
 */
void shyd_ast_op_call(shyd_ctx_t* ctx, shyd_elem_t* elem, stack_t* node_stack);

/**
 * \brief Creates an AST operator expression node and pushes it onto the node
 * stack.
 * 
 * \param[in] ctx Pointer to the shunting yard context.
 * \param[in] elem Pointer to the shunting yard operator element.
 * \param[in] node_stack Pointer to the AST node stack.
 */
void shyd_ast_op(shyd_ctx_t* ctx, shyd_elem_t* elem, stack_t* node_stack);

/**
 * \brief Parses an expression.
 * 
 * \param[in] par Pointer to the parser to be used.
 * \returns Expression node.
 */
ast_node_t* shyd_parse_expr(parser_t* par);

#endif