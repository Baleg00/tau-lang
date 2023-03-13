#ifndef TAU_TOKEN_H
#define TAU_TOKEN_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "list.h"
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
  TOK_KW_FUN, // keyword "fun"
  TOK_KW_GEN, // keyword "gen"
  TOK_KW_STRUCT, // keyword "struct"
  TOK_KW_UNION, // keyword "union"
  TOK_KW_ENUM, // keyword "enum"
  TOK_KW_MOD, // keyword "mod"
  TOK_KW_USE, // keyword "use"
  TOK_KW_IF, // keyword "if"
  TOK_KW_THEN, // keyword "then"
  TOK_KW_ELSE, // keyword "else"
  TOK_KW_FOR, // keyword "for"
  TOK_KW_WHILE, // keyword "while"
  TOK_KW_DO, // keyword "do"
  TOK_KW_BREAK, // keyword "break"
  TOK_KW_CONTINUE, // keyword "continue"
  TOK_KW_RETURN, // keyword "return"
  TOK_KW_YIELD, // keyword "yield"
  TOK_KW_MUT, // keyword "mut"
  TOK_KW_CONST, // keyword "const"
  TOK_KW_TYPE, // keyword "type"
  TOK_KW_SELF, // keyword "Self"
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
  TOK_PUNCT_SEMICOLON, // punctuation ";"
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
  location_t* loc;

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
    } lit_str, lit_char, id;
  };
} token_t;

token_t* token_init(void);
void token_free(token_t* tok);

void token_list_json_dump(FILE* stream, list_t* list);

const char* token_kind_to_string(token_kind_t kind);

bool token_is_literal(token_t* tok);
bool token_is_literal_integer(token_t* tok);
bool token_is_literal_float(token_t* tok);
bool token_is_keyword(token_t* tok);
bool token_is_punctuation(token_t* tok);

#endif
