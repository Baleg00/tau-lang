/**
 * \file
 * 
 * Tokens are atomic words of the language such as identifiers,
 * literals, keywords and punctuations.
*/

#ifndef TAU_TOKEN_H
#define TAU_TOKEN_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "list.h"
#include "location.h"

/** Indicates the category of a token. */
typedef enum token_flag_e
{
  TOK_FLAG_UNKNOWN = 0x000000, // Unknown
  TOK_FLAG_ID      = 0x010000, // Identifier
  TOK_FLAG_LIT     = 0x020000, // Literal
  TOK_FLAG_KW      = 0x040000, // Keyword
  TOK_FLAG_PUNCT   = 0x080000, // Punctuation
  TOK_FLAG_EOF     = 0x100000 // End of file
} token_flag_t;

/** Indicates the kind of a token. */
typedef enum token_kind_e
{
  TOK_UNKNOWN = TOK_FLAG_UNKNOWN, // Unknown token

  TOK_ID = TOK_FLAG_ID, // Identifier

  TOK_LIT_INT = TOK_FLAG_LIT, // Literal integer (e.g. 123)
  TOK_LIT_FLT, // Literal float (e.g. 1.23)
  TOK_LIT_STR, // Literal string (e.g. "abc")
  TOK_LIT_CHAR, // Literal character (e.g. 'a')
  TOK_LIT_BOOL, // Literal boolean
  TOK_LIT_NULL, // Literal null

  TOK_KW_IS = TOK_FLAG_KW, // Keyword `is`
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

  TOK_PUNCT_PLUS = TOK_FLAG_PUNCT, // Punctuation `+`
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

  TOK_EOF = TOK_FLAG_EOF // End of file
} token_kind_t;

/** Utility macro which expands to fields that all tokens must have. */
#define TOKEN_HEADER\
  struct\
  {\
    token_kind_t kind;\
    location_t* loc;\
  }

/** Base type for all tokens. */
typedef struct token_s
{
  TOKEN_HEADER;
} token_t;

/** Token type for identifiers. */
typedef struct token_id_s
{
  TOKEN_HEADER;
  char* value;
} token_id_t;

/** Token type for integer literals. */
typedef struct token_lit_int_s
{
  TOKEN_HEADER;
  uint64_t value;
} token_lit_int_t;

/** Token type for floating-point literals. */
typedef struct token_lit_flt_s
{
  TOKEN_HEADER;
  long double value;
} token_lit_flt_t;

/** Token type for string literals. */
typedef struct token_lit_str_s
{
  TOKEN_HEADER;
  char* value;
} token_lit_str_t;

/** Token type for character literals. */
typedef struct token_lit_char_s
{
  TOKEN_HEADER;
  char value;
} token_lit_char_t;

/** Token type for boolean literals. */
typedef struct token_lit_bool_s
{
  TOKEN_HEADER;
  bool value;
} token_lit_bool_t;

#undef TOKEN_HEADER

/**
 * \brief Initializes a new token.
 * 
 * \param[in] kind Token kind.
 * \param[in] loc Token location.
 * \returns New token.
*/
token_t* token_init(token_kind_t kind, location_t* loc);

/**
 * \brief Initializes a new identifier token.
 * 
 * \param[in] loc Token location.
 * \param[in] value Identifier string.
 * \returns New identifier token.
*/
token_id_t* token_id_init(location_t* loc, char* value);

/**
 * \brief Initializes a new integer literal token.
 * 
 * \param[in] loc Token location.
 * \param[in] value Integer value.
 * \returns New integer literal token.
*/
token_lit_int_t* token_lit_int_init(location_t* loc, uint64_t value);

/**
 * \brief Initializes a new floating-point literal token.
 * 
 * \param[in] loc Token location.
 * \param[in] value Floating-point value.
 * \returns New floating-point literal token.
*/
token_lit_flt_t* token_lit_flt_init(location_t* loc, long double value);

/**
 * \brief Initializes a new string literal token.
 * 
 * \param[in] loc Token location.
 * \param[in] value String value.
 * \returns New string literal token.
*/
token_lit_str_t* token_lit_str_init(location_t* loc, char* value);

/**
 * \brief Initializes a new character literal token.
 * 
 * \param[in] loc Token location.
 * \param[in] value Character value.
 * \returns New character literal token.
*/
token_lit_char_t* token_lit_char_init(location_t* loc, char value);

/**
 * \brief Destroys a token.
 * 
 * \param[in] tok Token to be destroyed.
*/
void token_free(token_t* tok);

/**
 * \brief Writes a JSON dump of a list of tokens into a stream.
 * 
 * \param[in] stream Stream to be written to.
 * \param[in] list List of tokens to be dumped.
*/
void token_list_json_dump(FILE* stream, list_t* list);

/**
 * \brief Returns a string representation of a token kind.
 * 
 * \param[in] kind Token kind.
 * \returns String representation.
*/
const char* token_kind_to_string(token_kind_t kind);

/**
 * \brief Checks if a token is a literal.
 * 
 * \param[in] tok Token to be checked.
 * \returns True if token is a literal, false otherwise.
*/
bool token_is_literal(token_t* tok);

/**
 * \brief Checks if a token is a literal integer.
 * 
 * \param[in] tok Token to be checked.
 * \returns True if token is a literal integer, false otherwise.
*/
bool token_is_literal_integer(token_t* tok);

/**
 * \brief Checks if a token is a literal float.
 * 
 * \param[in] tok Token to be checked.
 * \returns True if token is a literal float, false otherwise.
*/
bool token_is_literal_float(token_t* tok);

/**
 * \brief Checks if a token is a keyword.
 * 
 * \param[in] tok Token to be checked.
 * \returns True if token is a keyword, false otherwise.
*/
bool token_is_keyword(token_t* tok);

/**
 * \brief Checks if a token is a punctuation.
 * 
 * \param[in] tok Token to be checked.
 * \returns True if token is a punctuation, false otherwise.
*/
bool token_is_punctuation(token_t* tok);

#endif
