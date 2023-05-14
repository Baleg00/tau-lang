#include "lexer.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "util.h"
#include "log.h"
#include "crumb.h"

#include "list.h"

#include "file.h"

#include "location.h"
#include "token.h"

#include "diagnostics.h"

#include "memtrace.h"

#define IDENTIFIER_MAX_LEN 255

lexer_t* lexer_init(char* path, char* src)
{
  lexer_t* lex = (lexer_t*)malloc(sizeof(lexer_t));
  assert(lex != NULL);
  lex->loc = location_init(path, src, src, 0, 0, 0);
  lex->toks = list_init();
  return lex;
}

LIST_FOR_EACH_FUNC_DECL(token_free, token_t);

void lexer_free(lexer_t* lex)
{
  list_for_each(lex->toks, LIST_FOR_EACH_FUNC_NAME(token_free));
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
  location_t* loc = location_copy(lex->loc);
  token_t* tok = token_init(kind, loc);
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
    { "Self",     TOK_KW_SELF },
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
    { "true",     TOK_LIT_BOOL },
    { "false",    TOK_LIT_BOOL },
    { "null",     TOK_LIT_NULL },
  };

  location_t* loc = location_copy(lex->loc);

  size_t len = lexer_skip(lex, lexer_is_word);
  loc->len = len;

  if (len > IDENTIFIER_MAX_LEN)
    report_error_identifier_too_long(loc);

  char* begin = loc->cur;

  char buf[IDENTIFIER_MAX_LEN + 1];
  strncpy(buf, begin, len);
  buf[len] = '\0';

  token_kind_t kind = TOK_ID;

  for (size_t i = 0; i < sizeof(lookup) / sizeof(lookup[0]); ++i)
    if (strcmp(lookup[i].keyword, buf) == 0)
    {
      kind = lookup[i].kind;
      break;
    }

  token_t* tok = NULL;

  if (kind == TOK_ID)
  {
    char* value = (char*)malloc((len + 1) * sizeof(char));
    assert(value != NULL);
    strcpy(value, buf);
    
    tok = (token_t*)token_id_init(loc, value);
  }
  else
    tok = token_init(kind, loc);

  lexer_token_push(lex, tok);
}

void lexer_read_octal_integer(lexer_t* lex)
{
  location_t* loc = location_copy(lex->loc);

  lexer_skip_n(lex, 2);

  size_t len = 2 + lexer_skip(lex, lexer_is_octal);
  loc->len = len;

  if (len == 2 || lexer_is_word(lex))
    report_error_ill_formed_integer_literal(loc);

  uint64_t value = strtoull(lex->loc->cur - len, NULL, 8);
  token_lit_int_t* tok = token_lit_int_init(loc, value);
  lexer_token_push(lex, (token_t*)tok);
}

void lexer_read_binary_integer(lexer_t* lex)
{
  location_t* loc = location_copy(lex->loc);
  
  lexer_skip_n(lex, 2);

  size_t len = 2 + lexer_skip(lex, lexer_is_binary);
  loc->len = len;

  if (len == 2 || lexer_is_word(lex))
    report_error_ill_formed_integer_literal(loc);

  uint64_t value = strtoull(lex->loc->cur - len, NULL, 2);
  token_lit_int_t* tok = token_lit_int_init(loc, value);
  lexer_token_push(lex, (token_t*)tok);
}

void lexer_read_decimal_number(lexer_t* lex)
{
  location_t* loc = location_copy(lex->loc);
  
  size_t len = lexer_skip(lex, lexer_is_decimal);

  if (lexer_current(lex) == '.')
  {
    if (!isdigit(lexer_peek(lex)))
    {
      loc->len = len;
      uint64_t value = strtoull(lex->loc->cur - len, NULL, 10);
      token_lit_int_t* tok = token_lit_int_init(loc, value);
      lexer_token_push(lex, (token_t*)tok);

      lexer_read_punctuation(lex);
      return;
    }

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

    loc->len = len;

    if (lexer_is_word(lex))
      report_error_ill_formed_floating_point_literal(loc);
    
    long double value = strtold(lex->loc->cur - len, NULL);
    token_lit_flt_t* tok = token_lit_flt_init(loc, value);
    lexer_token_push(lex, (token_t*)tok);
  }
  else
  {
    loc->len = len;

    if (lexer_is_word(lex))
      report_error_ill_formed_integer_literal(loc);

    uint64_t value = strtoull(lex->loc->cur - len, NULL, 10);
    token_lit_int_t* tok = token_lit_int_init(loc, value);
    lexer_token_push(lex, (token_t*)tok);
  }
}

void lexer_read_hexadecimal_integer(lexer_t* lex)
{
  location_t* loc = location_copy(lex->loc);

  lexer_skip_n(lex, 2);

  size_t len = 2 + lexer_skip(lex, lexer_is_hexadecimal);
  loc->len = len;

  if (len == 2 || lexer_is_word(lex))
    report_error_ill_formed_integer_literal(loc);

  uint64_t value = strtoull(lex->loc->cur - len, NULL, 16);
  token_lit_int_t* tok = token_lit_int_init(loc, value);
  lexer_token_push(lex, (token_t*)tok);
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
  location_t* loc = location_copy(lex->loc);

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

        if (!isxdigit(lexer_current(lex)))
        {
          loc->len = 2;
          loc->cur = lex->loc->cur - 2;
          report_error_missing_hex_digits_in_escape_sequence(loc);
        }

        len += lexer_skip(lex, lexer_is_hexadecimal);
        break;

      default:
        loc->len = 2;
        loc->cur = lex->loc->cur - 2;
        report_error_unknown_escape_sequence(loc);
      }
    }
  }

  if (ch != '"')
  {
    loc->len = 1;
    report_error_missing_terminating_double_quotes(loc);
  }

  loc->len = len + 2;

  char* buf = (char*)malloc((len + 1) * sizeof(char));
  assert(buf != NULL);

  strncpy(buf, lex->loc->cur - len - 1, len);
  buf[len] = '\0';

  token_lit_str_t* tok = token_lit_str_init(loc, buf);
  lexer_token_push(lex, (token_t*)tok);
}

void lexer_read_character(lexer_t* lex)
{
  static const struct {
    const char* const key;
    char value;
  } lookup[] = {
    { "\\\\", '\\' },
    { "\\b",  '\b' },
    { "\\f",  '\f' },
    { "\\n",  '\n' },
    { "\\r",  '\r' },
    { "\\t",  '\t' },
    { "\\\'", '\'' },
    { "\\\"", '"'  }
  };

  location_t* loc = location_copy(lex->loc);

  lexer_next(lex);

  size_t len = 0;

  if (lexer_current(lex) == '\'')
  {
    loc->len = 2;
    report_error_empty_character_literal(loc);
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
        loc->len = 2;
        loc->cur = lex->loc->cur - 2;
        report_error_missing_hex_digits_in_escape_sequence(loc);
      }

      len += lexer_skip(lex, lexer_is_hexadecimal);

      if (len > 4)
      {
        loc->len = len;
        loc->cur = lex->loc->cur - len;
        report_error_too_many_hex_digits_in_escape_sequence(loc);
      }
      break;

    default:
      loc->len = 2;
      loc->cur = lex->loc->cur - 2;
      report_error_unknown_escape_sequence(loc);
    }
  }
  else
    ++len;

  if (lexer_next(lex) != '\'')
  {
    loc->len = 1;
    report_error_missing_terminating_single_quote(loc);
  }

  loc->len = len + 2;
  char* begin = loc->cur + 1;
  char value = '\0';

  if (len == 1)
    value = *begin;
  else if (len == 2)
  {
    for (size_t i = 0; i < sizeof(lookup) / sizeof(lookup[0]); ++i)
      if (strncmp(lookup[i].key, begin, len) == 0)
      {
        value = lookup[i].value;
        break;
      }
  }
  else
    value = (char)strtol(begin + 2, NULL, 16);

  token_lit_char_t* tok = token_lit_char_init(loc, value);
  lexer_token_push(lex, (token_t*)tok);
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
  tok->loc->len = 1;
  
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
