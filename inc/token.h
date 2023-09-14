/**
 * \file token.h
 * 
 * \brief Token interface.
 * 
 * \details Tokens represent the smallest meaningful units of source code.
 * They are the result of lexical analysis, where the source code is divided
 * into a sequence of tokens such as identifiers, literals, keywords, and
 * punctuation symbols. Tokens serve as the input for the subsequent stages
 * of parsing and semantic analysis in the compiler. Each token has a specific
 * kind and may carry additional information, such as the source location, to
 * aid in the compilation process.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TOKEN_H
#define TAU_TOKEN_H

#include <stdbool.h>
#include <stdio.h>

#include "list.h"
#include "location.h"

/**
 * \brief Enumeration of token kinds.
 */
typedef enum token_kind_e
{
  TOK_UNKNOWN, // Unknown token

  TOK_ID, // Identifier

  TOK_LIT_INT, // Literal integer (e.g. 123)
  TOK_LIT_FLT, // Literal float (e.g. 1.23)
  TOK_LIT_STR, // Literal string (e.g. "abc")
  TOK_LIT_CHAR, // Literal character (e.g. 'a')
  TOK_LIT_BOOL, // Literal boolean
  TOK_LIT_NULL, // Literal null

  TOK_KW_IS, // Keyword `is`
  TOK_KW_AS, // Keyword `as`
  TOK_KW_SIZEOF, // Keyword `sizeof`
  TOK_KW_ALIGNOF, // Keyword `alignof`
  TOK_KW_TYPEOF, // Keyword `typeof`
  TOK_KW_IN, // Keyword `in`
  TOK_KW_FUN, // Keyword `fun`
  TOK_KW_GEN, // Keyword `gen`
  TOK_KW_STRUCT, // Keyword `struct`
  TOK_KW_UNION, // Keyword `union`
  TOK_KW_ENUM, // Keyword `enum`
  TOK_KW_MOD, // Keyword `mod`
  TOK_KW_USE, // Keyword `use`
  TOK_KW_IF, // Keyword `if`
  TOK_KW_THEN, // Keyword `then`
  TOK_KW_ELSE, // Keyword `else`
  TOK_KW_FOR, // Keyword `for`
  TOK_KW_WHILE, // Keyword `while`
  TOK_KW_DO, // Keyword `do`
  TOK_KW_BREAK, // Keyword `break`
  TOK_KW_CONTINUE, // Keyword `continue`
  TOK_KW_RETURN, // Keyword `return`
  TOK_KW_YIELD, // Keyword `yield`
  TOK_KW_MUT, // Keyword `mut`
  TOK_KW_CONST, // Keyword `const`
  TOK_KW_TYPE, // Keyword `type`
  TOK_KW_SELF, // Keyword `Self`
  TOK_KW_I8, // Keyword `i8`
  TOK_KW_I16, // Keyword `i16`
  TOK_KW_I32, // Keyword `i32`
  TOK_KW_I64, // Keyword `i64`
  TOK_KW_ISIZE, // Keyword `isize`
  TOK_KW_U8, // Keyword `u8`
  TOK_KW_U16, // Keyword `u16`
  TOK_KW_U32, // Keyword `u32`
  TOK_KW_U64, // Keyword `u64`
  TOK_KW_USIZE, // Keyword `usize`
  TOK_KW_F32, // Keyword `f32`
  TOK_KW_F64, // Keyword `f64`
  TOK_KW_BOOL, // Keyword `bool`
  TOK_KW_UNIT, // Keyword `unit`

  TOK_PUNCT_PLUS, // Punctuation `+`
  TOK_PUNCT_PLUS_PLUS, // Punctuation `++`
  TOK_PUNCT_PLUS_EQUAL, // Punctuation `+=`
  TOK_PUNCT_MINUS, // Punctuation `-`
  TOK_PUNCT_MINUS_MINUS, // Punctuation `--`
  TOK_PUNCT_MINUS_EQUAL, // Punctuation `-=`
  TOK_PUNCT_ASTERISK, // Punctuation `*`
  TOK_PUNCT_ASTERISK_EQUAL, // Punctuation `*=`
  TOK_PUNCT_ASTERISK_DOT, // Punctuation `*.`
  TOK_PUNCT_SLASH, // Punctuation `/`
  TOK_PUNCT_SLASH_EQUAL, // Punctuation `/=`
  TOK_PUNCT_PERCENT, // Punctuation `%`
  TOK_PUNCT_PERCENT_EQUAL, // Punctuation `%=`
  TOK_PUNCT_AMPERSAND, // Punctuation `&`
  TOK_PUNCT_AMPERSAND_AMPERSAND, // Punctuation `&&`
  TOK_PUNCT_AMPERSAND_EQUAL, // Punctuation `&=`
  TOK_PUNCT_BAR, // Punctuation `|`
  TOK_PUNCT_BAR_BAR, // Punctuation `||`
  TOK_PUNCT_BAR_EQUAL, // Punctuation `|=`
  TOK_PUNCT_HAT, // Punctuation `^`
  TOK_PUNCT_HAT_EQUAL, // Punctuation `^=`
  TOK_PUNCT_TILDE, // Punctuation `~`
  TOK_PUNCT_LESS, // Punctuation `<`
  TOK_PUNCT_LESS_LESS, // Punctuation `<<`
  TOK_PUNCT_LESS_LESS_EQUAL, // Punctuation `<<=`
  TOK_PUNCT_LESS_EQUAL, // Punctuation `<=`
  TOK_PUNCT_GREATER, // Punctuation `>`
  TOK_PUNCT_GREATER_GREATER, // Punctuation `>>`
  TOK_PUNCT_GREATER_GREATER_EQUAL, // Punctuation `>>=`
  TOK_PUNCT_GREATER_EQUAL, // Punctuation `>=`
  TOK_PUNCT_BANG, // Punctuation `!`
  TOK_PUNCT_BANG_EQUAL, // Punctuation `!=`
  TOK_PUNCT_DOT, // Punctuation `.`
  TOK_PUNCT_DOT_DOT, // Punctuation `..`
  TOK_PUNCT_DOT_DOT_DOT, // Punctuation `...`
  TOK_PUNCT_QUESTION, // Punctuation `?`
  TOK_PUNCT_QUESTION_DOT, // Punctuation `?.`
  TOK_PUNCT_EQUAL, // Punctuation `=`
  TOK_PUNCT_EQUAL_EQUAL, // Punctuation `==`
  TOK_PUNCT_COMMA, // Punctuation `,`
  TOK_PUNCT_COLON, // Punctuation `:`
  TOK_PUNCT_SEMICOLON, // Punctuation `;`
  TOK_PUNCT_PAREN_LEFT, // Punctuation `(`
  TOK_PUNCT_PAREN_RIGHT, // Punctuation `)`
  TOK_PUNCT_BRACKET_LEFT, // Punctuation `[`
  TOK_PUNCT_BRACKET_RIGHT, // Punctuation `]`
  TOK_PUNCT_BRACE_LEFT, // Punctuation `{`
  TOK_PUNCT_BRACE_RIGHT, // Punctuation `}`

  TOK_EOF, // End of file
} token_kind_t;

/**
 * \brief Structure representing a token.
 */
typedef struct token_s
{
  token_kind_t kind; // Token kind.
  location_t* loc; // Token location in the source code.
} token_t;

/**
 * \brief Initializes a new token with the specified kind and location.
 * 
 * \param[in] kind The kind of the token.
 * \param[in] loc The location information of the token.
 * \returns Pointer to the newly initialized token.
 */
token_t* token_init(token_kind_t kind, location_t* loc);

/**
 * \brief Frees the memory allocated for a token.
 * 
 * \param[in] tok The token to be freed.
 */
void token_free(token_t* tok);

/**
 * \brief Dumps the JSON representation of a list of tokens to the specified
 * stream.
 * 
 * \param[in] stream The stream to write the JSON output to.
 * \param[in] list The list of tokens to be dumped.
 */
void token_list_json_dump(FILE* stream, list_t* list);

/**
 * \brief Converts a token kind to its corresponding string representation.
 * 
 * \param[in] kind The token kind to be converted.
 * \returns The string representation of the token kind.
 */
const char* token_kind_to_string(token_kind_t kind);

/**
 * \brief Checks if a token is a literal token.
 * 
 * \param[in] tok The token to be checked.
 * \returns `true` if the token is a literal token, `false` otherwise.
 */
bool token_is_lit(token_t* tok);

/**
 * \brief Checks if a token is a keyword token.
 * 
 * \param[in] tok The token to be checked.
 * \returns `true` if the token is a keyword token, `false` otherwise.
 */
bool token_is_kw(token_t* tok);

/**
 * \brief Checks if a token is a punctuation token.
 * 
 * \param[in] tok The token to be checked.
 * \returns `true` if the token is a punctuation token, `false` otherwise.
 */
bool token_is_punct(token_t* tok);

#endif
