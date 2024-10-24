/**
 * \file
 * 
 * \brief Syntax analyzer interface.
 * 
 * \details A syntax analyzer, also known as a parser, is a crucial component in
 * a compiler. Its primary function is to analyze the structure of the source
 * code and ensure that it adheres to the language's specified syntax rules.
 * This involves checking the order and arrangement of tokens to determine if
 * they form valid statements or expressions. If the code follows the correct
 * syntax, the syntax analyzer generates a data structure called the Abstract
 * Syntax Tree (AST). If errors are detected, it provides meaningful error
 * messages to aid developers in fixing the issues.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_PARSER_H
#define TAU_PARSER_H

#include <stdbool.h>

#include "ast/ast.h"
#include "stages/lexer/token/token.h"
#include "utils/collections/vector.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Represents a syntax analyzer.
 */
typedef struct parser_t parser_t;

/**
 * \brief Function pointer type to be used when parsing delimited or terminated
 * list of nodes.
 */
typedef ast_node_t*(*parse_func_t)(parser_t*);

/**
 * \brief Initializes a new parser.
 * 
 * \returns Pointer to the newly initialized parser.
 */
parser_t* parser_init(void);

/**
 * \brief Destroys a parser.
 * 
 * \details It is the caller's responsibility to properly free the AST.
 * 
 * \param[in] par Parser to be destroyed.
 */
void parser_free(parser_t* par);

/**
 * \brief Returns current token to be processed.
 * 
 * \param[in] par Parser to be used.
 * \returns Current token.
 */
token_t* parser_current(parser_t* par);

/**
 * \brief Returns the current token and advances to the next one.
 * 
 * \details Does not advance if there is no next token and returns an EOF token
 * instead.
 * 
 * \param[in] par Parser to be used.
 * \returns Next token or an EOF token if there is no next token.
 */
token_t* parser_next(parser_t* par);

/**
 * \brief Returns next token without advancing.
 * 
 * \param[in] par Parser to be used.
 * \returns Next token or an EOF token if there is no next token.
 */
token_t* parser_peek(parser_t* par);

/**
 * \brief Checks if the current token's kind matches the specified one and
 * advances if it does.
 * 
 * \param[in] par Parser to be used.
 * \param[in] kind Expected token kind.
 * \returns True if token kinds match, false otherwise.
 */
bool parser_consume(parser_t* par, token_kind_t kind);

/**
 * \brief Checks if the current token's kind matches the specified one. If it
 * does returns the token and advances to the next one, raises an error otherwise.
 * 
 * \param[in] par Parser to be used.
 * \param[in] kind Expected token kind.
 * \returns Current token.
 */
token_t* parser_expect(parser_t* par, token_kind_t kind);

/**
 * \brief Queries whether the parser ignores newlines when fetching tokens or not.
 *
 * \param[in] par Parser to be used.
 * \returns `true` if the parser ignores newlines, `false` otherwise.
 */
bool parser_get_ignore_newline(parser_t* par);

/**
 * \brief Sets whether the parser should ignore newlines when fetching tokens
 * or not.
 * 
 * \param[in] par Parser to be used.
 * \param[in] ignore `true` if newlines should be ignored, `false` otherwise.
 */
void parser_set_ignore_newline(parser_t* par, bool ignore);

/**
 * \brief Clears the parser's declaration context.
 * 
 * \param[in] par Parser to be used.
 */
void parser_decl_context_clear(parser_t* par);

/**
 * \brief Parses a public modifier and updates the declaration context.
 * 
 * \param[in] par Parser to be used.
 */
void parser_parse_decl_context_pub(parser_t* par);

/**
 * \brief Parses an external modifier and updates the declaration context.
 * 
 * \param[in] par Parser to be used.
 */
void parser_parse_decl_context_extern(parser_t* par);

/**
 * \brief Parses a list of nodes delimited by a specific token.
 * 
 * \details Parses nodes until the next token's kind does not match the
 * delimiter.
 * 
 * \param[in] par Parser to be used.
 * \param[out] dest Pointer to the vector to push parsed nodes onto.
 * \param[in] delim Delimiter token kind.
 * \param[in] parse_func Function to be used for parsing a node.
 */
void parser_parse_delimited_list(parser_t* par, vector_t* dest, token_kind_t delim, parse_func_t parse_func);

/**
 * \brief Parses a list of nodes terminated by a specific token.
 * 
 * \details Parses nodes until a token whose kind matches the terminator is
 * encountered.
 * 
 * \param[in] par Parser to be used.
 * \param[out] dest Pointer to the vector to push parsed nodes onto.
 * \param[in] termin Terminator token kind.
 * \param[in] parse_func Function to be used for parsing a node.
 */
void parser_parse_terminated_list(parser_t* par, vector_t* dest, token_kind_t termin, parse_func_t parse_func);

/**
 * \brief Parses a string literal and returns the corresponding calling convention kind.
 * 
 * \param[in] par Parser to be used.
 * \returns The calling convention kind.
 */
callconv_kind_t parser_parse_callconv(parser_t* par);

/**
 * \brief Parses an identifier token.
 * 
 * \param[in] par Parser to be used.
 * \returns Identifier node.
 */
ast_node_t* parser_parse_id(parser_t* par);

/**
 * \brief Parses a type identifier.
 * 
 * \param[in] par Parser to be used.
 * \returns Type identifier node.
 */
ast_node_t* parser_parse_type_id(parser_t* par);

/**
 * \brief Parses a mutable type.
 * 
 * \details Mutable types are prefixed with the `mut` keyword.
 * 
 * \param[in] par Parser to be used.
 * \returns Mutable type node.
 */
ast_node_t* parser_parse_type_mut(parser_t* par);

/**
 * \brief Parses a pointer type.
 * 
 * \details Pointer types are prefixed with an asterisk (`*`).
 * 
 * \param[in] par Parser to be used.
 * \returns Pointer type node.
 */
ast_node_t* parser_parse_type_ptr(parser_t* par);

/**
 * \brief Parses a sized or unsized array type.
 * 
 * \details Pointer types are prefixed with an opening and closing bracket (`[]`).
 * A compile-time expression may be present between the brackets which must
 * evaluate to a non-negative integer.
 * 
 * \param[in] par Parser to be used.
 * \returns Array type node.
 */
ast_node_t* parser_parse_type_array(parser_t* par);

/**
 * \brief Parses a reference type.
 * 
 * \details Reference types are prefixed with an ampersand (`&`).
 * 
 * \param[in] par Parser to be used.
 * \returns Reference type node.
 */
ast_node_t* parser_parse_type_ref(parser_t* par);

/**
 * \brief Parses an optional type.
 * 
 * \details Optional types are prefixed with a question mark (`?`).
 * 
 * \param[in] par Parser to be used.
 * \returns Optional type node.
 */
ast_node_t* parser_parse_type_opt(parser_t* par);

/**
 * \brief Parses a function type.
 * 
 * \details Function types are introduced by the `fun` keyword followed by a
 * list of parameter types enclosed within parentheses (`()`) and delimited by
 * commas (`,`). The parameter type list is followed by a colon (`:`) and the
 * return type.
 * 
 * \param[in] par Parser to be used.
 * \returns Function type node.
 */
ast_node_t* parser_parse_type_fun(parser_t* par);

/**
 * \brief Parses a type that may be defined inside another one.
 * 
 * \details Member types are defined inside one or more namespaces and can be
 * accessed using the member operator (`.`).
 * 
 * \param[in] par Parser to be used.
 * \returns Member type node.
 */
ast_node_t* parser_parse_type_member(parser_t* par);

/**
 * \brief Parses a type.
 * 
 * \param[in] par Parser to be used.
 * \returns Type node.
 */
ast_node_t* parser_parse_type(parser_t* par);

/**
 * \brief Parses an expression.
 * 
 * \param[in] par Parser to be used.
 * \returns Expression node.
 */
ast_node_t* parser_parse_expr(parser_t* par);

/**
 * \brief Parses an if-statement.
 * 
 * \details An if statement is introduced by the `if` keyword followed by an
 * expression which evaluates to a boolean type and the `then` keyword followed
 * by a statement to be executed only if the condition is true. An optional
 * `else` keyword may be used which is followed by another statement to be
 * executed only if the condition is not met.
 * 
 * \param[in] par Parser to be used.
 * \returns If-statement node.
 */
ast_node_t* parser_parse_stmt_if(parser_t* par);

/**
 * \brief Parses a for-loop.
 * 
 * \details A for-loop is introduced by the `for` keyword followed by a loop
 * variable declaration, a `in` keyword, an expression evaluating to an iterable
 * type (generator or sized array), a `do` keyword and finally a statement. The
 * loop variable's type must match the iterable's value type. The loop variable
 * takes on and the statement is executed for each value in the iterable object.
 * 
 * \param[in] par Parser to be used.
 * \returns For-loop node.
 */
ast_node_t* parser_parse_stmt_for(parser_t* par);

/**
 * \brief Parses the loop variable of a for-loop.
 * 
 * \param[in] par Parser to be used.
 * \return Loop variable node.
 */
ast_node_t* parser_parse_stmt_for_var(parser_t* par);

/**
 * \brief Parses a while-loop.
 * 
 * \details A while-loop is introduced by the `while` keyword followed by an
 * expression evaluating to a boolean type, a `do` keyword and a statement.
 * The condition is checked before each execution of the statement and the loop
 * is terminated if the condition it is not met.
 * 
 * \param[in] par Parser to be used.
 * \returns While-loop node.
 */
ast_node_t* parser_parse_stmt_while(parser_t* par);

/**
 * \brief Parses a do-while-loop.
 *
 * \details A do-while-loop is introduced by the `do` keyword followed by a
 * statement, the `while` keyword, and an expression evaluating to a boolean type.
 * The condition is checked before each execution of the statement except the first
 * one and the loop is terminated if the condition it is not met.
 *
 * \param[in] par Parser to be used.
 * \returns Do-while-loop node.
 */
ast_node_t* parser_parse_stmt_do_while(parser_t* par);

/**
 * \brief Parses a loop.
 *
 * \details A loop is introduced by the `loop` keyword followed by a statement.
 *
 * \param[in] par Parser to be used.
 * \returns Loop node.
 */
ast_node_t* parser_parse_stmt_loop(parser_t* par);

/**
 * \brief Parses a break-statement.
 * 
 * \details A break-statement is just a `break` keyword. It prematurely
 * terminates a for-loop or while-loop.
 * 
 * \param[in] par Parser to be used.
 * \returns Break-statement node.
 */
ast_node_t* parser_parse_stmt_break(parser_t* par);

/**
 * \brief Parses a continue-statement.
 * 
 * \details A continue-statement is just a `continue` keyword. It prematurely
 * terminates the current iteration and jumps to the next one in a for-loop or
 * while-loop.
 * 
 * \param[in] par Parser to be used.
 * \returns Continue-statement node.
 */
ast_node_t* parser_parse_stmt_continue(parser_t* par);

/**
 * \brief Parses a return-statement.
 * 
 * \details A return-statement is introduced by the `return` keyword and is
 * optionally followed by an expression which evaluates to a type matching the
 * containing function's return type. If the expression is omitted, the
 * return type must be the `unit` type.
 * 
 * \param[in] par Parser to be used.
 * \returns Return-statement node.
 */
ast_node_t* parser_parse_stmt_return(parser_t* par);

/**
 * \brief Parses a defer-statement.
 * 
 * \details A defer-statement is introduced by the `defer` keyword and must be
 * followed by a statement.
 * 
 * \param[in] par Parser to be used.
 * \returns Defer-statement node.
 */
ast_node_t* parser_parse_stmt_defer(parser_t* par);

/**
 * \brief Parses a block-statement.
 * 
 * \details A block-statement is a collection of statements enclosed within
 * braces ('{}'). If the block statement is executed, the contained statements
 * are also executed in the order they appeared.
 * 
 * \param[in] par Parser to be used.
 * \returns Block-statement node.
 */
ast_node_t* parser_parse_stmt_block(parser_t* par);

/**
 * \brief Parses an expression as a statement.
 * 
 * \param[in] par Parser to be used.
 * \returns Expression-statement node.
 */
ast_node_t* parser_parse_stmt_expr(parser_t* par);

/**
 * \brief Parses a statement.
 * 
 * \param[in] par Parser to be used.
 * \returns Statement node.
 */
ast_node_t* parser_parse_stmt(parser_t* par);

/**
 * \brief Parses a variable declaration.
 * 
 * \details A variable is introduced by an identifier and a type separated by a
 * colon (`:`). It may optionally be initialized by an assignment (`=`) followed
 * by an expression evaluating to the same type.
 * 
 * \param[in] par Parser to be used.
 * \returns Variable declaration node.
 */
ast_node_t* parser_parse_decl_var(parser_t* par);

/**
 * \brief Parses a function declaration.
 * 
 * \details A function is introduced by the `fun` keyword followed by an
 * identifier, a parameter list and a return type. The function body may be
 * enclosed within braces (`{}`) if it consists of multiple statements.
 * 
 * \param[in] par Parser to be used.
 * \returns Function declaration node.
 */
ast_node_t* parser_parse_decl_fun(parser_t* par);

/**
 * \brief Parses a struct declaration.
 * 
 * \details A struct is introduced by the `struct` keyword followed by an
 * identifier and a possibly empty list of member variables enclosed within
 * braces (`{}`).
 * 
 * \param[in] par Parser to be used.
 * \returns Struct declaration node.
 */
ast_node_t* parser_parse_decl_struct(parser_t* par);

/**
 * \brief Parses a struct member declaration.
 * 
 * \param[in] par Parser to be used.
 * \returns Struct member declaration node.
 */
ast_node_t* parser_parse_decl_struct_member(parser_t* par);

/**
 * \brief Parses a union declaration.
 * 
 * \details A union is introduced by the `union` keyword followed by an
 * identifier and a possibly empty list of member variables enclosed within
 * braces (`{}`).
 * 
 * \param[in] par Parser to be used.
 * \returns Union declaration node.
 */
ast_node_t* parser_parse_decl_union(parser_t* par);

/**
 * \brief Parses a union member declaration.
 * 
 * \param[in] par Parser to be used.
 * \returns Union member declaration node.
 */
ast_node_t* parser_parse_decl_union_member(parser_t* par);

/**
 * \brief Parses an enum declaration.
 * 
 * \details An enum is introduced by the `enum` keyword followed by an
 * identifier and a possibly empty list of enumerators enclosed within braces
 * (`{}`).
 * 
 * \param[in] par Parser to be used.
 * \returns Enum declaration node.
 */
ast_node_t* parser_parse_decl_enum(parser_t* par);

/**
 * \brief Parses a module declaration.
 * 
 * \details A module is introduced by the `mod` keyword followed by an
 * identifier and a possibly empty list of declarations enclosed within braces
 * (`{}`).
 * 
 * \param[in] par Parser to be used.
 * \returns Module declaration node.
 */
ast_node_t* parser_parse_decl_mod(parser_t* par);

/**
 * \brief Parses a type alias declaration.
 *
 * \param[in] par Parser to be used.
 * \returns Module declaration node.
 */
ast_node_t* parser_parse_decl_type_alias(parser_t* par);

/**
 * \brief Parses a declaration.
 * 
 * \param[in] par Parser to be used.
 * \returns Declaration node.
 */
ast_node_t* parser_parse_decl(parser_t* par);

/**
 * \brief Parses a declaration inside a module.
 * 
 * \param[in] par Parser to be used.
 * \returns Declaration node.
 */
ast_node_t* parser_parse_decl_in_mod(parser_t* par);

/**
 * \brief Parses a top level declaration.
 * 
 * \param[in] par Parser to be used.
 * \returns Declaration node.
 */
ast_node_t* parser_parse_decl_top_level(parser_t* par);

/**
 * \brief Parses a simple function parameter.
 * 
 * \param[in] par Parser to be used.
 * \returns Parameter node.
 */
ast_node_t* parser_parse_decl_param(parser_t* par);

/**
 * \brief Parses an enumerator in an enum declaration.
 * 
 * \param[in] par Parser to be used.
 * \returns Enumerator node.
 */
ast_node_t* parser_parse_decl_enum_constant(parser_t* par);

/**
 * \brief Parses a path segment.
 * 
 * \param[in] par Parser to be used.
 * \returns Path segment node.
 */
ast_node_t* parser_parse_path_segment(parser_t* par);

/**
 * \brief Parses a path wildcard.
 * 
 * \param[in] par Parser to be used.
 * \returns Path wildcard node.
 */
ast_node_t* parser_parse_path_wildcard(parser_t* par);

/**
 * \brief Parses a path list.
 * 
 * \param[in] par Parser to be used.
 * \returns Path list node.
 */
ast_node_t* parser_parse_path_list(parser_t* par);

/**
 * \brief Parses a path.
 * 
 * \param[in] par Parser to be used.
 * \returns Path node.
 */
ast_node_t* parser_parse_path(parser_t* par);

/**
 * \brief Parses a use directive.
 * 
 * \param[in] par Parser to be used.
 * \returns Use directive node.
 */
ast_node_t* parser_parse_use_directive(parser_t* par);

/**
 * \brief Processes a list of tokens and produces an abstract syntax tree.
 * 
 * \param[in] par Parser to be used.
 * \param[in] toks Vector of tokens to be parsed.
 * \returns Pointer to the root node.
 */
ast_node_t* parser_parse(parser_t* par, vector_t* toks);

TAU_EXTERN_C_END

#endif
