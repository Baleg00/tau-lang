/**
 * \file
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

#include "stages/lexer/location.h"
#include "utils/common.h"
#include "utils/str.h"
#include "utils/str_view.h"
#include "utils/collections/vector.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Enumeration of token kinds.
 */
typedef enum tau_token_kind_e
{
  TAU_TOK_UNKNOWN, ///< Unknown token

  TAU_TOK_ID, ///< Identifier

  TAU_TOK_LIT_INT, ///< Literal integer (e.g. 123)
  TAU_TOK_LIT_FLT, ///< Literal float (e.g. 1.23)
  TAU_TOK_LIT_STR, ///< Literal string (e.g. "abc")
  TAU_TOK_LIT_CHAR, ///< Literal character (e.g. 'a')
  TAU_TOK_LIT_BOOL, ///< Literal boolean
  TAU_TOK_LIT_NULL, ///< Literal null

  TAU_TOK_KW_IS, ///< Keyword `is`
  TAU_TOK_KW_AS, ///< Keyword `as`
  TAU_TOK_KW_SIZEOF, ///< Keyword `sizeof`
  TAU_TOK_KW_ALIGNOF, ///< Keyword `alignof`
  TAU_TOK_KW_USE, ///< Keyword `use`
  TAU_TOK_KW_IN, ///< Keyword `in`
  TAU_TOK_KW_PUB, ///< Keyword `pub`
  TAU_TOK_KW_EXTERN, ///< Keyword `extern`
  TAU_TOK_KW_FUN, ///< Keyword `fun`
  TAU_TOK_KW_STRUCT, ///< Keyword `struct`
  TAU_TOK_KW_UNION, ///< Keyword `union`
  TAU_TOK_KW_ENUM, ///< Keyword `enum`
  TAU_TOK_KW_MOD, ///< Keyword `mod`
  TAU_TOK_KW_IF, ///< Keyword `if`
  TAU_TOK_KW_THEN, ///< Keyword `then`
  TAU_TOK_KW_ELSE, ///< Keyword `else`
  TAU_TOK_KW_FOR, ///< Keyword `for`
  TAU_TOK_KW_WHILE, ///< Keyword `while`
  TAU_TOK_KW_DO, ///< Keyword `do`
  TAU_TOK_KW_LOOP, ///< Keyword `loop`
  TAU_TOK_KW_BREAK, ///< Keyword `break`
  TAU_TOK_KW_CONTINUE, ///< Keyword `continue`
  TAU_TOK_KW_RETURN, ///< Keyword `return`
  TAU_TOK_KW_DEFER, ///< Keyword `defer`
  TAU_TOK_KW_MUT, ///< Keyword `mut`
  TAU_TOK_KW_I8, ///< Keyword `i8`
  TAU_TOK_KW_I16, ///< Keyword `i16`
  TAU_TOK_KW_I32, ///< Keyword `i32`
  TAU_TOK_KW_I64, ///< Keyword `i64`
  TAU_TOK_KW_ISIZE, ///< Keyword `isize`
  TAU_TOK_KW_U8, ///< Keyword `u8`
  TAU_TOK_KW_U16, ///< Keyword `u16`
  TAU_TOK_KW_U32, ///< Keyword `u32`
  TAU_TOK_KW_U64, ///< Keyword `u64`
  TAU_TOK_KW_USIZE, ///< Keyword `usize`
  TAU_TOK_KW_F32, ///< Keyword `f32`
  TAU_TOK_KW_F64, ///< Keyword `f64`
  TAU_TOK_KW_C64, ///< Keyword `c64`
  TAU_TOK_KW_C128, ///< Keyword `c128`
  TAU_TOK_KW_VEC, ///< Keyword `vec`
  TAU_TOK_KW_MAT, ///< Keyword `mat`
  TAU_TOK_KW_CHAR, ///< Keyword `char`
  TAU_TOK_KW_BOOL, ///< Keyword `bool`
  TAU_TOK_KW_UNIT, ///< Keyword `unit`
  TAU_TOK_KW_TYPE, ///< Keyword `type`
  TAU_TOK_KW_UNDEF, ///< Keyword `undef`

  TAU_TOK_PUNCT_PLUS, ///< Punctuation `+`
  TAU_TOK_PUNCT_PLUS_PLUS, ///< Punctuation `++`
  TAU_TOK_PUNCT_PLUS_EQUAL, ///< Punctuation `+=`
  TAU_TOK_PUNCT_MINUS, ///< Punctuation `-`
  TAU_TOK_PUNCT_MINUS_MINUS, ///< Punctuation `--`
  TAU_TOK_PUNCT_MINUS_EQUAL, ///< Punctuation `-=`
  TAU_TOK_PUNCT_MINUS_GREATER, ///< Punctuation `->`
  TAU_TOK_PUNCT_ASTERISK, ///< Punctuation `*`
  TAU_TOK_PUNCT_ASTERISK_EQUAL, ///< Punctuation `*=`
  TAU_TOK_PUNCT_SLASH, ///< Punctuation `/`
  TAU_TOK_PUNCT_SLASH_EQUAL, ///< Punctuation `/=`
  TAU_TOK_PUNCT_PERCENT, ///< Punctuation `%`
  TAU_TOK_PUNCT_PERCENT_EQUAL, ///< Punctuation `%=`
  TAU_TOK_PUNCT_AMPERSAND, ///< Punctuation `&`
  TAU_TOK_PUNCT_AMPERSAND_AMPERSAND, ///< Punctuation `&&`
  TAU_TOK_PUNCT_AMPERSAND_EQUAL, ///< Punctuation `&=`
  TAU_TOK_PUNCT_BAR, ///< Punctuation `|`
  TAU_TOK_PUNCT_BAR_BAR, ///< Punctuation `||`
  TAU_TOK_PUNCT_BAR_EQUAL, ///< Punctuation `|=`
  TAU_TOK_PUNCT_HAT, ///< Punctuation `^`
  TAU_TOK_PUNCT_HAT_EQUAL, ///< Punctuation `^=`
  TAU_TOK_PUNCT_TILDE, ///< Punctuation `~`
  TAU_TOK_PUNCT_LESS, ///< Punctuation `<`
  TAU_TOK_PUNCT_LESS_LESS, ///< Punctuation `<<`
  TAU_TOK_PUNCT_LESS_LESS_EQUAL, ///< Punctuation `<<=`
  TAU_TOK_PUNCT_LESS_EQUAL, ///< Punctuation `<=`
  TAU_TOK_PUNCT_GREATER, ///< Punctuation `>`
  TAU_TOK_PUNCT_GREATER_GREATER, ///< Punctuation `>>`
  TAU_TOK_PUNCT_GREATER_GREATER_EQUAL, ///< Punctuation `>>=`
  TAU_TOK_PUNCT_GREATER_EQUAL, ///< Punctuation `>=`
  TAU_TOK_PUNCT_BANG, ///< Punctuation `!`
  TAU_TOK_PUNCT_BANG_EQUAL, ///< Punctuation `!=`
  TAU_TOK_PUNCT_DOT, ///< Punctuation `.`
  TAU_TOK_PUNCT_DOT_DOT, ///< Punctuation `..`
  TAU_TOK_PUNCT_DOT_DOT_DOT, ///< Punctuation `...`
  TAU_TOK_PUNCT_DOT_LESS, ///< Punctuation `.<`
  TAU_TOK_PUNCT_QUESTION, ///< Punctuation `?`
  TAU_TOK_PUNCT_QUESTION_DOT, ///< Punctuation `?.`
  TAU_TOK_PUNCT_EQUAL, ///< Punctuation `=`
  TAU_TOK_PUNCT_EQUAL_EQUAL, ///< Punctuation `==`
  TAU_TOK_PUNCT_COMMA, ///< Punctuation `,`
  TAU_TOK_PUNCT_COLON, ///< Punctuation `:`
  TAU_TOK_PUNCT_SEMICOLON, ///< Punctuation `;`
  TAU_TOK_PUNCT_PAREN_LEFT, ///< Punctuation `(`
  TAU_TOK_PUNCT_PAREN_RIGHT, ///< Punctuation `)`
  TAU_TOK_PUNCT_BRACKET_LEFT, ///< Punctuation `[`
  TAU_TOK_PUNCT_BRACKET_RIGHT, ///< Punctuation `]`
  TAU_TOK_PUNCT_BRACKET_ANGLE_LEFT, ///< Punctuation `[<`
  TAU_TOK_PUNCT_BRACKET_ANGLE_RIGHT, ///< Punctuation `>]`
  TAU_TOK_PUNCT_BRACE_LEFT, ///< Punctuation `{`
  TAU_TOK_PUNCT_BRACE_RIGHT, ///< Punctuation `}`
  TAU_TOK_PUNCT_HASH, ///< Punctuation `#`

  TAU_TOK_NEWLINE, ///< Newline

  TAU_TOK_EOF, ///< End of file
} tau_token_kind_t;

/**
 * \brief Represents a token.
 */
typedef struct tau_token_t
{
  tau_token_kind_t kind; // Token kind.
  size_t pos; // Position of the token's first character in the source code.
} tau_token_t;

/**
 * \brief Queries a token's location in a source file.
 *
 * \details The location of a token is calculated lazily in order to
 * reduce memory usage during runtime. This function assumes that the
 * source code for the token is syntactically correct.
 *
 * \param[in] tok Pointer to the token whose location is to be retrieved.
 * \returns The token's location.
 */
tau_location_t tau_token_location(tau_token_t* tok);

/**
 * \brief Dumps the JSON representation of a token to the specified stream.
 * 
 * \param[in] stream The stream to write the JSON output to.
 * \param[in] tok Pointer to the token to be dumped.
 */
void tau_token_json_dump(FILE* stream, tau_token_t* tok);

/**
 * \brief Dumps the JSON representation of a vector of tokens to the specified
 * stream.
 * 
 * \param[in] stream The stream to write the JSON output to.
 * \param[in] vec The vector of tokens to be dumped.
 */
void tau_token_json_dump_vector(FILE* stream, tau_vector_t* vec);

/**
 * \brief Converts a token kind to its corresponding c-string representation.
 * 
 * \param[in] kind The token kind to be converted.
 * \returns The c-string representation of the token kind.
 */
const char* tau_token_kind_to_cstr(tau_token_kind_t kind);

/**
 * \brief Creates a string from a token.
 *
 * \param tok Pointer to the token.
 * \returns Pointer to the string.
 */
tau_string_t* tau_token_to_string(tau_token_t* tok);

/**
 * \brief Creates a string view of a token.
 *
 * \param tok Pointer to the token.
 * \returns The string view.
 */
tau_string_view_t tau_token_to_string_view(tau_token_t* tok);

/**
 * \brief Checks if a token is a literal token.
 * 
 * \param[in] tok The token to be checked.
 * \returns `true` if the token is a literal token, `false` otherwise.
 */
bool tau_token_is_literal(tau_token_t* tok);

/**
 * \brief Checks if a token is a keyword token.
 * 
 * \param[in] tok The token to be checked.
 * \returns `true` if the token is a keyword token, `false` otherwise.
 */
bool tau_token_is_keyword(tau_token_t* tok);

/**
 * \brief Checks if a token is a punctuation token.
 * 
 * \param[in] tok The token to be checked.
 * \returns `true` if the token is a punctuation token, `false` otherwise.
 */
bool tau_token_is_punctuation(tau_token_t* tok);

TAU_EXTERN_C_END

#endif
