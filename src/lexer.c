#include "lexer.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "util.h"
#include "file.h"
#include "log.h"
#include "crumb.h"
#include "diagnostics.h"
#include "list.h"
#include "token.h"
#include "memtrace.h"

lexer_t* lexer_init(char* path, char* src)
{
  lexer_t* lex = (lexer_t*)malloc(sizeof(lexer_t));
  assert(lex != NULL);

  lex->loc = location_init();

  lex->loc->path = path;
  lex->loc->src = src;
  lex->loc->row = 0;
  lex->loc->col = 0;
  lex->loc->len = 0;

  lex->loc->cur = lex->loc->src;

  lex->toks = list_init();

  return lex;
}

void lexer_free(lexer_t* lex)
{
  list_for_each(lex->toks, token_free);
  list_free(lex->toks);
  location_free(lex->loc);
  free(lex);
}

list_t* lexer_tokens(lexer_t* lex)
{
  return lex->toks;
}

token_t* lexer_token_init(lexer_t* lex, token_kind_t kind)
{
  token_t* tok = token_init();
  tok->kind = kind;

  tok->loc = location_init();
  *tok->loc = *lex->loc;

  return tok;
}

void lexer_token_push(lexer_t* lex, token_t* tok)
{
  list_push_back(lex->toks, tok);
}

int lexer_is_space(lexer_t* lex)
{
  return isspace(*lex->loc->cur);
}

int lexer_is_word_begin(lexer_t* lex)
{
  return isalpha(*lex->loc->cur) || *lex->loc->cur == '_';
}

int lexer_is_word(lexer_t* lex)
{
  return isalnum(*lex->loc->cur) || *lex->loc->cur == '_';
}

int lexer_is_decimal(lexer_t* lex)
{
  return isdigit(*lex->loc->cur);
}

int lexer_is_hexadecimal(lexer_t* lex)
{
  return isxdigit(*lex->loc->cur);
}

int lexer_is_octal(lexer_t* lex)
{
  return '0' <= *lex->loc->cur && '7' >= *lex->loc->cur;
}

int lexer_is_binary(lexer_t* lex)
{
  return *lex->loc->cur == '0' || *lex->loc->cur == '1';
}

int lexer_is_punctuation(lexer_t* lex)
{
  return ispunct(*lex->loc->cur);
}

char lexer_current(lexer_t* lex)
{
  return *lex->loc->cur;
}

char lexer_next(lexer_t* lex)
{
  if (*lex->loc->cur == '\0')
    return '\0';
  
  if (*lex->loc->cur == '\n')
    {
      ++lex->loc->row;
      lex->loc->col = 0;
    }
    else
      ++lex->loc->col;

  return *(lex->loc->cur++);
}

char lexer_peek(lexer_t* lex)
{
  return *lex->loc->cur == '\0' ? '\0' : *(lex->loc->cur + 1);
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

size_t lexer_skip(lexer_t* lex, int(*pred)(lexer_t*))
{
  const char* begin = lex->loc->cur;

  while (*lex->loc->cur != '\0' && pred(lex))
    lexer_next(lex);

  return (size_t)(lex->loc->cur - begin);
}

void lexer_skip_n(lexer_t* lex, size_t n)
{
  while (lexer_current(lex) != '\0' && n-- > 0)
    lexer_next(lex);
}

void lexer_read_word(lexer_t* lex)
{
  static const struct {
    const char* const keyword;
    const token_kind_t kind;
  } lookup[] = {
    { "is",       TOK_KW_IS },
    { "as",       TOK_KW_AS },
    { "sizeof",   TOK_KW_SIZEOF },
    { "alignof",  TOK_KW_ALIGNOF },
    { "typeof",   TOK_KW_TYPEOF },
    { "in",       TOK_KW_IN },
    { "fun",      TOK_KW_FUN },
    { "gen",      TOK_KW_GEN },
    { "struct",   TOK_KW_STRUCT },
    { "union",    TOK_KW_UNION },
    { "enum",     TOK_KW_ENUM },
    { "mod",      TOK_KW_MOD },
    { "use",      TOK_KW_USE },
    { "if",       TOK_KW_IF },
    { "then",     TOK_KW_THEN },
    { "else",     TOK_KW_ELSE },
    { "for",      TOK_KW_FOR },
    { "while",    TOK_KW_WHILE },
    { "do",       TOK_KW_DO },
    { "break",    TOK_KW_BREAK },
    { "continue", TOK_KW_CONTINUE },
    { "return",   TOK_KW_RETURN },
    { "yield",    TOK_KW_YIELD },
    { "mut",      TOK_KW_MUT },
    { "const",    TOK_KW_CONST },
    { "type",     TOK_KW_TYPE },
    { "i8",       TOK_KW_I8 },
    { "i16",      TOK_KW_I16 },
    { "i32",      TOK_KW_I32 },
    { "i64",      TOK_KW_I64 },
    { "isize",    TOK_KW_ISIZE },
    { "u8",       TOK_KW_U8 },
    { "u16",      TOK_KW_U16 },
    { "u32",      TOK_KW_U32 },
    { "u64",      TOK_KW_U64 },
    { "usize",    TOK_KW_USIZE },
    { "f32",      TOK_KW_F32 },
    { "f64",      TOK_KW_F64 },
    { "bool",     TOK_KW_BOOL },
    { "unit",     TOK_KW_UNIT },
    { "true",     TOK_LIT_BOOL_TRUE },
    { "false",    TOK_LIT_BOOL_FALSE },
    { "null",     TOK_LIT_NULL },
  };

  token_t* tok = lexer_token_init(lex, TOK_UNKNOWN);

  char* begin = lex->loc->cur;
  
  size_t len = lexer_skip(lex, lexer_is_word);
  tok->loc->len = len;
  
  char* buf = (char*)malloc((len + 1) * sizeof(char));
  assert(buf != NULL);

  memcpy(buf, begin, len * sizeof(char));
  buf[len] = '\0';

  for (size_t i = 0; i < sizeof(lookup) / sizeof(lookup[0]); ++i)
    if (strcmp(lookup[i].keyword, buf) == 0)
    {
      tok->kind = lookup[i].kind;
      free(buf);
      break;
    }

  if (tok->kind == TOK_UNKNOWN)
  {
    tok->kind = TOK_ID;
    tok->id.value = buf;
  }

  lexer_token_push(lex, tok);
}

void lexer_read_octal_integer(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT_OCT);

  lexer_skip_n(lex, 2);

  size_t len = 2 + lexer_skip(lex, lexer_is_octal);
  tok->loc->len = len;

  if (len == 2 || lexer_is_word(lex))
    report_error_ill_formed_integer_literal(tok->loc);

  tok->lit_int.value = strtoull(lex->loc->cur - len, NULL, 8);

  lexer_token_push(lex, tok);
}

void lexer_read_binary_integer(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT_BIN);

  lexer_skip_n(lex, 2);

  size_t len = 2 + lexer_skip(lex, lexer_is_binary);
  tok->loc->len = len;

  if (len == 2 || lexer_is_word(lex))
    report_error_ill_formed_integer_literal(tok->loc);

  tok->lit_int.value = strtoull(lex->loc->cur - len, NULL, 2);

  lexer_token_push(lex, tok);
}

void lexer_read_decimal_number(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT_DEC);
  
  size_t len = lexer_skip(lex, lexer_is_decimal);

  if (lexer_current(lex) == '.')
  {
    if (!isdigit(lexer_peek(lex)))
    {
      tok->lit_int.value = strtoull(lex->loc->cur - len, NULL, 10);
      tok->loc->len = len;
      lexer_token_push(lex, tok);

      lexer_read_punctuation(lex);
      return;
    }
    
    tok->kind = TOK_LIT_FLT_DEC;

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

    tok->lit_flt.value = strtold(lex->loc->cur - len, NULL);
    tok->loc->len = len;

    if (lexer_is_word(lex))
      report_error_ill_formed_float_literal(tok->loc);

    lexer_token_push(lex, tok);
  }
  else
  {
    tok->lit_int.value = strtoull(lex->loc->cur - len, NULL, 10);
    tok->loc->len = len;

    if (lexer_is_word(lex))
      report_error_ill_formed_integer_literal(tok->loc);

    lexer_token_push(lex, tok);
  }
}

void lexer_read_hexadecimal_integer(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT_HEX);

  lexer_skip_n(lex, 2);

  size_t len = 2 + lexer_skip(lex, lexer_is_hexadecimal);
  tok->loc->len = len;

  if (len == 2 || lexer_is_word(lex))
    report_error_ill_formed_integer_literal(tok->loc);

  tok->lit_int.value = strtoull(lex->loc->cur - len, NULL, 16);

  lexer_token_push(lex, tok);
}

void lexer_read_number(lexer_t* lex)
{
  if (lexer_current(lex) == '0')
    switch (lexer_peek(lex))
    {
    case 'x':
    case 'X':
      lexer_read_hexadecimal_integer(lex);
      return;

    case 'o':
    case 'O':
      lexer_read_octal_integer(lex);
      return;

    case 'b':
    case 'B':
      lexer_read_binary_integer(lex);
      return;
    }

  lexer_read_decimal_number(lex);
}

void lexer_read_string(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_STR);

  char ch = lexer_next(lex);

  size_t len = 0;

  while ((ch = lexer_next(lex)) != '"' && ch != '\0')
  {
    ++len;

    if (ch == '\\')
    {
      ch = lexer_next(lex);
      ++len;

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
        ++len;
        break;
      
      case 'x':
      case 'X': // arbitrary hexadecimal bytes
        ++len;

        if (!isxdigit(lexer_next(lex)))
          report_error_escape_no_hex_digits(tok->loc);

        len += lexer_skip(lex, lexer_is_hexadecimal);
        break;

      default:
        report_error_unknown_escape_sequence(tok->loc);
      }
    }
  }

  if (ch != '"')
  {
    tok->loc->len = 1;
    report_error_missing_terminating_character(tok->loc, '"');
  }

  tok->loc->len = len;

  char* buf = (char*)malloc((len + 1) * sizeof(char));
  assert(buf != NULL);

  memcpy(buf, lex->loc->cur - len - 2, len * sizeof(char));
  buf[len] = '\0';

  tok->lit_str.value = buf;

  lexer_token_push(lex, tok);
}

void lexer_read_character(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_CHAR);
  
  lexer_next(lex);

  size_t len = 0;

  if (lexer_current(lex) == '\'')
  {
    tok->loc->len = 2;
    report_error_empty_character_literal(tok->loc);
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

      if (!isxdigit(lexer_next(lex)))
        report_error_escape_no_hex_digits(tok->loc);

      len += lexer_skip(lex, lexer_is_hexadecimal);
      break;

    default:
      report_error_unknown_escape_sequence(tok->loc);
    }
  }
  else
    ++len;

  if (lexer_next(lex) != '\'')
  {
    tok->loc->len = 1;
    report_error_missing_terminating_character(tok->loc, '\'');
  }

  tok->loc->len = len;

  char* buf = (char*)malloc((len + 1) * sizeof(char));  
  assert(buf != NULL);

  memcpy(buf, lex->loc->cur - len - 2, len * sizeof(char));
  buf[len] = '\0';

  tok->lit_char.value = buf;

  lexer_token_push(lex, tok);
}

void lexer_read_punctuation(lexer_t* lex)
{
  static const struct {
    const token_kind_t kind;
    const size_t len;
  } lookup[] = {
    { TOK_PUNCT_PLUS,                  1 },
    { TOK_PUNCT_PLUS_PLUS,             2 },
    { TOK_PUNCT_PLUS_EQUAL,            2 },
    { TOK_PUNCT_MINUS,                 1 },
    { TOK_PUNCT_MINUS_MINUS,           2 },
    { TOK_PUNCT_MINUS_EQUAL,           2 },
    { TOK_PUNCT_ASTERISK,              1 },
    { TOK_PUNCT_ASTERISK_EQUAL,        2 },
    { TOK_PUNCT_ASTERISK_DOT,          2 },
    { TOK_PUNCT_SLASH,                 1 },
    { TOK_PUNCT_SLASH_EQUAL,           2 },
    { TOK_PUNCT_PERCENT,               1 },
    { TOK_PUNCT_PERCENT_EQUAL,         2 },
    { TOK_PUNCT_AMPERSAND,             1 },
    { TOK_PUNCT_AMPERSAND_AMPERSAND,   2 },
    { TOK_PUNCT_AMPERSAND_EQUAL,       2 },
    { TOK_PUNCT_BAR,                   1 },
    { TOK_PUNCT_BAR_BAR,               2 },
    { TOK_PUNCT_BAR_EQUAL,             2 },
    { TOK_PUNCT_HAT,                   1 },
    { TOK_PUNCT_HAT_EQUAL,             2 },
    { TOK_PUNCT_TILDE,                 1 },
    { TOK_PUNCT_LESS,                  1 },
    { TOK_PUNCT_LESS_LESS,             2 },
    { TOK_PUNCT_LESS_LESS_EQUAL,       3 },
    { TOK_PUNCT_LESS_EQUAL,            2 },
    { TOK_PUNCT_GREATER,               1 },
    { TOK_PUNCT_GREATER_GREATER,       2 },
    { TOK_PUNCT_GREATER_GREATER_EQUAL, 3 },
    { TOK_PUNCT_GREATER_EQUAL,         2 },
    { TOK_PUNCT_BANG,                  1 },
    { TOK_PUNCT_BANG_EQUAL,            2 },
    { TOK_PUNCT_DOT,                   1 },
    { TOK_PUNCT_DOT_DOT,               2 },
    { TOK_PUNCT_DOT_DOT_DOT,           3 },
    { TOK_PUNCT_QUESTION,              1 },
    { TOK_PUNCT_QUESTION_DOT,          2 },
    { TOK_PUNCT_EQUAL,                 1 },
    { TOK_PUNCT_EQUAL_EQUAL,           2 },
    { TOK_PUNCT_COMMA,                 1 },
    { TOK_PUNCT_COLON,                 1 },
    { TOK_PUNCT_SEMICOLON,             1 },
    { TOK_PUNCT_PAREN_LEFT,            1 },
    { TOK_PUNCT_PAREN_RIGHT,           1 },
    { TOK_PUNCT_BRACKET_LEFT,          1 },
    { TOK_PUNCT_BRACKET_RIGHT,         1 },
    { TOK_PUNCT_BRACE_LEFT,            1 },
    { TOK_PUNCT_BRACE_RIGHT,           1 },
  };

  token_t* tok = lexer_token_init(lex, TOK_UNKNOWN);
  token_kind_t kind = TOK_UNKNOWN;

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
    else
      kind = TOK_PUNCT_MINUS;
  else if (lexer_consume(lex, '*'))
    if (lexer_consume(lex, '.'))
      kind = TOK_PUNCT_ASTERISK_DOT;
    else if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_ASTERISK_EQUAL;
    else
      kind = TOK_PUNCT_ASTERISK;
  else if (lexer_consume(lex, '/'))
    if (lexer_consume(lex, '='))
      kind = TOK_PUNCT_SLASH_EQUAL;
    else
      kind = TOK_PUNCT_SLASH;
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
    kind = TOK_PUNCT_BRACKET_LEFT;
  else if (lexer_consume(lex, ']'))
    kind = TOK_PUNCT_BRACKET_RIGHT;
  else if (lexer_consume(lex, '{'))
    kind = TOK_PUNCT_BRACE_LEFT;
  else if (lexer_consume(lex, '}'))
    kind = TOK_PUNCT_BRACE_RIGHT;
  else
    report_error_unexpected_character(tok->loc);

  tok->kind = kind;
  tok->loc->len = 0;

  for (size_t i = 0; i < sizeof(lookup) / sizeof(lookup[0]); ++i)
    if (lookup[i].kind == kind)
    {
      tok->loc->len = lookup[i].len;
      break;
    }

  lexer_token_push(lex, tok);
}

void lexer_read_next(lexer_t* lex)
{
  lexer_skip(lex, lexer_is_space);

  if (lexer_is_word_begin(lex))
    lexer_read_word(lex);
  else if (lexer_is_decimal(lex))
    lexer_read_number(lex);
  else if (lexer_current(lex) == '"')
    lexer_read_string(lex);
  else if (lexer_current(lex) == '\'')
    lexer_read_character(lex);
  else if (lexer_is_punctuation(lex))
    lexer_read_punctuation(lex);
  else if (lexer_current(lex) == '\0')
    lexer_token_push(lex, lexer_token_init(lex, TOK_EOF));
  else
    report_error_unexpected_character(lex->loc);
}

void lexer_lex(lexer_t* lex)
{
  while (list_empty(lex->toks) || ((token_t*)list_back(lex->toks))->kind != TOK_EOF)
    lexer_read_next(lex);
}
