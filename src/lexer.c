#include "lexer.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "util.h"
#include "log.h"
#include "crumb.h"

#include "file.h"

#include "token.h"

#include "diagnostics.h"

#define LEXER_MAX_BUFFER_SIZE 256

void lexer_init(lexer_t* lex, arena_t* arena, const char* path, char* src)
{
  lex->arena = arena;

  lex->loc = location_init(path, src, src, 0, 0, 0);
}

void lexer_free(lexer_t* lex)
{
  location_free(lex->loc);
}

location_t* lexer_copy_loc(lexer_t* lex)
{
  location_t* loc = location_init(
    location_get_path(lex->loc),
    location_get_src(lex->loc),
    location_get_ptr(lex->loc),
    location_get_row(lex->loc),
    location_get_col(lex->loc),
    location_get_len(lex->loc)
  );

  return loc;
}

token_t* lexer_token_init(lexer_t* lex, token_kind_t kind)
{
  location_t* loc = lexer_copy_loc(lex);
  token_t* tok = token_init(kind, loc);
  return tok;
}

bool lexer_is_space(lexer_t* lex)
{
  return isspace(*location_get_ptr(lex->loc)) != 0;
}

bool lexer_is_word_begin(lexer_t* lex)
{
  return isalpha(*location_get_ptr(lex->loc)) != 0 || *location_get_ptr(lex->loc) == '_';
}

bool lexer_is_word(lexer_t* lex)
{
  return isalnum(*location_get_ptr(lex->loc)) != 0 || *location_get_ptr(lex->loc) == '_';
}

bool lexer_is_decimal(lexer_t* lex)
{
  return isdigit(*location_get_ptr(lex->loc)) != 0;
}

bool lexer_is_hexadecimal(lexer_t* lex)
{
  return isxdigit(*location_get_ptr(lex->loc)) != 0;
}

bool lexer_is_octal(lexer_t* lex)
{
  return '0' <= *location_get_ptr(lex->loc) && '7' >= *location_get_ptr(lex->loc);
}

bool lexer_is_binary(lexer_t* lex)
{
  return *location_get_ptr(lex->loc) == '0' || *location_get_ptr(lex->loc) == '1';
}

bool lexer_is_punctuation(lexer_t* lex)
{
  return ispunct(*location_get_ptr(lex->loc)) != 0;
}

char lexer_current(lexer_t* lex)
{
  return *location_get_ptr(lex->loc);
}

char lexer_next(lexer_t* lex)
{
  if (*location_get_ptr(lex->loc) == '\0')
    return '\0';
  
  if (*location_get_ptr(lex->loc) == '\n')
  {
    location_set_row(lex->loc, location_get_row(lex->loc) + 1);
    location_set_col(lex->loc, 0);
  }
  else
    location_set_col(lex->loc, location_get_col(lex->loc) + 1);

  char ch = *location_get_ptr(lex->loc);

  location_set_ptr(lex->loc, location_get_ptr(lex->loc) + 1);

  return ch;
}

char lexer_peek(lexer_t* lex)
{
  return *location_get_ptr(lex->loc) == '\0' ? '\0' : *(location_get_ptr(lex->loc) + 1);
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
  const char* begin = location_get_ptr(lex->loc);

  while (*location_get_ptr(lex->loc) != '\0' && pred(lex))
    lexer_next(lex);

  return (size_t)(location_get_ptr(lex->loc) - begin);
}

void lexer_skip_n(lexer_t* lex, size_t n)
{
  while (lexer_current(lex) != '\0' && n-- > 0)
    lexer_next(lex);
}

token_t* lexer_read_word(lexer_t* lex)
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

  token_t* tok = lexer_token_init(lex, TOK_ID);

  size_t len = lexer_skip(lex, lexer_is_word);
  location_set_len(token_get_loc(tok), len);

  if (len > LEXER_MAX_BUFFER_SIZE - 1)
    report_error_identifier_too_long(token_get_loc(tok));

  const char* begin = location_get_ptr(token_get_loc(tok));

  char buf[LEXER_MAX_BUFFER_SIZE];
  strncpy(buf, begin, len);
  buf[len] = '\0';

  for (size_t i = 0; i < countof(lookup); ++i)
    if (strcmp(lookup[i].keyword, buf) == 0)
    {
      token_set_kind(tok, lookup[i].kind);
      break;
    }

  return tok;
}

token_t* lexer_read_octal_integer(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT);

  lexer_skip_n(lex, 2);

  size_t len = 2 + lexer_skip(lex, lexer_is_octal);
  location_set_len(token_get_loc(tok), len);

  if (len == 2 || lexer_is_word(lex))
    report_error_ill_formed_integer_literal(token_get_loc(tok));

  return tok;
}

token_t* lexer_read_binary_integer(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT);

  lexer_skip_n(lex, 2);

  size_t len = 2 + lexer_skip(lex, lexer_is_binary);
  location_set_len(token_get_loc(tok), len);

  if (len == 2 || lexer_is_word(lex))
    report_error_ill_formed_integer_literal(token_get_loc(tok));

  return tok;
}

token_t* lexer_read_decimal_number(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT);
  
  size_t len = lexer_skip(lex, lexer_is_decimal);

  if (lexer_current(lex) == '.')
  {
    if (!isdigit(lexer_peek(lex)))
    {
      location_set_len(token_get_loc(tok), len);
      return tok;
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

    location_set_len(token_get_loc(tok), len);

    if (lexer_is_word(lex))
      report_error_ill_formed_floating_point_literal(token_get_loc(tok));
    
    token_set_kind(tok, TOK_LIT_FLT);
    return tok;
  }

  location_set_len(token_get_loc(tok), len);

  if (lexer_is_word(lex))
    report_error_ill_formed_integer_literal(token_get_loc(tok));

  return tok;
}

token_t* lexer_read_hexadecimal_integer(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT);

  lexer_skip_n(lex, 2);

  size_t len = 2 + lexer_skip(lex, lexer_is_hexadecimal);
  location_set_len(token_get_loc(tok), len);

  if (len == 2 || lexer_is_word(lex))
    report_error_ill_formed_integer_literal(token_get_loc(tok));

  return tok;
}

token_t* lexer_read_number(lexer_t* lex)
{
  if (lexer_current(lex) == '0')
    switch (lexer_peek(lex))
    {
    case 'x':
    case 'X':
      return lexer_read_hexadecimal_integer(lex);
    case 'o':
    case 'O':
      return lexer_read_octal_integer(lex);
    case 'b':
    case 'B':
      return lexer_read_binary_integer(lex);
    }

  return lexer_read_decimal_number(lex);
}

token_t* lexer_read_string(lexer_t* lex)
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

        if (!isxdigit(lexer_current(lex)))
        {
          location_set_len(token_get_loc(tok), 2);
          location_set_ptr(lex->loc, location_get_ptr(lex->loc) - 2);
          report_error_missing_hex_digits_in_escape_sequence(token_get_loc(tok));
        }

        len += lexer_skip(lex, lexer_is_hexadecimal);
        break;

      default:
        location_set_len(token_get_loc(tok), 2);
        location_set_ptr(lex->loc, location_get_ptr(lex->loc) - 2);
        report_error_unknown_escape_sequence(token_get_loc(tok));
      }
    }
  }

  if (ch != '"')
  {
    location_set_len(token_get_loc(tok), 1);
    report_error_missing_terminating_double_quotes(token_get_loc(tok));
  }

  location_set_len(token_get_loc(tok), len + 2);

  return tok;
}

token_t* lexer_read_character(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_CHAR);

  lexer_next(lex);

  size_t len = 0;

  if (lexer_current(lex) == '\'')
  {
    location_set_len(token_get_loc(tok), 2);
    report_error_empty_character_literal(token_get_loc(tok));
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
        location_set_len(token_get_loc(tok), 2);
        location_set_ptr(lex->loc, location_get_ptr(lex->loc) - 2);
        report_error_missing_hex_digits_in_escape_sequence(token_get_loc(tok));
      }

      len += lexer_skip(lex, lexer_is_hexadecimal);

      if (len > 4)
      {
        location_set_len(token_get_loc(tok), len);
        location_set_ptr(lex->loc, location_get_ptr(lex->loc) - len);
        report_error_too_many_hex_digits_in_escape_sequence(token_get_loc(tok));
      }
      break;

    default:
      location_set_len(token_get_loc(tok), 2);
      location_set_ptr(lex->loc, location_get_ptr(lex->loc) - 2);
      report_error_unknown_escape_sequence(token_get_loc(tok));
    }
  }
  else
    ++len;

  if (lexer_next(lex) != '\'')
  {
    location_set_len(token_get_loc(tok), 1);
    report_error_missing_terminating_single_quote(token_get_loc(tok));
  }

  location_set_len(token_get_loc(tok), len + 2);

  return tok;
}

token_t* lexer_read_punctuation(lexer_t* lex)
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
  location_set_len(token_get_loc(tok), 1);
  
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
    report_error_unexpected_character(token_get_loc(tok));

  token_set_kind(tok, kind);
  location_set_len(token_get_loc(tok), 0);

  for (size_t i = 0; i < countof(lookup); ++i)
    if (lookup[i].kind == kind)
    {
      location_set_len(token_get_loc(tok), lookup[i].len);
      break;
    }

  return tok;
}

token_t* lexer_read_next(lexer_t* lex)
{
  lexer_skip(lex, lexer_is_space);

  if (lexer_is_word_begin(lex))
    return lexer_read_word(lex);
  else if (lexer_is_decimal(lex))
    return lexer_read_number(lex);
  else if (lexer_current(lex) == '"')
    return lexer_read_string(lex);
  else if (lexer_current(lex) == '\'')
    return lexer_read_character(lex);
  else if (lexer_is_punctuation(lex))
    return lexer_read_punctuation(lex);
  else if (lexer_current(lex) == '\0')
    return lexer_token_init(lex, TOK_EOF);
  
  report_error_unexpected_character(lex->loc);

  return NULL;
}

void lexer_lex(lexer_t* lex, list_t* toks)
{
  while (list_empty(toks) || token_get_kind((token_t*)list_back(toks)) != TOK_EOF)
    list_push_back(toks, lexer_read_next(lex));
}
