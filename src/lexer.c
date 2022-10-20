#include "lexer.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

lexer_t* lexer_init(const char* filepath)
{
  lexer_t* lex = (lexer_t*)malloc(sizeof(lexer_t));
  assert(lex != NULL);

  lex->filepath = filepath;
  lex->row = 0;
  lex->col = 0;

  FILE* file = fopen(filepath, "r");
  assert(file != NULL);

  fseek(file, 0, SEEK_END);
  size_t len = (size_t)ftell(file);
  fseek(file, 0, SEEK_SET);

  lex->src = (char*)calloc(len + 1, sizeof(char));
  assert(lex->src != NULL);

  fread(lex->src, sizeof(char), len, file);

  fclose(file);

  lex->cur = lex->src;

  lex->toks = list_init();

  return lex;
}

void lexer_free(lexer_t* lex)
{
  for (list_elem_t* elem = list_front_elem(lex->toks); elem != NULL; elem = list_elem_next(elem))
    token_free((token_t*)list_elem_data(elem));

  list_free(lex->toks);
  free(lex);
}

token_t* lexer_token_init(lexer_t* lex, token_kind_t kind)
{
  token_t* tok = token_init(kind);

  tok->loc.filepath = lex->filepath;
  tok->loc.src = lex->src;
  tok->loc.cur = lex->cur;
  tok->loc.row = lex->row;
  tok->loc.col = lex->col;

  return tok;
}

void lexer_token_push(lexer_t* lex, token_t* tok)
{
  list_push_back(lex->toks, tok);
}

int lexer_is_space(lexer_t* lex)
{
  return isspace(*lex->cur);
}

int lexer_is_word_begin(lexer_t* lex)
{
  return isalpha(*lex->cur) || *lex->cur == '_';
}

int lexer_is_word(lexer_t* lex)
{
  return isalnum(*lex->cur) || *lex->cur == '_';
}

int lexer_is_decimal(lexer_t* lex)
{
  return isdigit(*lex->cur);
}

int lexer_is_hexadecimal(lexer_t* lex)
{
  return isxdigit(*lex->cur);
}

int lexer_is_octal(lexer_t* lex)
{
  return '0' <= *lex->cur && '7' >= *lex->cur;
}

int lexer_is_binary(lexer_t* lex)
{
  return *lex->cur == '0' || *lex->cur == '1';
}

int lexer_is_punctuation(lexer_t* lex)
{
  return ispunct(*lex->cur);
}

char lexer_current(lexer_t* lex)
{
  return *lex->cur;
}

char lexer_next(lexer_t* lex)
{
  if (*lex->cur == '\0')
    return '\0';
  
  if (*lex->cur == '\n')
    {
      ++lex->row;
      lex->col = 0;
    }
    else
      ++lex->col;

  return *(lex->cur++);
}

char lexer_peek(lexer_t* lex)
{
  return *lex->cur == '\0' ? '\0' : *(lex->cur + 1);
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
  const char* begin = lex->cur;

  while (*lex->cur != '\0' && pred(lex))
    lexer_next(lex);

  return (size_t)(lex->cur - begin);
}

void lexer_skip_n(lexer_t* lex, size_t n)
{
  while (lexer_current(lex) != '\0' && n-- > 0)
    lexer_next(lex);
}

void lexer_read_word(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_UNKNOWN);

  char* begin = lex->cur;
  size_t len = lexer_skip(lex, lexer_is_word);
  tok->loc.len = len;

       if (strncmp(begin, "is"     , len) == 0) tok->kind = TOK_KW_IS;
  else if (strncmp(begin, "as"     , len) == 0) tok->kind = TOK_KW_AS;
  else if (strncmp(begin, "sizeof" , len) == 0) tok->kind = TOK_KW_SIZEOF;
  else if (strncmp(begin, "alignof", len) == 0) tok->kind = TOK_KW_ALIGNOF;
  else if (strncmp(begin, "typeof" , len) == 0) tok->kind = TOK_KW_TYPEOF;
  else if (strncmp(begin, "in"     , len) == 0) tok->kind = TOK_KW_IN;
  else if (strncmp(begin, "var"    , len) == 0) tok->kind = TOK_KW_VAR;
  else if (strncmp(begin, "fun"    , len) == 0) tok->kind = TOK_KW_FUN;
  else if (strncmp(begin, "gen"    , len) == 0) tok->kind = TOK_KW_GEN;
  else if (strncmp(begin, "struct" , len) == 0) tok->kind = TOK_KW_STRUCT;
  else if (strncmp(begin, "union"  , len) == 0) tok->kind = TOK_KW_UNION;
  else if (strncmp(begin, "enum"   , len) == 0) tok->kind = TOK_KW_ENUM;
  else if (strncmp(begin, "mod"    , len) == 0) tok->kind = TOK_KW_MOD;
  else if (strncmp(begin, "use"    , len) == 0) tok->kind = TOK_KW_USE;
  else if (strncmp(begin, "from"   , len) == 0) tok->kind = TOK_KW_FROM;
  else if (strncmp(begin, "if"     , len) == 0) tok->kind = TOK_KW_IF;
  else if (strncmp(begin, "then"   , len) == 0) tok->kind = TOK_KW_THEN;
  else if (strncmp(begin, "else"   , len) == 0) tok->kind = TOK_KW_ELSE;
  else if (strncmp(begin, "elif"   , len) == 0) tok->kind = TOK_KW_ELIF;
  else if (strncmp(begin, "for"    , len) == 0) tok->kind = TOK_KW_FOR;
  else if (strncmp(begin, "while"  , len) == 0) tok->kind = TOK_KW_WHILE;
  else if (strncmp(begin, "when"   , len) == 0) tok->kind = TOK_KW_WHEN;
  else if (strncmp(begin, "do"     , len) == 0) tok->kind = TOK_KW_DO;
  else if (strncmp(begin, "return" , len) == 0) tok->kind = TOK_KW_RETURN;
  else if (strncmp(begin, "yield"  , len) == 0) tok->kind = TOK_KW_YIELD;
  else if (strncmp(begin, "pub"    , len) == 0) tok->kind = TOK_KW_PUB;
  else if (strncmp(begin, "mut"    , len) == 0) tok->kind = TOK_KW_MUT;
  else if (strncmp(begin, "const"  , len) == 0) tok->kind = TOK_KW_CONST;
  else if (strncmp(begin, "static" , len) == 0) tok->kind = TOK_KW_STATIC;
  else if (strncmp(begin, "i8"     , len) == 0) tok->kind = TOK_KW_I8;
  else if (strncmp(begin, "i16"    , len) == 0) tok->kind = TOK_KW_I16;
  else if (strncmp(begin, "i32"    , len) == 0) tok->kind = TOK_KW_I32;
  else if (strncmp(begin, "i64"    , len) == 0) tok->kind = TOK_KW_I64;
  else if (strncmp(begin, "isize"  , len) == 0) tok->kind = TOK_KW_ISIZE;
  else if (strncmp(begin, "u8"     , len) == 0) tok->kind = TOK_KW_U8;
  else if (strncmp(begin, "u16"    , len) == 0) tok->kind = TOK_KW_U16;
  else if (strncmp(begin, "u32"    , len) == 0) tok->kind = TOK_KW_U32;
  else if (strncmp(begin, "u64"    , len) == 0) tok->kind = TOK_KW_U64;
  else if (strncmp(begin, "usize"  , len) == 0) tok->kind = TOK_KW_USIZE;
  else if (strncmp(begin, "f32"    , len) == 0) tok->kind = TOK_KW_F32;
  else if (strncmp(begin, "f64"    , len) == 0) tok->kind = TOK_KW_F64;
  else if (strncmp(begin, "bool"   , len) == 0) tok->kind = TOK_KW_BOOL;
  else if (strncmp(begin, "unit"   , len) == 0) tok->kind = TOK_KW_UNIT;
  else if (strncmp(begin, "true"   , len) == 0) tok->kind = TOK_LIT_BOOL_TRUE;
  else if (strncmp(begin, "false"  , len) == 0) tok->kind = TOK_LIT_BOOL_FALSE;
  else if (strncmp(begin, "null"   , len) == 0) tok->kind = TOK_LIT_NULL;
  else
  {
    tok->kind = TOK_ID;
    tok->id.len = len;
    tok->id.value = malloc((len + 1) * sizeof(char));
    assert(tok->id.value != NULL);

    memcpy(tok->id.value, lex->cur - len, len * sizeof(char));
    tok->id.value[len] = '\0';
  }

  lexer_token_push(lex, tok);
}

void lexer_read_octal_integer(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT_OCT);

  size_t len = lexer_skip(lex, lexer_is_octal);
  tok->loc.len = len;

  assert(len != 0 && !lexer_is_word(lex));

  tok->lit_int.value = strtoull(lex->cur - len, NULL, 8);

  lexer_token_push(lex, tok);
}

void lexer_read_binary_integer(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT_BIN);

  size_t len = lexer_skip(lex, lexer_is_binary);
  tok->loc.len = len;

  assert(len != 0 && !lexer_is_word(lex));

  tok->lit_int.value = strtoull(lex->cur - len, NULL, 2);

  lexer_token_push(lex, tok);
}

void lexer_read_decimal_number(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT_DEC);

  size_t len = lexer_skip(lex, lexer_is_decimal);

  if (lexer_current(lex) == '.')
  {
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

    tok->lit_flt.value = strtold(lex->cur - len, NULL);
  }
  else
    tok->lit_int.value = strtoull(lex->cur - len, NULL, 10);

  assert(!lexer_is_word(lex));

  tok->loc.len = len;

  lexer_token_push(lex, tok);
}

void lexer_read_hexadecimal_number(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT_HEX);

  size_t len = lexer_skip(lex, lexer_is_hexadecimal);

  if (lexer_current(lex) == '.')
  {
    tok->kind = TOK_LIT_FLT_HEX;

    lexer_next(lex);
    ++len;

    len += lexer_skip(lex, lexer_is_hexadecimal);

    if (lexer_current(lex) == 'p' || lexer_current(lex) == 'P')
    {
      lexer_next(lex);
      ++len;

      if (lexer_current(lex) == '+' || lexer_current(lex) == '-')
      {
        lexer_next(lex);
        ++len;
      }

      len += lexer_skip(lex, lexer_is_hexadecimal);
    }

    tok->lit_flt.value = strtold(lex->cur - len, NULL);
  }
  else
    tok->lit_int.value = strtoull(lex->cur - len, NULL, 16);

  assert(!lexer_is_word(lex));

  tok->loc.len = len;

  lexer_token_push(lex, tok);
}

void lexer_read_number(lexer_t* lex)
{
  if (lexer_current(lex) == '0')
    switch (lexer_peek(lex))
    {
    case 'x':
    case 'X':
      lexer_skip_n(lex, 2);
      lexer_read_hexadecimal_number(lex);
      return;

    case 'o':
    case 'O':
      lexer_skip_n(lex, 2);
      lexer_read_octal_integer(lex);
      return;

    case 'b':
    case 'B':
      lexer_skip_n(lex, 2);
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
        break;
      
      case 'x':
      case 'X': // arbitrary hexadecimal bytes
        len += lexer_skip(lex, lexer_is_hexadecimal);
        break;

      default:
        assert(false);
      }
    }
  }

  assert(ch == '"');

  tok->lit_str.value = malloc((len + 1) * sizeof(char));
  assert(tok->lit_str.value != NULL);

  memcpy(tok->lit_str.value, lex->cur - len - 2, len * sizeof(char));
  tok->lit_str.value[len] = '\0';
  tok->lit_str.len = len;

  tok->loc.len = len;

  lexer_token_push(lex, tok);
}

void lexer_read_character(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_CHAR);
  
  lexer_next(lex);

  size_t len = 0;

  assert(lexer_current(lex) != '\'');

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

      assert(isxdigit(lexer_next(lex)));
      
      ++len;
      if (isxdigit(lexer_current(lex)))
      {
        lexer_next(lex);
        ++len;
      }
      break;

    default:
      assert(false);
    }
  }
  else
    ++len;

  assert(lexer_next(lex) == '\'');

  tok->lit_char.value = malloc((len + 1) * sizeof(char));
  assert(tok->lit_char.value != NULL);

  memcpy(tok->lit_char.value, lex->cur - len - 2, len * sizeof(char));
  tok->lit_char.value[len] = '\0';
  tok->lit_char.len = len;

  tok->loc.len = len;

  lexer_token_push(lex, tok);
}

void lexer_read_punctuation(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_UNKNOWN);

  if (lexer_consume(lex, '+'))
    if (lexer_consume(lex, '+'))
      tok->kind = TOK_PUNCT_PLUS_PLUS;
    else if (lexer_consume(lex, '='))
      tok->kind = TOK_PUNCT_PLUS_EQUAL;
    else
      tok->kind = TOK_PUNCT_PLUS;
  else if (lexer_consume(lex, '-'))
    if (lexer_consume(lex, '-'))
      tok->kind = TOK_PUNCT_MINUS_MINUS;
    else if (lexer_consume(lex, '='))
      tok->kind = TOK_PUNCT_MINUS_EQUAL;
    else
      tok->kind = TOK_PUNCT_MINUS;
  else if (lexer_consume(lex, '*'))
    if (lexer_consume(lex, '.'))
      tok->kind = TOK_PUNCT_ASTERISK_DOT;
    else if (lexer_consume(lex, '='))
      tok->kind = TOK_PUNCT_ASTERISK_EQUAL;
    else
      tok->kind = TOK_PUNCT_ASTERISK;
  else if (lexer_consume(lex, '/'))
    if (lexer_consume(lex, '='))
      tok->kind = TOK_PUNCT_SLASH_EQUAL;
    else
      tok->kind = TOK_PUNCT_SLASH;
  else if (lexer_consume(lex, '%'))
    if (lexer_consume(lex, '='))
      tok->kind = TOK_PUNCT_PERCENT_EQUAL;
    else
      tok->kind = TOK_PUNCT_PERCENT;
  else if (lexer_consume(lex, '&'))
    if (lexer_consume(lex, '&'))
      tok->kind = TOK_PUNCT_AMPERSAND_AMPERSAND;
    else if (lexer_consume(lex, '='))
      tok->kind = TOK_PUNCT_AMPERSAND_EQUAL;
    else
      tok->kind = TOK_PUNCT_AMPERSAND;
  else if (lexer_consume(lex, '|'))
    if (lexer_consume(lex, '|'))
      tok->kind = TOK_PUNCT_BAR_BAR;
    else if (lexer_consume(lex, '='))
      tok->kind = TOK_PUNCT_BAR_EQUAL;
    else
      tok->kind = TOK_PUNCT_BAR;
  else if (lexer_consume(lex, '^'))
    if (lexer_consume(lex, '='))
      tok->kind = TOK_PUNCT_HAT_EQUAL;
    else
      tok->kind = TOK_PUNCT_HAT;
  else if (lexer_consume(lex, '~'))
    tok->kind = TOK_PUNCT_TILDE;
  else if (lexer_consume(lex, '<'))
    if (lexer_consume(lex, '<'))
      if (lexer_consume(lex, '='))
        tok->kind = TOK_PUNCT_LESS_LESS_EQUAL;
      else
        tok->kind = TOK_PUNCT_LESS_LESS;
    else if (lexer_consume(lex, '='))
      tok->kind = TOK_PUNCT_LESS_EQUAL;
    else
      tok->kind = TOK_PUNCT_LESS;
  else if (lexer_consume(lex, '>'))
    if (lexer_consume(lex, '>'))
      if (lexer_consume(lex, '='))
        tok->kind = TOK_PUNCT_GREATER_GREATER_EQUAL;
      else
        tok->kind = TOK_PUNCT_GREATER_GREATER;
    else if (lexer_consume(lex, '='))
      tok->kind = TOK_PUNCT_GREATER_EQUAL;
    else
      tok->kind = TOK_PUNCT_GREATER;
  else if (lexer_consume(lex, '!'))
    if (lexer_consume(lex, '='))
      tok->kind = TOK_PUNCT_BANG_EQUAL;
    else
      tok->kind = TOK_PUNCT_BANG;
  else if (lexer_consume(lex, '.'))
    if (lexer_consume(lex, '.'))
      if (lexer_consume(lex, '.'))
        tok->kind = TOK_PUNCT_DOT_DOT_DOT;
      else
        tok->kind = TOK_PUNCT_DOT_DOT;
    else
      tok->kind = TOK_PUNCT_DOT;
  else if (lexer_consume(lex, '?'))
    if (lexer_consume(lex, '.'))
      tok->kind = TOK_PUNCT_QUESTION_DOT;
    else
      tok->kind = TOK_PUNCT_QUESTION;
  else if (lexer_consume(lex, '='))
    if (lexer_consume(lex, '='))
      tok->kind = TOK_PUNCT_EQUAL_EQUAL;
    else
      tok->kind = TOK_PUNCT_EQUAL;
  else if (lexer_consume(lex, ','))
    tok->kind = TOK_PUNCT_COMMA;
  else if (lexer_consume(lex, ':'))
    tok->kind = TOK_PUNCT_COLON;
  else if (lexer_consume(lex, '('))
    tok->kind = TOK_PUNCT_PAREN_LEFT;
  else if (lexer_consume(lex, ')'))
    tok->kind = TOK_PUNCT_PAREN_RIGHT;
  else if (lexer_consume(lex, '['))
    tok->kind = TOK_PUNCT_BRACKET_LEFT;
  else if (lexer_consume(lex, ']'))
    tok->kind = TOK_PUNCT_BRACKET_RIGHT;
  else if (lexer_consume(lex, '{'))
    tok->kind = TOK_PUNCT_BRACE_LEFT;
  else if (lexer_consume(lex, '}'))
    tok->kind = TOK_PUNCT_BRACE_RIGHT;
  else
    assert(false);

  tok->loc.len = 1;

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
    assert(false);
}

void lexer_lex(lexer_t* lex)
{
  while (list_empty(lex->toks) || ((token_t*)list_back(lex->toks))->kind != TOK_EOF)
    lexer_read_next(lex);
}
