/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/lexer/lexer.h"

#include <ctype.h>
#include <string.h>

#include "stages/lexer/location.h"
#include "stages/lexer/token/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

#define LEXER_MAX_BUFFER_SIZE 256

static const struct
{
  const char* const keyword;
  const token_kind_t kind;
} g_keyword_lookup_table[] = {
  { "is",       TOK_KW_IS       },
  { "as",       TOK_KW_AS       },
  { "sizeof",   TOK_KW_SIZEOF   },
  { "alignof",  TOK_KW_ALIGNOF  },
  { "use",      TOK_KW_USE      },
  { "in",       TOK_KW_IN       },
  { "pub",      TOK_KW_PUB      },
  { "extern",   TOK_KW_EXTERN   },
  { "fun",      TOK_KW_FUN      },
  { "struct",   TOK_KW_STRUCT   },
  { "union",    TOK_KW_UNION    },
  { "enum",     TOK_KW_ENUM     },
  { "mod",      TOK_KW_MOD      },
  { "if",       TOK_KW_IF       },
  { "then",     TOK_KW_THEN     },
  { "else",     TOK_KW_ELSE     },
  { "for",      TOK_KW_FOR      },
  { "while",    TOK_KW_WHILE    },
  { "do",       TOK_KW_DO       },
  { "loop",     TOK_KW_LOOP     },
  { "break",    TOK_KW_BREAK    },
  { "continue", TOK_KW_CONTINUE },
  { "return",   TOK_KW_RETURN   },
  { "defer",    TOK_KW_DEFER    },
  { "mut",      TOK_KW_MUT      },
  { "i8",       TOK_KW_I8       },
  { "i16",      TOK_KW_I16      },
  { "i32",      TOK_KW_I32      },
  { "i64",      TOK_KW_I64      },
  { "isize",    TOK_KW_ISIZE    },
  { "u8",       TOK_KW_U8       },
  { "u16",      TOK_KW_U16      },
  { "u32",      TOK_KW_U32      },
  { "u64",      TOK_KW_U64      },
  { "usize",    TOK_KW_USIZE    },
  { "f32",      TOK_KW_F32      },
  { "f64",      TOK_KW_F64      },
  { "c64",      TOK_KW_C64      },
  { "c128",     TOK_KW_C128     },
  { "char",     TOK_KW_CHAR     },
  { "bool",     TOK_KW_BOOL     },
  { "unit",     TOK_KW_UNIT     },
  { "type",     TOK_KW_TYPE     },
  { "undef",    TOK_KW_UNDEF    },
  { "true",     TOK_LIT_BOOL    },
  { "false",    TOK_LIT_BOOL    },
  { "null",     TOK_LIT_NULL    },
};

struct lexer_t
{
  const char* path; ///< Path to the source file.
  const char* src; ///< Contents of the source file.
  size_t pos; ///< Current position in the source file.
  vector_t* tokens; ///< Vector of tokens.
  error_bag_t* errors; ///< Associated error bag.
};

static bool lexer_is_kw_vec(const char* str, size_t len)
{
  if (len < 6)
    return false;

  if (strncmp(str, "vec", 3) != 0)
    return false;

  str += 3;

  if (!isdigit(*str))
    return false;

  while (isdigit(*str))
    str++;

  if (*str == '\0')
    return false;

  size_t suffix_len = 0;

  while (isalnum(str[suffix_len]))
    suffix_len++;

  switch (suffix_len)
  {
  case 2:
    if (strncmp("i8", str, suffix_len) == 0 ||
        strncmp("u8", str, suffix_len) == 0)
      return true;
  case 3:
    if (strncmp("i16", str, suffix_len) == 0 ||
        strncmp("u16", str, suffix_len) == 0 ||
        strncmp("i32", str, suffix_len) == 0 ||
        strncmp("u32", str, suffix_len) == 0 ||
        strncmp("i64", str, suffix_len) == 0 ||
        strncmp("u64", str, suffix_len) == 0 ||
        strncmp("f32", str, suffix_len) == 0 ||
        strncmp("f64", str, suffix_len) == 0)
      return true;
  default:
    return false;
  }
}

static bool lexer_is_kw_mat(const char* str, size_t len)
{
  if (len < 6)
    return false;

  if (strncmp(str, "mat", 3) != 0)
    return false;

  str += 3;

  if (!isdigit(*str))
    return false;

  while (isdigit(*str))
    str++;

  if (*str == '\0')
    return false;

  if (*str == 'x')
  {
    str++;

    if (!isdigit(*str))
      return false;

    while (isdigit(*str))
      str++;

    if (*str == '\0')
      return false;
  }

  size_t suffix_len = 0;

  while (isalnum(str[suffix_len]))
    suffix_len++;

  switch (suffix_len)
  {
  case 2:
    if (strncmp("i8", str, suffix_len) == 0 ||
        strncmp("u8", str, suffix_len) == 0)
      return true;
  case 3:
    if (strncmp("i16", str, suffix_len) == 0 ||
        strncmp("u16", str, suffix_len) == 0 ||
        strncmp("i32", str, suffix_len) == 0 ||
        strncmp("u32", str, suffix_len) == 0 ||
        strncmp("i64", str, suffix_len) == 0 ||
        strncmp("u64", str, suffix_len) == 0 ||
        strncmp("f32", str, suffix_len) == 0 ||
        strncmp("f64", str, suffix_len) == 0)
      return true;
  default:
    return false;
  }
}

lexer_t* lexer_init(void)
{
  lexer_t* lex = (lexer_t*)malloc(sizeof(lexer_t));

  lex->path = NULL;
  lex->src = NULL;
  lex->pos = 0;

  return lex;
}

void lexer_free(lexer_t* lex)
{
  free(lex);
}

token_t* lexer_token_init(lexer_t* lex, token_kind_t kind)
{
  return token_registry_token_init(lex->path, kind, lex->pos);
}

location_t lexer_location(lexer_t* lex)
{
  size_t row = 0;
  size_t col = 0;

  for (size_t i = 0; i < lex->pos && lex->src[i] != '\0'; i++, col++)
  {
    if (lex->src[i] == '\n')
    {
      row++;
      col = 0;
    }
  }

  location_t loc = {
    .path = lex->path,
    .src = lex->src,
    .ptr = lex->src + lex->pos,
    .row = row,
    .col = col,
    .len = 0
  };

  return loc;
}

bool lexer_is_space(lexer_t* lex)
{
  return isspace(lex->src[lex->pos]) != 0;
}

bool lexer_is_word_begin(lexer_t* lex)
{
  return isalpha(lex->src[lex->pos]) != 0 || lex->src[lex->pos] == '_';
}

bool lexer_is_word(lexer_t* lex)
{
  return isalnum(lex->src[lex->pos]) != 0 || lex->src[lex->pos] == '_';
}

bool lexer_is_decimal(lexer_t* lex)
{
  return isdigit(lex->src[lex->pos]) != 0;
}

bool lexer_is_hexadecimal(lexer_t* lex)
{
  return isxdigit(lex->src[lex->pos]) != 0;
}

bool lexer_is_octal(lexer_t* lex)
{
  return '0' <= lex->src[lex->pos] && '7' >= lex->src[lex->pos];
}

bool lexer_is_binary(lexer_t* lex)
{
  return lex->src[lex->pos] == '0' || lex->src[lex->pos] == '1';
}

bool lexer_is_punctuation(lexer_t* lex)
{
  return ispunct(lex->src[lex->pos]) != 0;
}

char lexer_current(lexer_t* lex)
{
  return lex->src[lex->pos];
}

char lexer_next(lexer_t* lex)
{
  if (lex->src[lex->pos] == '\0')
    return '\0';

  if (lex->src[lex->pos] == '\n')
  {
    token_t* tok = token_registry_token_init(lex->path, TOK_NEWLINE, lex->pos);
    vector_push(lex->tokens, tok);
  }

  return lex->src[lex->pos++];
}

char lexer_peek(lexer_t* lex)
{
  return lex->src[lex->pos] == '\0' ? '\0' : lex->src[lex->pos + 1];
}

bool lexer_consume(lexer_t* lex, char ch)
{
  if (lexer_current(lex) == ch)
  {
    lexer_next(lex);
    return true;
  }

  return false;
}

size_t lexer_skip(lexer_t* lex, bool(*pred)(lexer_t*))
{
  size_t begin = lex->pos;

  while (lex->src[lex->pos] != '\0' && pred(lex))
    lexer_next(lex);

  return lex->pos - begin;
}

void lexer_skip_n(lexer_t* lex, size_t n)
{
  while (lexer_current(lex) != '\0' && n-- > 0)
    lexer_next(lex);
}

size_t lexer_skip_integer_suffix(lexer_t* lex)
{
  size_t begin = lex->pos;

  location_t loc = lexer_location(lex);

  size_t len = lexer_skip(lex, lexer_is_word);

  switch (len)
  {
  case 0:
    break;
  case 2:
    if (strncmp("iz", lex->src + begin, len) == 0 ||
        strncmp("uz", lex->src + begin, len) == 0 ||
        strncmp("i8", lex->src + begin, len) == 0 ||
        strncmp("u8", lex->src + begin, len) == 0)
      break;
  case 3:
    if (strncmp("i16", lex->src + begin, len) == 0 ||
        strncmp("u16", lex->src + begin, len) == 0 ||
        strncmp("i32", lex->src + begin, len) == 0 ||
        strncmp("u32", lex->src + begin, len) == 0 ||
        strncmp("i64", lex->src + begin, len) == 0 ||
        strncmp("u64", lex->src + begin, len) == 0 ||
        strncmp("c64", lex->src + begin, len) == 0)
      break;
  case 4:
    if (strncmp("c128", lex->src + begin, len) == 0)
      break;
  default:
  {
    loc.len = len;

    error_bag_put_lexer_invalid_integer_suffix(lex->errors, loc);
  }
  }

  return len;
}

void lexer_skip_comment_line(lexer_t* lex)
{
  while (lexer_current(lex) != '\n' && lexer_current(lex) != '\0')
    lexer_next(lex);
}

void lexer_skip_comment_block(lexer_t* lex)
{
  while (!(lexer_consume(lex, '*') && lexer_consume(lex, '/')))
    lexer_next(lex);
}

token_t* lexer_read_word(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_ID);

  size_t len = lexer_skip(lex, lexer_is_word);

  if (len > LEXER_MAX_BUFFER_SIZE - 1)
  {
    location_t loc = lexer_location(lex);

    loc.len = len;

    error_bag_put_lexer_identifier_too_long(lex->errors, loc);
  }

  char buf[LEXER_MAX_BUFFER_SIZE];

  strncpy(buf, lex->src + tok->pos, len);

  buf[len] = '\0';

  if (lexer_is_kw_vec(buf, len))
  {
    tok->kind = TOK_KW_VEC;
  }
  else if (lexer_is_kw_mat(buf, len))
  {
    tok->kind = TOK_KW_MAT;
  }
  else
  {
    for (size_t i = 0; i < COUNTOF(g_keyword_lookup_table); ++i)
    {
      if (strcmp(g_keyword_lookup_table[i].keyword, buf) == 0)
      {
        tok->kind = g_keyword_lookup_table[i].kind;
        break;
      }
    }
  }

  return tok;
}

token_t* lexer_read_octal_integer(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT);

  lexer_skip_n(lex, 2);

  size_t len = 2 + lexer_skip(lex, lexer_is_octal);

  len += lexer_skip_integer_suffix(lex);

  if (len == 2 || lexer_is_word(lex))
  {
    location_t loc = lexer_location(lex);

    loc.len = len;

    error_bag_put_lexer_ill_formed_integer(lex->errors, loc);
  }

  return tok;
}

token_t* lexer_read_binary_integer(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT);

  lexer_skip_n(lex, 2);

  size_t len = 2 + lexer_skip(lex, lexer_is_binary);

  len += lexer_skip_integer_suffix(lex);

  if (len == 2 || lexer_is_word(lex))
  {
    location_t loc = lexer_location(lex);

    loc.len = len;

    error_bag_put_lexer_ill_formed_integer(lex->errors, loc);
  }

  return tok;
}

token_t* lexer_read_decimal_number(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT);

  size_t len = lexer_skip(lex, lexer_is_decimal);

  if (lexer_current(lex) == '.')
  {
    if (!isdigit(lexer_peek(lex)))
      return tok;

    lexer_next(lex);

    ++len;

    len += lexer_skip(lex, lexer_is_decimal);

    if (lexer_current(lex) == 'e' || lexer_current(lex) == 'E')
    {
      lexer_next(lex);

      ++len;

      if (lexer_current(lex) == '+' || lexer_current(lex) == '-')
      {
        lexer_next(lex);

        ++len;
      }

      len += lexer_skip(lex, lexer_is_decimal);
    }

    if (lexer_is_word(lex))
    {
      location_t loc = lexer_location(lex);

      loc.len = len;

      error_bag_put_lexer_ill_formed_float(lex->errors, loc);
    }

    tok->kind = TOK_LIT_FLT;

    return tok;
  }

  len += lexer_skip_integer_suffix(lex);

  if (lexer_is_word(lex))
  {
    location_t loc = lexer_location(lex);

    loc.len = len;

    error_bag_put_lexer_ill_formed_integer(lex->errors, loc);
  }

  return tok;
}

token_t* lexer_read_hexadecimal_integer(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT);

  lexer_skip_n(lex, 2);

  size_t len = 2 + lexer_skip(lex, lexer_is_hexadecimal);

  len += lexer_skip_integer_suffix(lex);

  if (len == 2 || lexer_is_word(lex))
  {
    location_t loc = lexer_location(lex);

    loc.len = len;

    error_bag_put_lexer_ill_formed_integer(lex->errors, loc);
  }

  return tok;
}

token_t* lexer_read_number(lexer_t* lex)
{
  if (lexer_current(lex) == '0')
  {
    char ch = lexer_peek(lex);

    if (ch == 'x' || ch == 'X')
      return lexer_read_hexadecimal_integer(lex);

    if (ch == 'o' || ch == 'O')
      return lexer_read_hexadecimal_integer(lex);

    if (ch == 'b' || ch == 'B')
      return lexer_read_hexadecimal_integer(lex);
  }

  return lexer_read_decimal_number(lex);
}

token_t* lexer_read_string(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_STR);

  location_t begin_loc = lexer_location(lex);

  lexer_next(lex);

  char ch;
  while ((ch = lexer_next(lex)) != '"' && ch != '\0')
  {
    if (ch == '\\')
    {
      ch = lexer_next(lex);

      switch (ch)
      {
      case '\\': // backslash
      case 'b': // backspace
      case 'f': // form feed
      case 'n': // line feed
      case 'r': // carriage return
      case 't': // horizontal tab
      case '\'': // single quote
      case '"': // double quote
        break;

      case 'x':
      case 'X': // arbitrary hexadecimal bytes
        if (!isxdigit(lexer_current(lex)))
        {
          location_t loc = lexer_location(lex);

          loc.ptr -= 2;
          loc.len = 2;

          error_bag_put_lexer_missing_hex_digits(lex->errors, loc);
        }

        lexer_skip(lex, lexer_is_hexadecimal);
        break;

      default:
      {
        location_t loc = lexer_location(lex);

        loc.ptr -= 2;
        loc.len = 2;

        error_bag_put_lexer_unknown_escape_sequence(lex->errors, loc);
      }
      }
    }
  }

  if (ch != '"')
  {
    begin_loc.len = 1;

    error_bag_put_lexer_missing_double_quote(lex->errors, begin_loc);
  }

  return tok;
}

token_t* lexer_read_character(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_CHAR);

  location_t begin_loc = lexer_location(lex);

  lexer_next(lex);

  size_t len = 0;

  if (lexer_current(lex) == '\'')
  {
    begin_loc.len = 2;

    error_bag_put_lexer_empty_character(lex->errors, begin_loc);

    return tok;
  }

  if (lexer_next(lex) == '\\')
  {
    ++len;

    switch (lexer_next(lex))
    {
    case '\\': // backslash
    case 'b': // backspace
    case 'f': // form feed
    case 'n': // line feed
    case 'r': // carriage return
    case 't': // horizontal tab
    case '\'': // single quote
    case '"': // double quote
      ++len;
      break;

    case 'x':
    case 'X': // arbitrary hexadecimal byte
      ++len;

      if (!isxdigit(lexer_current(lex)))
      {
        location_t loc = lexer_location(lex);

        loc.ptr -= 2;
        loc.len = 2;

        error_bag_put_lexer_missing_hex_digits(lex->errors, loc);
      }

      len += lexer_skip(lex, lexer_is_hexadecimal);

      if (len > 4)
      {
        location_t loc = lexer_location(lex);

        loc.ptr -= len;
        loc.len = len;

        error_bag_put_lexer_too_many_hex_digits(lex->errors, loc);
      }
      break;

    default:
    {
      location_t loc = lexer_location(lex);

      loc.ptr -= 2;
      loc.len = 2;

      error_bag_put_lexer_unknown_escape_sequence(lex->errors, loc);
    }

    }
  }
  else
    ++len;

  if (lexer_current(lex) != '\'')
  {
    begin_loc.len = 1;

    error_bag_put_lexer_missing_double_quote(lex->errors, begin_loc);
  }
  else
  {
    lexer_next(lex);
  }

  return tok;
}

token_t* lexer_read_punctuation(lexer_t* lex)
{
  token_kind_t kind = TOK_UNKNOWN;

  size_t pos = lex->pos;

  if (lexer_consume(lex, '+'))
    if (lexer_consume(lex, '+'))
      kind = TOK_PUNCT_PLUS_PLUS;
    else if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_PLUS_EQUAL;
    else
      kind = TOK_PUNCT_PLUS;
  else if (lexer_consume(lex, '-'))
    if (lexer_consume(lex, '-'))
      kind = TOK_PUNCT_MINUS_MINUS;
    else if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_MINUS_EQUAL;
    else if (lexer_consume(lex, '>'))
      kind = TOK_PUNCT_MINUS_GREATER;
    else
      kind = TOK_PUNCT_MINUS;
  else if (lexer_consume(lex, '*'))
    if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_ASTERISK_EQUAL;
    else
      kind = TOK_PUNCT_ASTERISK;
  else if (lexer_consume(lex, '/'))
  {
    if (lexer_consume(lex, '/'))
    {
      lexer_skip_comment_line(lex);
      return lexer_read_next(lex);
    }

    if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_SLASH_EQUAL;
    else if (lexer_consume(lex, '*'))
    {
      lexer_skip_comment_block(lex);
      return lexer_read_next(lex);
    }
    else
      kind = TOK_PUNCT_SLASH;
  }
  else if (lexer_consume(lex, '%'))
    if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_PERCENT_EQUAL;
    else
      kind = TOK_PUNCT_PERCENT;
  else if (lexer_consume(lex, '&'))
    if (lexer_consume(lex, '&'))
      kind = TOK_PUNCT_AMPERSAND_AMPERSAND;
    else if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_AMPERSAND_EQUAL;
    else
      kind = TOK_PUNCT_AMPERSAND;
  else if (lexer_consume(lex, '|'))
    if (lexer_consume(lex, '|'))
      kind = TOK_PUNCT_BAR_BAR;
    else if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_BAR_EQUAL;
    else
      kind = TOK_PUNCT_BAR;
  else if (lexer_consume(lex, '^'))
    if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_HAT_EQUAL;
    else
      kind = TOK_PUNCT_HAT;
  else if (lexer_consume(lex, '~'))
    kind = TOK_PUNCT_TILDE;
  else if (lexer_consume(lex, '<'))
    if (lexer_consume(lex, '<'))
      if (lexer_consume(lex, '='))
        kind = TOK_PUNCT_LESS_LESS_EQUAL;
      else
        kind = TOK_PUNCT_LESS_LESS;
    else if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_LESS_EQUAL;
    else
      kind = TOK_PUNCT_LESS;
  else if (lexer_consume(lex, '>'))
    if (lexer_consume(lex, '>'))
      if (lexer_consume(lex, '='))
        kind = TOK_PUNCT_GREATER_GREATER_EQUAL;
      else
        kind = TOK_PUNCT_GREATER_GREATER;
    else if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_GREATER_EQUAL;
    else if (lexer_consume(lex, ']'))
      kind = TOK_PUNCT_BRACKET_ANGLE_RIGHT;
    else
      kind = TOK_PUNCT_GREATER;
  else if (lexer_consume(lex, '!'))
    if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_BANG_EQUAL;
    else
      kind = TOK_PUNCT_BANG;
  else if (lexer_consume(lex, '.'))
    if (lexer_consume(lex, '.'))
      if (lexer_consume(lex, '.'))
        kind = TOK_PUNCT_DOT_DOT_DOT;
      else
        kind = TOK_PUNCT_DOT_DOT;
    else if (lexer_consume(lex, '<'))
      kind = TOK_PUNCT_DOT_LESS;
    else
      kind = TOK_PUNCT_DOT;
  else if (lexer_consume(lex, '?'))
    if (lexer_consume(lex, '.'))
      kind = TOK_PUNCT_QUESTION_DOT;
    else
      kind = TOK_PUNCT_QUESTION;
  else if (lexer_consume(lex, '='))
    if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_EQUAL_EQUAL;
    else
      kind = TOK_PUNCT_EQUAL;
  else if (lexer_consume(lex, ','))
    kind = TOK_PUNCT_COMMA;
  else if (lexer_consume(lex, ':'))
    kind = TOK_PUNCT_COLON;
  else if (lexer_consume(lex, ';'))
    kind = TOK_PUNCT_SEMICOLON;
  else if (lexer_consume(lex, '('))
    kind = TOK_PUNCT_PAREN_LEFT;
  else if (lexer_consume(lex, ')'))
    kind = TOK_PUNCT_PAREN_RIGHT;
  else if (lexer_consume(lex, '['))
    if (lexer_consume(lex, '<'))
      kind = TOK_PUNCT_BRACKET_ANGLE_LEFT;
    else
      kind = TOK_PUNCT_BRACKET_LEFT;
  else if (lexer_consume(lex, ']'))
    kind = TOK_PUNCT_BRACKET_RIGHT;
  else if (lexer_consume(lex, '{'))
    kind = TOK_PUNCT_BRACE_LEFT;
  else if (lexer_consume(lex, '}'))
    kind = TOK_PUNCT_BRACE_RIGHT;
  else if (lexer_consume(lex, '#'))
    kind = TOK_PUNCT_HASH;
  else
  {
    location_t loc = lexer_location(lex);

    loc.len = 1;

    error_bag_put_lexer_unexpected_character(lex->errors, loc);
  }

  return token_registry_token_init(lex->path, kind, pos);
}

token_t* lexer_read_next(lexer_t* lex)
{
  lexer_skip(lex, lexer_is_space);

  if (lexer_is_word_begin(lex))
    return lexer_read_word(lex);

  if (lexer_is_decimal(lex))
    return lexer_read_number(lex);

  if (lexer_current(lex) == '"')
    return lexer_read_string(lex);

  if (lexer_current(lex) == '\'')
    return lexer_read_character(lex);

  if (lexer_is_punctuation(lex))
    return lexer_read_punctuation(lex);

  if (lexer_current(lex) == '\0')
    return lexer_token_init(lex, TOK_EOF);

  location_t loc = lexer_location(lex);

  loc.len = 1;

  error_bag_put_lexer_unexpected_character(lex->errors, loc);

  return NULL;
}

void lexer_lex(lexer_t* lex, const char* path, const char* src, vector_t* tokens, error_bag_t* errors)
{
  lex->path = path;
  lex->src = src;
  lex->pos = 0;

  lex->tokens = tokens;
  lex->errors = errors;

  token_registry_register_file(path, src);

  while (vector_empty(lex->tokens) || ((token_t*)vector_back(lex->tokens))->kind != TOK_EOF)
  {
    vector_push(lex->tokens, lexer_read_next(lex));

    if (error_bag_full(lex->errors))
      return;
  }
}
