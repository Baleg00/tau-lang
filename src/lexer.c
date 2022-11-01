#include "lexer.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "crumb.h"

lexer_t* lexer_init(const char* filepath)
{
  lexer_t* lex = (lexer_t*)malloc(sizeof(lexer_t));
  assert(lex != NULL);

  lex->filepath = filepath;
  lex->row = 0;
  lex->col = 0;

  FILE* file = fopen(filepath, "r");
  
  if (file == NULL)
  {
    log_fatal("lexer", "Cannot open file: %s", filepath);
    exit(EXIT_FAILURE);
  }

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

       if (len == 2 && strncmp(begin, "is"      , len) == 0) tok->kind = TOK_KW_IS;
  else if (len == 2 && strncmp(begin, "as"      , len) == 0) tok->kind = TOK_KW_AS;
  else if (len == 6 && strncmp(begin, "sizeof"  , len) == 0) tok->kind = TOK_KW_SIZEOF;
  else if (len == 7 && strncmp(begin, "alignof" , len) == 0) tok->kind = TOK_KW_ALIGNOF;
  else if (len == 6 && strncmp(begin, "typeof"  , len) == 0) tok->kind = TOK_KW_TYPEOF;
  else if (len == 2 && strncmp(begin, "in"      , len) == 0) tok->kind = TOK_KW_IN;
  else if (len == 3 && strncmp(begin, "var"     , len) == 0) tok->kind = TOK_KW_VAR;
  else if (len == 3 && strncmp(begin, "fun"     , len) == 0) tok->kind = TOK_KW_FUN;
  else if (len == 3 && strncmp(begin, "gen"     , len) == 0) tok->kind = TOK_KW_GEN;
  else if (len == 6 && strncmp(begin, "struct"  , len) == 0) tok->kind = TOK_KW_STRUCT;
  else if (len == 5 && strncmp(begin, "union"   , len) == 0) tok->kind = TOK_KW_UNION;
  else if (len == 4 && strncmp(begin, "enum"    , len) == 0) tok->kind = TOK_KW_ENUM;
  else if (len == 3 && strncmp(begin, "mod"     , len) == 0) tok->kind = TOK_KW_MOD;
  else if (len == 3 && strncmp(begin, "use"     , len) == 0) tok->kind = TOK_KW_USE;
  else if (len == 4 && strncmp(begin, "from"    , len) == 0) tok->kind = TOK_KW_FROM;
  else if (len == 2 && strncmp(begin, "if"      , len) == 0) tok->kind = TOK_KW_IF;
  else if (len == 4 && strncmp(begin, "then"    , len) == 0) tok->kind = TOK_KW_THEN;
  else if (len == 4 && strncmp(begin, "else"    , len) == 0) tok->kind = TOK_KW_ELSE;
  else if (len == 4 && strncmp(begin, "elif"    , len) == 0) tok->kind = TOK_KW_ELIF;
  else if (len == 3 && strncmp(begin, "for"     , len) == 0) tok->kind = TOK_KW_FOR;
  else if (len == 5 && strncmp(begin, "while"   , len) == 0) tok->kind = TOK_KW_WHILE;
  else if (len == 4 && strncmp(begin, "when"    , len) == 0) tok->kind = TOK_KW_WHEN;
  else if (len == 2 && strncmp(begin, "do"      , len) == 0) tok->kind = TOK_KW_DO;
  else if (len == 5 && strncmp(begin, "break"   , len) == 0) tok->kind = TOK_KW_BREAK;
  else if (len == 8 && strncmp(begin, "continue", len) == 0) tok->kind = TOK_KW_CONTINUE;
  else if (len == 6 && strncmp(begin, "return"  , len) == 0) tok->kind = TOK_KW_RETURN;
  else if (len == 5 && strncmp(begin, "yield"   , len) == 0) tok->kind = TOK_KW_YIELD;
  else if (len == 3 && strncmp(begin, "pub"     , len) == 0) tok->kind = TOK_KW_PUB;
  else if (len == 3 && strncmp(begin, "mut"     , len) == 0) tok->kind = TOK_KW_MUT;
  else if (len == 5 && strncmp(begin, "const"   , len) == 0) tok->kind = TOK_KW_CONST;
  else if (len == 6 && strncmp(begin, "static"  , len) == 0) tok->kind = TOK_KW_STATIC;
  else if (len == 2 && strncmp(begin, "i8"      , len) == 0) tok->kind = TOK_KW_I8;
  else if (len == 3 && strncmp(begin, "i16"     , len) == 0) tok->kind = TOK_KW_I16;
  else if (len == 3 && strncmp(begin, "i32"     , len) == 0) tok->kind = TOK_KW_I32;
  else if (len == 3 && strncmp(begin, "i64"     , len) == 0) tok->kind = TOK_KW_I64;
  else if (len == 4 && strncmp(begin, "isize"   , len) == 0) tok->kind = TOK_KW_ISIZE;
  else if (len == 2 && strncmp(begin, "u8"      , len) == 0) tok->kind = TOK_KW_U8;
  else if (len == 3 && strncmp(begin, "u16"     , len) == 0) tok->kind = TOK_KW_U16;
  else if (len == 3 && strncmp(begin, "u32"     , len) == 0) tok->kind = TOK_KW_U32;
  else if (len == 3 && strncmp(begin, "u64"     , len) == 0) tok->kind = TOK_KW_U64;
  else if (len == 5 && strncmp(begin, "usize"   , len) == 0) tok->kind = TOK_KW_USIZE;
  else if (len == 3 && strncmp(begin, "f32"     , len) == 0) tok->kind = TOK_KW_F32;
  else if (len == 3 && strncmp(begin, "f64"     , len) == 0) tok->kind = TOK_KW_F64;
  else if (len == 4 && strncmp(begin, "bool"    , len) == 0) tok->kind = TOK_KW_BOOL;
  else if (len == 4 && strncmp(begin, "unit"    , len) == 0) tok->kind = TOK_KW_UNIT;
  else if (len == 4 && strncmp(begin, "true"    , len) == 0) tok->kind = TOK_LIT_BOOL_TRUE;
  else if (len == 5 && strncmp(begin, "false"   , len) == 0) tok->kind = TOK_LIT_BOOL_FALSE;
  else if (len == 4 && strncmp(begin, "null"    , len) == 0) tok->kind = TOK_LIT_NULL;
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

  if (len == 0 || lexer_is_word(lex))
  {
    crumb_error(&tok->loc, "Ill-formed octal integer!");
    exit(EXIT_FAILURE);
  }

  tok->lit_int.value = strtoull(lex->cur - len, NULL, 8);

  lexer_token_push(lex, tok);
}

void lexer_read_binary_integer(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT_BIN);

  size_t len = lexer_skip(lex, lexer_is_binary);
  tok->loc.len = len;

  if (len == 0 || lexer_is_word(lex))
  {
    crumb_error(&tok->loc, "Ill-formed binary integer!");
    exit(EXIT_FAILURE);
  }

  tok->lit_int.value = strtoull(lex->cur - len, NULL, 2);

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
      tok->lit_int.value = strtoull(lex->cur - len, NULL, 10);
      tok->loc.len = len;
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

    tok->lit_flt.value = strtold(lex->cur - len, NULL);
    tok->loc.len = len;

    if (lexer_is_word(lex))
    {
      crumb_error(&tok->loc, "Ill-formed decimal float!");
      exit(EXIT_FAILURE);
    }

    lexer_token_push(lex, tok);
  }
  else
  {
    tok->lit_int.value = strtoull(lex->cur - len, NULL, 10);
    tok->loc.len = len;

    if (lexer_is_word(lex))
    {
      crumb_error(&tok->loc, "Ill-formed decimal integer!");
      exit(EXIT_FAILURE);
    }

    lexer_token_push(lex, tok);
  }
}

void lexer_read_hexadecimal_number(lexer_t* lex)
{
  token_t* tok = lexer_token_init(lex, TOK_LIT_INT_HEX);

  size_t len = lexer_skip(lex, lexer_is_hexadecimal);

  if (lexer_current(lex) == '.')
  {
    if (!isxdigit(lexer_peek(lex)))
    {
      tok->lit_int.value = strtoull(lex->cur - len, NULL, 16);
      tok->loc.len = len;
      lexer_token_push(lex, tok);

      lexer_read_punctuation(lex);
      return;
    }

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
    tok->loc.len = len;

    if (lexer_is_word(lex))
    {
      crumb_error(&tok->loc, "Ill-formed hexadecimal float!");
      exit(EXIT_FAILURE);
    }

    lexer_token_push(lex, tok);
  }
  else
  {
    tok->lit_int.value = strtoull(lex->cur - len, NULL, 16);
    tok->loc.len = len;

    if (lexer_is_word(lex))
    {
      crumb_error(&tok->loc, "Ill-formed hexadecimal integer!");
      exit(EXIT_FAILURE);
    }

    lexer_token_push(lex, tok);
  }
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
        crumb_error(&tok->loc, "Illegal escape sequence in string!");
        exit(EXIT_FAILURE);
      }
    }
  }

  if (ch != '"')
  {
    crumb_error(&tok->loc, "Missing string closing quotation mark!");
    exit(EXIT_FAILURE);
  }

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

  if (lexer_current(lex) == '\'')
  {
    crumb_error(&tok->loc, "Empty character!");
    exit(EXIT_FAILURE);
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
      {
        crumb_error(&tok->loc, "Illegal character in hexadecimal byte escape sequence!");
        exit(EXIT_FAILURE);
      }

      ++len;
      if (isxdigit(lexer_current(lex)))
      {
        lexer_next(lex);
        ++len;
      }
      break;

    default:
      crumb_error(&tok->loc, "Illegal escape sequence!");
      exit(EXIT_FAILURE);
    }
  }
  else
    ++len;

  tok->loc.len = len;

  if (lexer_next(lex) != '\'')
  {
    crumb_error(&tok->loc, "Missing character closing apostrophe!");
    exit(EXIT_FAILURE);
  }

  tok->lit_char.value = malloc((len + 1) * sizeof(char));  
  assert(tok->lit_char.value != NULL);

  memcpy(tok->lit_char.value, lex->cur - len - 2, len * sizeof(char));
  tok->lit_char.value[len] = '\0';
  tok->lit_char.len = len;

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
  {
    crumb_error(&tok->loc, "Unknown punctuation!");
    exit(EXIT_FAILURE);
  }

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
  {
    token_t* tok = lexer_token_init(lex, TOK_UNKNOWN);
    crumb_error(&tok->loc, "Unexpected character!");
    exit(EXIT_FAILURE);
  }
}

void lexer_lex(lexer_t* lex)
{
  while (list_empty(lex->toks) || ((token_t*)list_back(lex->toks))->kind != TOK_EOF)
    lexer_read_next(lex);
}
