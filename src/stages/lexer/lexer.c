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

#define LEXER_MAX_WORD_SIZE 256

static const struct
{
  const char* const keyword;
  const tau_token_kind_t kind;
} g_keyword_lookup_table[] = {
  { "is",       TAU_TOK_KW_IS       },
  { "as",       TAU_TOK_KW_AS       },
  { "sizeof",   TAU_TOK_KW_SIZEOF   },
  { "alignof",  TAU_TOK_KW_ALIGNOF  },
  { "use",      TAU_TOK_KW_USE      },
  { "in",       TAU_TOK_KW_IN       },
  { "pub",      TAU_TOK_KW_PUB      },
  { "extern",   TAU_TOK_KW_EXTERN   },
  { "fun",      TAU_TOK_KW_FUN      },
  { "struct",   TAU_TOK_KW_STRUCT   },
  { "union",    TAU_TOK_KW_UNION    },
  { "enum",     TAU_TOK_KW_ENUM     },
  { "mod",      TAU_TOK_KW_MOD      },
  { "if",       TAU_TOK_KW_IF       },
  { "then",     TAU_TOK_KW_THEN     },
  { "else",     TAU_TOK_KW_ELSE     },
  { "for",      TAU_TOK_KW_FOR      },
  { "while",    TAU_TOK_KW_WHILE    },
  { "do",       TAU_TOK_KW_DO       },
  { "loop",     TAU_TOK_KW_LOOP     },
  { "break",    TAU_TOK_KW_BREAK    },
  { "continue", TAU_TOK_KW_CONTINUE },
  { "return",   TAU_TOK_KW_RETURN   },
  { "defer",    TAU_TOK_KW_DEFER    },
  { "mut",      TAU_TOK_KW_MUT      },
  { "i8",       TAU_TOK_KW_I8       },
  { "i16",      TAU_TOK_KW_I16      },
  { "i32",      TAU_TOK_KW_I32      },
  { "i64",      TAU_TOK_KW_I64      },
  { "isize",    TAU_TOK_KW_ISIZE    },
  { "u8",       TAU_TOK_KW_U8       },
  { "u16",      TAU_TOK_KW_U16      },
  { "u32",      TAU_TOK_KW_U32      },
  { "u64",      TAU_TOK_KW_U64      },
  { "usize",    TAU_TOK_KW_USIZE    },
  { "f32",      TAU_TOK_KW_F32      },
  { "f64",      TAU_TOK_KW_F64      },
  { "c64",      TAU_TOK_KW_C64      },
  { "c128",     TAU_TOK_KW_C128     },
  { "char",     TAU_TOK_KW_CHAR     },
  { "bool",     TAU_TOK_KW_BOOL     },
  { "unit",     TAU_TOK_KW_UNIT     },
  { "type",     TAU_TOK_KW_TYPE     },
  { "undef",    TAU_TOK_KW_UNDEF    },
  { "true",     TAU_TOK_LIT_BOOL    },
  { "false",    TAU_TOK_LIT_BOOL    },
  { "null",     TAU_TOK_LIT_NULL    },
};

struct tau_lexer_t
{
  const char* path; ///< Path to the source file.
  const char* src; ///< Contents of the source file.
  size_t pos; ///< Current position in the source file.
  tau_vector_t* tokens; ///< Vector of tokens.
  tau_error_bag_t* errors; ///< Associated error bag.
};

static bool tau_lexer_is_kw_vec(const char* str, size_t len)
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

static bool tau_lexer_is_kw_mat(const char* str, size_t len)
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

tau_lexer_t* tau_lexer_init(void)
{
  tau_lexer_t* lex = (tau_lexer_t*)malloc(sizeof(tau_lexer_t));

  lex->path = NULL;
  lex->src = NULL;
  lex->pos = 0;

  return lex;
}

void tau_lexer_free(tau_lexer_t* lex)
{
  free(lex);
}

tau_token_t* tau_lexer_token_init(tau_lexer_t* lex, tau_token_kind_t kind)
{
  return tau_token_registry_token_init(lex->path, kind, lex->pos);
}

tau_location_t tau_lexer_location(tau_lexer_t* lex)
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

  tau_location_t loc = {
    .path = lex->path,
    .src = lex->src,
    .ptr = lex->src + lex->pos,
    .row = row,
    .col = col,
    .len = 0
  };

  return loc;
}

bool tau_lexer_is_space(tau_lexer_t* lex)
{
  return isspace(lex->src[lex->pos]) != 0;
}

bool tau_lexer_is_word_begin(tau_lexer_t* lex)
{
  return isalpha(lex->src[lex->pos]) != 0 || lex->src[lex->pos] == '_';
}

bool tau_lexer_is_word(tau_lexer_t* lex)
{
  return isalnum(lex->src[lex->pos]) != 0 || lex->src[lex->pos] == '_';
}

bool tau_lexer_is_decimal(tau_lexer_t* lex)
{
  return isdigit(lex->src[lex->pos]) != 0;
}

bool tau_lexer_is_hexadecimal(tau_lexer_t* lex)
{
  return isxdigit(lex->src[lex->pos]) != 0;
}

bool tau_lexer_is_octal(tau_lexer_t* lex)
{
  return '0' <= lex->src[lex->pos] && '7' >= lex->src[lex->pos];
}

bool tau_lexer_is_binary(tau_lexer_t* lex)
{
  return lex->src[lex->pos] == '0' || lex->src[lex->pos] == '1';
}

bool tau_lexer_is_punctuation(tau_lexer_t* lex)
{
  return ispunct(lex->src[lex->pos]) != 0;
}

char tau_lexer_current(tau_lexer_t* lex)
{
  return lex->src[lex->pos];
}

char tau_lexer_next(tau_lexer_t* lex)
{
  if (lex->src[lex->pos] == '\0')
    return '\0';

  if (lex->src[lex->pos] == '\n')
  {
    tau_token_t* tok = tau_token_registry_token_init(lex->path, TAU_TOK_NEWLINE, lex->pos);
    tau_vector_push(lex->tokens, tok);
  }

  return lex->src[lex->pos++];
}

char tau_lexer_peek(tau_lexer_t* lex)
{
  return lex->src[lex->pos] == '\0' ? '\0' : lex->src[lex->pos + 1];
}

bool tau_lexer_consume(tau_lexer_t* lex, char ch)
{
  if (tau_lexer_current(lex) == ch)
  {
    tau_lexer_next(lex);
    return true;
  }

  return false;
}

size_t tau_lexer_skip(tau_lexer_t* lex, bool(*pred)(tau_lexer_t*))
{
  size_t begin = lex->pos;

  while (lex->src[lex->pos] != '\0' && pred(lex))
    tau_lexer_next(lex);

  return lex->pos - begin;
}

void tau_lexer_skip_n(tau_lexer_t* lex, size_t n)
{
  while (tau_lexer_current(lex) != '\0' && n-- > 0)
    tau_lexer_next(lex);
}

size_t tau_lexer_skip_integer_suffix(tau_lexer_t* lex)
{
  size_t begin = lex->pos;

  tau_location_t loc = tau_lexer_location(lex);

  size_t len = tau_lexer_skip(lex, tau_lexer_is_word);

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

    tau_error_bag_put_lexer_invalid_integer_suffix(lex->errors, loc);
  }
  }

  return len;
}

void tau_lexer_skip_comment_line(tau_lexer_t* lex)
{
  while (tau_lexer_current(lex) != '\n' && tau_lexer_current(lex) != '\0')
    tau_lexer_next(lex);
}

void tau_lexer_skip_comment_block(tau_lexer_t* lex)
{
  while (!(tau_lexer_consume(lex, '*') && tau_lexer_consume(lex, '/')))
    tau_lexer_next(lex);
}

tau_token_t* tau_lexer_read_word(tau_lexer_t* lex)
{
  tau_token_t* tok = tau_lexer_token_init(lex, TAU_TOK_ID);

  size_t len = tau_lexer_skip(lex, tau_lexer_is_word);

  if (len > LEXER_MAX_WORD_SIZE - 2)
  {
    tau_location_t loc = tau_lexer_location(lex);

    loc.len = len;

    tau_error_bag_put_lexer_identifier_too_long(lex->errors, loc);
  }

  char buf[LEXER_MAX_WORD_SIZE] = { 0 };

  strncpy(buf, lex->src + tok->pos, len);

  if (tau_lexer_is_kw_vec(buf, len))
  {
    tok->kind = TAU_TOK_KW_VEC;
  }
  else if (tau_lexer_is_kw_mat(buf, len))
  {
    tok->kind = TAU_TOK_KW_MAT;
  }
  else
  {
    for (size_t i = 0; i < TAU_COUNTOF(g_keyword_lookup_table); ++i)
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

tau_token_t* tau_lexer_read_octal_integer(tau_lexer_t* lex)
{
  tau_token_t* tok = tau_lexer_token_init(lex, TAU_TOK_LIT_INT);

  tau_lexer_skip_n(lex, 2);

  size_t len = 2 + tau_lexer_skip(lex, tau_lexer_is_octal);

  len += tau_lexer_skip_integer_suffix(lex);

  if (len == 2 || tau_lexer_is_word(lex))
  {
    tau_location_t loc = tau_lexer_location(lex);

    loc.len = len;

    tau_error_bag_put_lexer_ill_formed_integer(lex->errors, loc);
  }

  return tok;
}

tau_token_t* tau_lexer_read_binary_integer(tau_lexer_t* lex)
{
  tau_token_t* tok = tau_lexer_token_init(lex, TAU_TOK_LIT_INT);

  tau_lexer_skip_n(lex, 2);

  size_t len = 2 + tau_lexer_skip(lex, tau_lexer_is_binary);

  len += tau_lexer_skip_integer_suffix(lex);

  if (len == 2 || tau_lexer_is_word(lex))
  {
    tau_location_t loc = tau_lexer_location(lex);

    loc.len = len;

    tau_error_bag_put_lexer_ill_formed_integer(lex->errors, loc);
  }

  return tok;
}

tau_token_t* tau_lexer_read_decimal_number(tau_lexer_t* lex)
{
  tau_token_t* tok = tau_lexer_token_init(lex, TAU_TOK_LIT_INT);

  size_t len = tau_lexer_skip(lex, tau_lexer_is_decimal);

  if (tau_lexer_current(lex) == '.')
  {
    if (!isdigit(tau_lexer_peek(lex)))
      return tok;

    tau_lexer_next(lex);

    ++len;

    len += tau_lexer_skip(lex, tau_lexer_is_decimal);

    if (tau_lexer_current(lex) == 'e' || tau_lexer_current(lex) == 'E')
    {
      tau_lexer_next(lex);

      ++len;

      if (tau_lexer_current(lex) == '+' || tau_lexer_current(lex) == '-')
      {
        tau_lexer_next(lex);

        ++len;
      }

      len += tau_lexer_skip(lex, tau_lexer_is_decimal);
    }

    if (tau_lexer_is_word(lex))
    {
      tau_location_t loc = tau_lexer_location(lex);

      loc.len = len;

      tau_error_bag_put_lexer_ill_formed_float(lex->errors, loc);
    }

    tok->kind = TAU_TOK_LIT_FLT;

    return tok;
  }

  len += tau_lexer_skip_integer_suffix(lex);

  if (tau_lexer_is_word(lex))
  {
    tau_location_t loc = tau_lexer_location(lex);

    loc.len = len;

    tau_error_bag_put_lexer_ill_formed_integer(lex->errors, loc);
  }

  return tok;
}

tau_token_t* tau_lexer_read_hexadecimal_integer(tau_lexer_t* lex)
{
  tau_token_t* tok = tau_lexer_token_init(lex, TAU_TOK_LIT_INT);

  tau_lexer_skip_n(lex, 2);

  size_t len = 2 + tau_lexer_skip(lex, tau_lexer_is_hexadecimal);

  len += tau_lexer_skip_integer_suffix(lex);

  if (len == 2 || tau_lexer_is_word(lex))
  {
    tau_location_t loc = tau_lexer_location(lex);

    loc.len = len;

    tau_error_bag_put_lexer_ill_formed_integer(lex->errors, loc);
  }

  return tok;
}

tau_token_t* tau_lexer_read_number(tau_lexer_t* lex)
{
  if (tau_lexer_current(lex) == '0')
  {
    char ch = tau_lexer_peek(lex);

    if (ch == 'x' || ch == 'X')
      return tau_lexer_read_hexadecimal_integer(lex);

    if (ch == 'o' || ch == 'O')
      return tau_lexer_read_hexadecimal_integer(lex);

    if (ch == 'b' || ch == 'B')
      return tau_lexer_read_hexadecimal_integer(lex);
  }

  return tau_lexer_read_decimal_number(lex);
}

tau_token_t* tau_lexer_read_string(tau_lexer_t* lex)
{
  tau_token_t* tok = tau_lexer_token_init(lex, TAU_TOK_LIT_STR);

  tau_location_t begin_loc = tau_lexer_location(lex);

  tau_lexer_next(lex);

  char ch;
  while ((ch = tau_lexer_next(lex)) != '"' && ch != '\0')
  {
    if (ch == '\\')
    {
      ch = tau_lexer_next(lex);

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
        if (!isxdigit(tau_lexer_current(lex)))
        {
          tau_location_t loc = tau_lexer_location(lex);

          loc.ptr -= 2;
          loc.len = 2;

          tau_error_bag_put_lexer_missing_hex_digits(lex->errors, loc);
        }

        tau_lexer_skip(lex, tau_lexer_is_hexadecimal);
        break;

      default:
      {
        tau_location_t loc = tau_lexer_location(lex);

        loc.ptr -= 2;
        loc.len = 2;

        tau_error_bag_put_lexer_unknown_escape_sequence(lex->errors, loc);
      }
      }
    }
  }

  if (ch != '"')
  {
    begin_loc.len = 1;

    tau_error_bag_put_lexer_missing_double_quote(lex->errors, begin_loc);
  }

  return tok;
}

tau_token_t* tau_lexer_read_character(tau_lexer_t* lex)
{
  tau_token_t* tok = tau_lexer_token_init(lex, TAU_TOK_LIT_CHAR);

  tau_location_t begin_loc = tau_lexer_location(lex);

  tau_lexer_next(lex);

  size_t len = 0;

  if (tau_lexer_current(lex) == '\'')
  {
    begin_loc.len = 2;

    tau_error_bag_put_lexer_empty_character(lex->errors, begin_loc);

    return tok;
  }

  if (tau_lexer_next(lex) == '\\')
  {
    ++len;

    switch (tau_lexer_next(lex))
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

      if (!isxdigit(tau_lexer_current(lex)))
      {
        tau_location_t loc = tau_lexer_location(lex);

        loc.ptr -= 2;
        loc.len = 2;

        tau_error_bag_put_lexer_missing_hex_digits(lex->errors, loc);
      }

      len += tau_lexer_skip(lex, tau_lexer_is_hexadecimal);

      if (len > 4)
      {
        tau_location_t loc = tau_lexer_location(lex);

        loc.ptr -= len;
        loc.len = len;

        tau_error_bag_put_lexer_too_many_hex_digits(lex->errors, loc);
      }
      break;

    default:
    {
      tau_location_t loc = tau_lexer_location(lex);

      loc.ptr -= 2;
      loc.len = 2;

      tau_error_bag_put_lexer_unknown_escape_sequence(lex->errors, loc);
    }

    }
  }
  else
    ++len;

  if (tau_lexer_current(lex) != '\'')
  {
    begin_loc.len = 1;

    tau_error_bag_put_lexer_missing_double_quote(lex->errors, begin_loc);
  }
  else
  {
    tau_lexer_next(lex);
  }

  return tok;
}

tau_token_t* tau_lexer_read_punctuation(tau_lexer_t* lex)
{
  tau_token_kind_t kind = TAU_TOK_UNKNOWN;

  size_t pos = lex->pos;

  if (tau_lexer_consume(lex, '+'))
    if (tau_lexer_consume(lex, '+'))
      kind = TAU_TOK_PUNCT_PLUS_PLUS;
    else if (tau_lexer_consume(lex, '='))
      kind = TAU_TOK_PUNCT_PLUS_EQUAL;
    else
      kind = TAU_TOK_PUNCT_PLUS;
  else if (tau_lexer_consume(lex, '-'))
    if (tau_lexer_consume(lex, '-'))
      kind = TAU_TOK_PUNCT_MINUS_MINUS;
    else if (tau_lexer_consume(lex, '='))
      kind = TAU_TOK_PUNCT_MINUS_EQUAL;
    else if (tau_lexer_consume(lex, '>'))
      kind = TAU_TOK_PUNCT_MINUS_GREATER;
    else
      kind = TAU_TOK_PUNCT_MINUS;
  else if (tau_lexer_consume(lex, '*'))
    if (tau_lexer_consume(lex, '='))
      kind = TAU_TOK_PUNCT_ASTERISK_EQUAL;
    else
      kind = TAU_TOK_PUNCT_ASTERISK;
  else if (tau_lexer_consume(lex, '/'))
  {
    if (tau_lexer_consume(lex, '/'))
    {
      tau_lexer_skip_comment_line(lex);
      return tau_lexer_read_next(lex);
    }

    if (tau_lexer_consume(lex, '='))
      kind = TAU_TOK_PUNCT_SLASH_EQUAL;
    else if (tau_lexer_consume(lex, '*'))
    {
      tau_lexer_skip_comment_block(lex);
      return tau_lexer_read_next(lex);
    }
    else
      kind = TAU_TOK_PUNCT_SLASH;
  }
  else if (tau_lexer_consume(lex, '%'))
    if (tau_lexer_consume(lex, '='))
      kind = TAU_TOK_PUNCT_PERCENT_EQUAL;
    else
      kind = TAU_TOK_PUNCT_PERCENT;
  else if (tau_lexer_consume(lex, '&'))
    if (tau_lexer_consume(lex, '&'))
      kind = TAU_TOK_PUNCT_AMPERSAND_AMPERSAND;
    else if (tau_lexer_consume(lex, '='))
      kind = TAU_TOK_PUNCT_AMPERSAND_EQUAL;
    else
      kind = TAU_TOK_PUNCT_AMPERSAND;
  else if (tau_lexer_consume(lex, '|'))
    if (tau_lexer_consume(lex, '|'))
      kind = TAU_TOK_PUNCT_BAR_BAR;
    else if (tau_lexer_consume(lex, '='))
      kind = TAU_TOK_PUNCT_BAR_EQUAL;
    else
      kind = TAU_TOK_PUNCT_BAR;
  else if (tau_lexer_consume(lex, '^'))
    if (tau_lexer_consume(lex, '='))
      kind = TAU_TOK_PUNCT_HAT_EQUAL;
    else
      kind = TAU_TOK_PUNCT_HAT;
  else if (tau_lexer_consume(lex, '~'))
    kind = TAU_TOK_PUNCT_TILDE;
  else if (tau_lexer_consume(lex, '<'))
    if (tau_lexer_consume(lex, '<'))
      if (tau_lexer_consume(lex, '='))
        kind = TAU_TOK_PUNCT_LESS_LESS_EQUAL;
      else
        kind = TAU_TOK_PUNCT_LESS_LESS;
    else if (tau_lexer_consume(lex, '='))
      kind = TAU_TOK_PUNCT_LESS_EQUAL;
    else
      kind = TAU_TOK_PUNCT_LESS;
  else if (tau_lexer_consume(lex, '>'))
    if (tau_lexer_consume(lex, '>'))
      if (tau_lexer_consume(lex, '='))
        kind = TAU_TOK_PUNCT_GREATER_GREATER_EQUAL;
      else
        kind = TAU_TOK_PUNCT_GREATER_GREATER;
    else if (tau_lexer_consume(lex, '='))
      kind = TAU_TOK_PUNCT_GREATER_EQUAL;
    else if (tau_lexer_consume(lex, ']'))
      kind = TAU_TOK_PUNCT_BRACKET_ANGLE_RIGHT;
    else
      kind = TAU_TOK_PUNCT_GREATER;
  else if (tau_lexer_consume(lex, '!'))
    if (tau_lexer_consume(lex, '='))
      kind = TAU_TOK_PUNCT_BANG_EQUAL;
    else
      kind = TAU_TOK_PUNCT_BANG;
  else if (tau_lexer_consume(lex, '.'))
    if (tau_lexer_consume(lex, '.'))
      if (tau_lexer_consume(lex, '.'))
        kind = TAU_TOK_PUNCT_DOT_DOT_DOT;
      else
        kind = TAU_TOK_PUNCT_DOT_DOT;
    else if (tau_lexer_consume(lex, '<'))
      kind = TAU_TOK_PUNCT_DOT_LESS;
    else
      kind = TAU_TOK_PUNCT_DOT;
  else if (tau_lexer_consume(lex, '?'))
    if (tau_lexer_consume(lex, '.'))
      kind = TAU_TOK_PUNCT_QUESTION_DOT;
    else
      kind = TAU_TOK_PUNCT_QUESTION;
  else if (tau_lexer_consume(lex, '='))
    if (tau_lexer_consume(lex, '='))
      kind = TAU_TOK_PUNCT_EQUAL_EQUAL;
    else
      kind = TAU_TOK_PUNCT_EQUAL;
  else if (tau_lexer_consume(lex, ','))
    kind = TAU_TOK_PUNCT_COMMA;
  else if (tau_lexer_consume(lex, ':'))
    kind = TAU_TOK_PUNCT_COLON;
  else if (tau_lexer_consume(lex, ';'))
    kind = TAU_TOK_PUNCT_SEMICOLON;
  else if (tau_lexer_consume(lex, '('))
    kind = TAU_TOK_PUNCT_PAREN_LEFT;
  else if (tau_lexer_consume(lex, ')'))
    kind = TAU_TOK_PUNCT_PAREN_RIGHT;
  else if (tau_lexer_consume(lex, '['))
    if (tau_lexer_consume(lex, '<'))
      kind = TAU_TOK_PUNCT_BRACKET_ANGLE_LEFT;
    else
      kind = TAU_TOK_PUNCT_BRACKET_LEFT;
  else if (tau_lexer_consume(lex, ']'))
    kind = TAU_TOK_PUNCT_BRACKET_RIGHT;
  else if (tau_lexer_consume(lex, '{'))
    kind = TAU_TOK_PUNCT_BRACE_LEFT;
  else if (tau_lexer_consume(lex, '}'))
    kind = TAU_TOK_PUNCT_BRACE_RIGHT;
  else if (tau_lexer_consume(lex, '#'))
    kind = TAU_TOK_PUNCT_HASH;
  else
  {
    tau_location_t loc = tau_lexer_location(lex);

    loc.len = 1;

    tau_error_bag_put_lexer_unexpected_character(lex->errors, loc);
  }

  return tau_token_registry_token_init(lex->path, kind, pos);
}

tau_token_t* tau_lexer_read_next(tau_lexer_t* lex)
{
  tau_lexer_skip(lex, tau_lexer_is_space);

  if (tau_lexer_is_word_begin(lex))
    return tau_lexer_read_word(lex);

  if (tau_lexer_is_decimal(lex))
    return tau_lexer_read_number(lex);

  if (tau_lexer_current(lex) == '"')
    return tau_lexer_read_string(lex);

  if (tau_lexer_current(lex) == '\'')
    return tau_lexer_read_character(lex);

  if (tau_lexer_is_punctuation(lex))
    return tau_lexer_read_punctuation(lex);

  if (tau_lexer_current(lex) == '\0')
    return tau_lexer_token_init(lex, TAU_TOK_EOF);

  tau_location_t loc = tau_lexer_location(lex);

  loc.len = 1;

  tau_error_bag_put_lexer_unexpected_character(lex->errors, loc);

  return NULL;
}

void tau_lexer_lex(tau_lexer_t* lex, const char* path, const char* src, tau_vector_t* tokens, tau_error_bag_t* errors)
{
  lex->path = path;
  lex->src = src;
  lex->pos = 0;

  lex->tokens = tokens;
  lex->errors = errors;

  tau_token_registry_register_file(path, src);

  while (tau_vector_empty(lex->tokens) || ((tau_token_t*)tau_vector_back(lex->tokens))->kind != TAU_TOK_EOF)
  {
    tau_vector_push(lex->tokens, tau_lexer_read_next(lex));

    if (tau_error_bag_full(lex->errors))
      return;
  }
}
