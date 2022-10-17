#ifndef TAU_TOKEN_H
#define TAU_TOKEN_H

#include <stdbool.h>

#include "location.h"

typedef enum token_kind_e
{
  TOK_UNKNOWN = 0, // unknown token

  TOK_ID, // identifier

  TOK_LIT_INT_DEC, // literal integer decimal
  TOK_LIT_INT_HEX, // literal integer hexadecimal
  TOK_LIT_INT_OCT, // literal integer octal
  TOK_LIT_INT_BIN, // literal integer binary
  TOK_LIT_FLT_DEC, // literal float decimal
  TOK_LIT_FLT_HEX, // literal float hexadecimal
  TOK_LIT_STR, // literal string
  TOK_LIT_CHAR, // literal character
  TOK_LIT_BOOL_TRUE, // literal boolean true
  TOK_LIT_BOOL_FALSE, // literal boolean false
  TOK_LIT_NULL, // literal null

  TOK_KW_IS, // keyword "is"
  TOK_KW_AS, // keyword "as"
  TOK_KW_SIZEOF, // keyword "sizeof"
  TOK_KW_ALIGNOF, // keyword "alignof"
  TOK_KW_TYPEOF, // keyword "typeof"
  TOK_KW_IN, // keyword "in"
  TOK_KW_VAR, // keyword "var"
  TOK_KW_FUN, // keyword "fun"
  TOK_KW_GEN, // keyword "gen"
  TOK_KW_STRUCT, // keyword "struct"
  TOK_KW_UNION, // keyword "union"
  TOK_KW_ENUM, // keyword "enum"
  TOK_KW_MOD, // keyword "mod"
  TOK_KW_USE, // keyword "use"
  TOK_KW_FROM, // keyword "from"
  TOK_KW_IF, // keyword "if"
  TOK_KW_THEN, // keyword "then"
  TOK_KW_ELSE, // keyword "else"
  TOK_KW_ELIF, // keyword "elif"
  TOK_KW_FOR, // keyword "for"
  TOK_KW_WHILE, // keyword "while"
  TOK_KW_WHEN, // keyword "when"
  TOK_KW_DO, // keyword "do"
  TOK_KW_BREAK, // keyword "break"
  TOK_KW_CONTINUE, // keyword "continue"
  TOK_KW_RETURN, // keyword "return"
  TOK_KW_YIELD, // keyword "yield"
  TOK_KW_PUB, // keyword "pub"
  TOK_KW_MUT, // keyword "mut"
  TOK_KW_CONST, // keyword "const"
  TOK_KW_STATIC, // keyword "static"
  TOK_KW_I8, // keyword "i8"
  TOK_KW_I16, // keyword "i16"
  TOK_KW_I32, // keyword "i32"
  TOK_KW_I64, // keyword "i64"
  TOK_KW_ISIZE, // keyword "isize"
  TOK_KW_U8, // keyword "u8"
  TOK_KW_U16, // keyword "u16"
  TOK_KW_U32, // keyword "u32"
  TOK_KW_U64, // keyword "u64"
  TOK_KW_USIZE, // keyword "usize"
  TOK_KW_F32, // keyword "f32"
  TOK_KW_F64, // keyword "f64"
  TOK_KW_BOOL, // keyword "bool"
  TOK_KW_UNIT, // keyword "unit"

  TOK_PUNCT_PLUS, // punctuation "+"
  TOK_PUNCT_PLUS_PLUS, // punctuation "++"
  TOK_PUNCT_PLUS_EQUAL, // punctuation "+="
  TOK_PUNCT_MINUS, // punctuation "-"
  TOK_PUNCT_MINUS_MINUS, // punctuation "--"
  TOK_PUNCT_MINUS_EQUAL, // punctuation "-="
  TOK_PUNCT_ASTERISK, // punctuation "*"
  TOK_PUNCT_ASTERISK_EQUAL, // punctuation "*="
  TOK_PUNCT_ASTERISK_DOT, // punctuation "*."
  TOK_PUNCT_SLASH, // punctuation "/"
  TOK_PUNCT_SLASH_EQUAL, // punctuation "/="
  TOK_PUNCT_PERCENT, // punctuation "%"
  TOK_PUNCT_PERCENT_EQUAL, // punctuation "%="
  TOK_PUNCT_AMPERSAND, // punctuation "&"
  TOK_PUNCT_AMPERSAND_AMPERSAND, // punctuation "&&"
  TOK_PUNCT_AMPERSAND_EQUAL, // punctuation "&="
  TOK_PUNCT_BAR, // punctuation "|"
  TOK_PUNCT_BAR_BAR, // punctuation "||"
  TOK_PUNCT_BAR_EQUAL, // punctuation "|="
  TOK_PUNCT_HAT, // punctuation "^"
  TOK_PUNCT_HAT_EQUAL, // punctuation "^="
  TOK_PUNCT_TILDE, // punctuation "~"
  TOK_PUNCT_LESS, // punctuation "<"
  TOK_PUNCT_LESS_LESS, // punctuation "<<"
  TOK_PUNCT_LESS_LESS_EQUAL, // punctuation "<<="
  TOK_PUNCT_LESS_EQUAL, // punctuation "<="
  TOK_PUNCT_GREATER, // punctuation ">"
  TOK_PUNCT_GREATER_GREATER, // punctuation ">>"
  TOK_PUNCT_GREATER_GREATER_EQUAL, // punctuation ">>="
  TOK_PUNCT_GREATER_EQUAL, // punctuation ">="
  TOK_PUNCT_BANG, // punctuation "!"
  TOK_PUNCT_BANG_EQUAL, // punctuation "!="
  TOK_PUNCT_DOT, // punctuation "."
  TOK_PUNCT_DOT_DOT, // punctuation ".."
  TOK_PUNCT_DOT_DOT_DOT, // punctuation "..."
  TOK_PUNCT_QUESTION, // punctuation "?"
  TOK_PUNCT_QUESTION_DOT, // punctuation "?."
  TOK_PUNCT_EQUAL, // punctuation "="
  TOK_PUNCT_EQUAL_EQUAL, // punctuation "=="
  TOK_PUNCT_COMMA, // punctuation ","
  TOK_PUNCT_COLON, // punctuation ":"
  TOK_PUNCT_PAREN_LEFT, // punctuation "("
  TOK_PUNCT_PAREN_RIGHT, // punctuation ")"
  TOK_PUNCT_BRACKET_LEFT, // punctuation "["
  TOK_PUNCT_BRACKET_RIGHT, // punctuation "]"
  TOK_PUNCT_BRACE_LEFT, // punctuation "{"
  TOK_PUNCT_BRACE_RIGHT, // punctuation "}"

  TOK_EOF = -1 // end of file
} token_kind_t;

typedef struct token_s
{
  token_kind_t kind;
  location_t loc;

  union
  {
    struct {
      uint64_t value;
    } lit_int;

    struct {
      double value;
    } lit_flt;

    struct {
      char* value;
      size_t len;
    } lit_str, lit_char, id;
  };
  
  struct token_s *prev, *next;
} token_t;

/// Doubly-linked list of tokens.
typedef struct token_list_s
{
  token_t *root, *tail;
  size_t len;
} token_list_t;

/// Initialize a new token with a given token kind.
/// @param[in] kind The kind of the new token.
/// @return Pointer to the new token.
token_t* token_init(token_kind_t kind);

/// Free a token previously returned by token_init.
/// @param tok The token to be freed.
void token_free(token_t* tok);

/// Return the kind of the token.
/// @param tok The token.
/// @return The token kind.
token_kind_t token_kind(token_t* tok);

/// Get the next token in the list.
/// @param tok The token.
/// @return Pointer to the next token, or NULL if there isn't a next token.
token_t* token_next(token_t* tok);

/// Get the previous token in the list.
/// @param tok The token.
/// @return Pointer to the previous token, or NULL if there isn't a previous token.
token_t* token_prev(token_t* tok);

/// Get the location information of the token.
/// @param tok The token.
/// @return The location information.
location_t token_location(token_t* tok);

/// Initialize a new token list.
/// @return Pointer to the new token list.
token_list_t* token_list_init(void);

/// Free a token list previously returned by token_list_init.
/// @param list The token list to be freed.
void token_list_free(token_list_t* list);

/// Insert a token onto the end of the list.
/// @param list The list onto which the token will be pushed. 
/// @param tok The token to be pushed onto the list.
void token_list_push(token_list_t* list, token_t* tok);

/// Remove and return the last token from the list.
/// @param list The list from which the last token will be removed.
/// @return The removed token or NULL if the list is empty.
token_t* token_list_pop(token_list_t* list);

/// Return the last token from the list.
/// @param list The list whose last token will be returned.
/// @return The last token or NULL if the list is empty.
token_t* token_list_top(token_list_t* list);

/// Return the size of the token list.
/// @param list The token list.
/// @return The number of tokens in the list.
size_t token_list_size(token_list_t* list);

/// Determine whether the list is empty.
/// @param list The list to be checked.
/// @return True if the list is empty, false otherwise.
bool token_list_empty(token_list_t* list);

/// Convert a token kind to a matching string.
/// @param kind The kind to be converted.
/// @return The matching string.
const char* token_kind_to_string(token_kind_t kind);

/// Determine whether a token kind is a literal kind.
/// @param kind The kind to be tested.
/// @return True if the token kind is a literal kind, false otherwise.
bool token_is_literal(token_kind_t kind);

/// Determine whether a token kind is a literal integer kind.
/// @param kind The kind to be tested.
/// @return True if the token kind is a literal integer kind, false otherwise.
bool token_is_literal_integer(token_kind_t kind);

/// Determine whether a token kind is a literal float kind.
/// @param kind The kind to be tested.
/// @return True if the token kind is a literal float kind, false otherwise.
bool token_is_literal_float(token_kind_t kind);

/// Determine whether a token kind is a keyword kind.
/// @param kind The kind to be tested.
/// @return True if the token kind is a keyword kind, false otherwise.
bool token_is_keyword(token_kind_t kind);

/// Determine whether a token kind is a punctuation kind.
/// @param kind The kind to be tested.
/// @return True if the token kind is a punctuation kind, false otherwise.
bool token_is_punctuation(token_kind_t kind);

#endif
