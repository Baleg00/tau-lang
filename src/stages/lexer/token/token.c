/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/lexer/token/token.h"

#include <ctype.h>

#include "stages/lexer/token/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

static const struct
{
  token_kind_t kind;
  size_t len;
} TOKEN_LEN_LOOKUP[] = {
  { TOK_UNKNOWN, 0 },
  { TOK_LIT_NULL, 4 },
  { TOK_KW_IS, 2 },
  { TOK_KW_AS, 2 },
  { TOK_KW_SIZEOF, 6 },
  { TOK_KW_ALIGNOF, 7 },
  { TOK_KW_USE, 3 },
  { TOK_KW_IN, 2 },
  { TOK_KW_PUB, 3 },
  { TOK_KW_EXTERN, 6 },
  { TOK_KW_FUN, 3 },
  { TOK_KW_STRUCT, 6 },
  { TOK_KW_UNION, 5 },
  { TOK_KW_ENUM, 4 },
  { TOK_KW_MOD, 3 },
  { TOK_KW_IF, 2 },
  { TOK_KW_THEN, 4 },
  { TOK_KW_ELSE, 4 },
  { TOK_KW_FOR, 3 },
  { TOK_KW_WHILE, 5 },
  { TOK_KW_DO, 2 },
  { TOK_KW_BREAK, 5 },
  { TOK_KW_CONTINUE, 8 },
  { TOK_KW_RETURN, 6 },
  { TOK_KW_DEFER, 5 },
  { TOK_KW_MUT, 3 },
  { TOK_KW_I8, 2 },
  { TOK_KW_I16, 3 },
  { TOK_KW_I32, 3 },
  { TOK_KW_I64, 3 },
  { TOK_KW_ISIZE, 5 },
  { TOK_KW_U8, 2 },
  { TOK_KW_U16, 3 },
  { TOK_KW_U32, 3 },
  { TOK_KW_U64, 3 },
  { TOK_KW_USIZE, 5 },
  { TOK_KW_F32, 3 },
  { TOK_KW_F64, 3 },
  { TOK_KW_CHAR, 4 },
  { TOK_KW_BOOL, 4 },
  { TOK_KW_UNIT, 4 },
  { TOK_PUNCT_PLUS, 1 },
  { TOK_PUNCT_PLUS_PLUS, 2 },
  { TOK_PUNCT_PLUS_EQUAL, 2 },
  { TOK_PUNCT_MINUS, 1 },
  { TOK_PUNCT_MINUS_MINUS, 2 },
  { TOK_PUNCT_MINUS_EQUAL, 2 },
  { TOK_PUNCT_ASTERISK, 1 },
  { TOK_PUNCT_ASTERISK_EQUAL, 2 },
  { TOK_PUNCT_ASTERISK_DOT, 2 },
  { TOK_PUNCT_SLASH, 1 },
  { TOK_PUNCT_SLASH_EQUAL, 2 },
  { TOK_PUNCT_PERCENT, 1 },
  { TOK_PUNCT_PERCENT_EQUAL, 2 },
  { TOK_PUNCT_AMPERSAND, 1 },
  { TOK_PUNCT_AMPERSAND_AMPERSAND, 2 },
  { TOK_PUNCT_AMPERSAND_EQUAL, 2 },
  { TOK_PUNCT_BAR, 1 },
  { TOK_PUNCT_BAR_BAR, 1 },
  { TOK_PUNCT_BAR_EQUAL, 2 },
  { TOK_PUNCT_HAT, 1 },
  { TOK_PUNCT_HAT_EQUAL, 2 },
  { TOK_PUNCT_TILDE, 1 },
  { TOK_PUNCT_LESS, 1 },
  { TOK_PUNCT_LESS_LESS, 2 },
  { TOK_PUNCT_LESS_LESS_EQUAL, 3 },
  { TOK_PUNCT_LESS_EQUAL, 2 },
  { TOK_PUNCT_GREATER, 1 },
  { TOK_PUNCT_GREATER_GREATER, 2 },
  { TOK_PUNCT_GREATER_GREATER_EQUAL, 3 },
  { TOK_PUNCT_GREATER_EQUAL, 2 },
  { TOK_PUNCT_BANG, 1 },
  { TOK_PUNCT_BANG_EQUAL, 2 },
  { TOK_PUNCT_DOT, 1 },
  { TOK_PUNCT_DOT_DOT, 2 },
  { TOK_PUNCT_DOT_DOT_DOT, 3 },
  { TOK_PUNCT_QUESTION, 1 },
  { TOK_PUNCT_QUESTION_DOT, 2 },
  { TOK_PUNCT_EQUAL, 1 },
  { TOK_PUNCT_EQUAL_EQUAL, 2 },
  { TOK_PUNCT_COMMA, 1 },
  { TOK_PUNCT_COLON, 1 },
  { TOK_PUNCT_SEMICOLON, 1 },
  { TOK_PUNCT_PAREN_LEFT, 1 },
  { TOK_PUNCT_PAREN_RIGHT, 1 },
  { TOK_PUNCT_BRACKET_LEFT, 1 },
  { TOK_PUNCT_BRACKET_RIGHT, 1 },
  { TOK_PUNCT_BRACE_LEFT, 1 },
  { TOK_PUNCT_BRACE_RIGHT, 1 },
  { TOK_PUNCT_HASH, 1 },
  { TOK_NEWLINE, 0 },
  { TOK_EOF, 0 },
};

static size_t token_len_lit_int_hex(const char* src, size_t pos)
{
  size_t begin = pos;

  pos += 2;

  while (isxdigit(src[pos]))
    pos++;

  if (src[pos] == 'i' || src[pos] == 'u')
  {
    pos++;

    while (isdigit(src[pos]))
      pos++;
  }

  return pos - begin;
}

static size_t token_len_lit_int_oct(const char* src, size_t pos)
{
  size_t begin = pos;

  pos += 2;

  while (src[pos] >= '0' && src[pos] <= '7')
    pos++;

  if (src[pos] == 'i' || src[pos] == 'u')
  {
    pos++;

    while (isdigit(src[pos]))
      pos++;
  }

  return pos - begin;
}

static size_t token_len_lit_int_bin(const char* src, size_t pos)
{
  size_t begin = pos;

  pos += 2;

  while (src[pos] == '0' || src[pos] == '1')
    pos++;

  if (src[pos] == 'i' || src[pos] == 'u')
  {
    pos++;

    while (isdigit(src[pos]))
      pos++;
  }

  return pos - begin;
}

static size_t token_len_lit_int_dec(const char* src, size_t pos)
{
  size_t begin = pos;

  pos += 2;

  while (isdigit(src[pos]))
    pos++;

  if (src[pos] == 'i' || src[pos] == 'u')
  {
    pos++;

    while (isdigit(src[pos]))
      pos++;
  }

  return pos - begin;
}

static size_t token_len_id(const char* src, size_t pos)
{
  size_t begin = pos;

  while (isalnum(src[pos]) || src[pos] == '_')
    pos++;

  return pos - begin;
}

static size_t token_len_lit_int(const char* src, size_t pos)
{
  if (src[pos] == '0')
  {
    if (src[pos + 1] == 'x' || src[pos + 1] == 'X')
      return token_len_lit_int_hex(src, pos);

    if (src[pos + 1] == 'o' || src[pos + 1] == 'O')
      return token_len_lit_int_oct(src, pos);

    if (src[pos + 1] == 'b' || src[pos + 1] == 'B')
      return token_len_lit_int_bin(src, pos);
  }

  return token_len_lit_int_dec(src, pos);
}

static size_t token_len_lit_flt(const char* src, size_t pos)
{
  size_t begin = pos;

  while (isdigit(src[pos]))
    pos++;

  if (src[pos] == '.')
    pos++;

  while (isdigit(src[pos]))
    pos++;

  if (src[pos] == 'e' || src[pos] == 'E')
  {
    pos++;

    if (src[pos] == '+' || src[pos] == '-')
      pos++;

    while (isdigit(src[pos]))
      pos++;
  }

  return pos - begin;
}

static size_t token_len_lit_str(const char* src, size_t pos)
{
  size_t begin = pos;

  pos++;

  while (src[pos] != '"')
  {
    if (src[pos] == '\\')
      pos++;

    pos++;
  }

  pos++;

  return pos - begin;
}

static size_t token_len_lit_char(const char* src, size_t pos)
{
  size_t begin = pos;

  pos++;

  while (src[pos] != '\'')
  {
    if (src[pos] == '\\')
      pos++;

    pos++;
  }

  pos++;

  return pos - begin;
}

static size_t token_len_lit_bool(const char* src, size_t pos)
{
  return src[pos] == 't' ? 4 : 5;
}

static size_t token_len_by_kind(token_kind_t kind)
{
  for (size_t i = 0; i < COUNTOF(TOKEN_LEN_LOOKUP); i++)
  {
    if (TOKEN_LEN_LOOKUP[i].kind == kind)
    {
      return TOKEN_LEN_LOOKUP[i].len;
    }
  }

  return 0;
}

location_t token_location(token_t* tok)
{
  const char* path = NULL;
  const char* src = NULL;

  token_registry_path_and_src(tok, &path, &src);

  ASSERT(path != NULL);
  ASSERT(src != NULL);

  size_t row = 0;
  size_t col = 0;

  for (size_t i = 0; i < tok->pos && src[i] != '\0'; i++)
  {
    if (src[i] == '\n')
    {
      row++;
      col = 0;
    }
  }

  size_t len = 0;

  switch (tok->kind)
  {
  case TOK_ID:       len = token_len_id      (src, tok->pos); break;
  case TOK_LIT_INT:  len = token_len_lit_int (src, tok->pos); break;
  case TOK_LIT_FLT:  len = token_len_lit_flt (src, tok->pos); break;
  case TOK_LIT_STR:  len = token_len_lit_str (src, tok->pos); break;
  case TOK_LIT_CHAR: len = token_len_lit_char(src, tok->pos); break;
  case TOK_LIT_BOOL: len = token_len_lit_bool(src, tok->pos); break;
  default:           len = token_len_by_kind (tok->kind    ); break;
  }

  location_t loc = {
    .path = path,
    .src = src,
    .ptr = src + tok->pos,
    .row = row,
    .col = col,
    .len = len
  };

  return loc;
}

void token_json_dump(FILE* stream, token_t* tok)
{
  fprintf(stream, "{\"kind\":\"%s\",\"loc\":", token_kind_to_cstr(tok->kind));

  const char* path = NULL;
  const char* src = NULL;

  token_registry_path_and_src(tok, &path, &src);

  ASSERT(path != NULL);
  ASSERT(src != NULL);

  location_t loc = token_location(tok);

  location_json_dump(loc, stream);

  if (token_is_literal(tok) || tok->kind == TOK_ID)
  {
    fprintf(stream, ",\"value\":\"");

    string_view_t view = token_to_string_view(tok);
    string_view_print_escaped(stream, view);
    fputc('"', stream);
  }

  fputc('}', stream);
}

void token_json_dump_vector(FILE* stream, vector_t* vec)
{
  fputc('[', stream);

  VECTOR_FOR_LOOP(i, vec)
  {
    token_t* tok = (token_t*)vector_get(vec, i);

    token_json_dump(stream, tok);

    if (i + 1 < vector_size(vec))
      fputc(',', stream);
  }

  fputc(']', stream);
}

const char* token_kind_to_cstr(token_kind_t kind)
{
  switch (kind)
  {
  case TOK_UNKNOWN:                     return "TOK_UNKNOWN";
  case TOK_ID:                          return "TOK_ID";
  case TOK_LIT_INT:                     return "TOK_LIT_INT";
  case TOK_LIT_FLT:                     return "TOK_LIT_FLT";
  case TOK_LIT_STR:                     return "TOK_LIT_STR";
  case TOK_LIT_CHAR:                    return "TOK_LIT_CHAR";
  case TOK_LIT_BOOL:                    return "TOK_LIT_BOOL";
  case TOK_LIT_NULL:                    return "TOK_LIT_NULL";
  case TOK_KW_IS:                       return "TOK_KW_IS";
  case TOK_KW_AS:                       return "TOK_KW_AS";
  case TOK_KW_SIZEOF:                   return "TOK_KW_SIZEOF";
  case TOK_KW_ALIGNOF:                  return "TOK_KW_ALIGNOF";
  case TOK_KW_USE:                      return "TOK_KW_USE";
  case TOK_KW_IN:                       return "TOK_KW_IN";
  case TOK_KW_PUB:                      return "TOK_KW_PUB";
  case TOK_KW_EXTERN:                   return "TOK_KW_EXTERN";
  case TOK_KW_FUN:                      return "TOK_KW_FUN";
  case TOK_KW_STRUCT:                   return "TOK_KW_STRUCT";
  case TOK_KW_UNION:                    return "TOK_KW_UNION";
  case TOK_KW_ENUM:                     return "TOK_KW_ENUM";
  case TOK_KW_MOD:                      return "TOK_KW_MOD";
  case TOK_KW_IF:                       return "TOK_KW_IF";
  case TOK_KW_THEN:                     return "TOK_KW_THEN";
  case TOK_KW_ELSE:                     return "TOK_KW_ELSE";
  case TOK_KW_FOR:                      return "TOK_KW_FOR";
  case TOK_KW_WHILE:                    return "TOK_KW_WHILE";
  case TOK_KW_DO:                       return "TOK_KW_DO";
  case TOK_KW_BREAK:                    return "TOK_KW_BREAK";
  case TOK_KW_CONTINUE:                 return "TOK_KW_CONTINUE";
  case TOK_KW_RETURN:                   return "TOK_KW_RETURN";
  case TOK_KW_DEFER:                    return "TOK_KW_DEFER";
  case TOK_KW_MUT:                      return "TOK_KW_MUT";
  case TOK_KW_I8:                       return "TOK_KW_I8";
  case TOK_KW_I16:                      return "TOK_KW_I16";
  case TOK_KW_I32:                      return "TOK_KW_I32";
  case TOK_KW_I64:                      return "TOK_KW_I64";
  case TOK_KW_ISIZE:                    return "TOK_KW_ISIZE";
  case TOK_KW_U8:                       return "TOK_KW_U8";
  case TOK_KW_U16:                      return "TOK_KW_U16";
  case TOK_KW_U32:                      return "TOK_KW_U32";
  case TOK_KW_U64:                      return "TOK_KW_U64";
  case TOK_KW_USIZE:                    return "TOK_KW_USIZE";
  case TOK_KW_F32:                      return "TOK_KW_F32";
  case TOK_KW_F64:                      return "TOK_KW_F64";
  case TOK_KW_CHAR:                     return "TOK_KW_CHAR";
  case TOK_KW_BOOL:                     return "TOK_KW_BOOL";
  case TOK_KW_UNIT:                     return "TOK_KW_UNIT";
  case TOK_PUNCT_PLUS:                  return "TOK_PUNCT_PLUS";
  case TOK_PUNCT_PLUS_PLUS:             return "TOK_PUNCT_PLUS_PLUS";
  case TOK_PUNCT_PLUS_EQUAL:            return "TOK_PUNCT_PLUS_EQUAL";
  case TOK_PUNCT_MINUS:                 return "TOK_PUNCT_MINUS";
  case TOK_PUNCT_MINUS_MINUS:           return "TOK_PUNCT_MINUS_MINUS";
  case TOK_PUNCT_MINUS_EQUAL:           return "TOK_PUNCT_MINUS_EQUAL";
  case TOK_PUNCT_ASTERISK:              return "TOK_PUNCT_ASTERISK";
  case TOK_PUNCT_ASTERISK_EQUAL:        return "TOK_PUNCT_ASTERISK_EQUAL";
  case TOK_PUNCT_ASTERISK_DOT:          return "TOK_PUNCT_ASTERISK_DOT";
  case TOK_PUNCT_SLASH:                 return "TOK_PUNCT_SLASH";
  case TOK_PUNCT_SLASH_EQUAL:           return "TOK_PUNCT_SLASH_EQUAL";
  case TOK_PUNCT_PERCENT:               return "TOK_PUNCT_PERCENT";
  case TOK_PUNCT_PERCENT_EQUAL:         return "TOK_PUNCT_PERCENT_EQUAL";
  case TOK_PUNCT_AMPERSAND:             return "TOK_PUNCT_AMPERSAND";
  case TOK_PUNCT_AMPERSAND_AMPERSAND:   return "TOK_PUNCT_AMPERSAND_AMPERSAND";
  case TOK_PUNCT_AMPERSAND_EQUAL:       return "TOK_PUNCT_AMPERSAND_EQUAL";
  case TOK_PUNCT_BAR:                   return "TOK_PUNCT_BAR";
  case TOK_PUNCT_BAR_BAR:               return "TOK_PUNCT_BAR_BAR";
  case TOK_PUNCT_BAR_EQUAL:             return "TOK_PUNCT_BAR_EQUAL";
  case TOK_PUNCT_HAT:                   return "TOK_PUNCT_HAT";
  case TOK_PUNCT_HAT_EQUAL:             return "TOK_PUNCT_HAT_EQUAL";
  case TOK_PUNCT_TILDE:                 return "TOK_PUNCT_TILDE";
  case TOK_PUNCT_LESS:                  return "TOK_PUNCT_LESS";
  case TOK_PUNCT_LESS_LESS:             return "TOK_PUNCT_LESS_LESS";
  case TOK_PUNCT_LESS_LESS_EQUAL:       return "TOK_PUNCT_LESS_LESS_EQUAL";
  case TOK_PUNCT_LESS_EQUAL:            return "TOK_PUNCT_LESS_EQUAL";
  case TOK_PUNCT_GREATER:               return "TOK_PUNCT_GREATER";
  case TOK_PUNCT_GREATER_GREATER:       return "TOK_PUNCT_GREATER_GREATER";
  case TOK_PUNCT_GREATER_GREATER_EQUAL: return "TOK_PUNCT_GREATER_GREATER_EQUAL";
  case TOK_PUNCT_GREATER_EQUAL:         return "TOK_PUNCT_GREATER_EQUAL";
  case TOK_PUNCT_BANG:                  return "TOK_PUNCT_BANG";
  case TOK_PUNCT_BANG_EQUAL:            return "TOK_PUNCT_BANG_EQUAL";
  case TOK_PUNCT_DOT:                   return "TOK_PUNCT_DOT";
  case TOK_PUNCT_DOT_DOT:               return "TOK_PUNCT_DOT_DOT";
  case TOK_PUNCT_DOT_DOT_DOT:           return "TOK_PUNCT_DOT_DOT_DOT";
  case TOK_PUNCT_QUESTION:              return "TOK_PUNCT_QUESTION";
  case TOK_PUNCT_QUESTION_DOT:          return "TOK_PUNCT_QUESTION_DOT";
  case TOK_PUNCT_EQUAL:                 return "TOK_PUNCT_EQUAL";
  case TOK_PUNCT_EQUAL_EQUAL:           return "TOK_PUNCT_EQUAL_EQUAL";
  case TOK_PUNCT_COMMA:                 return "TOK_PUNCT_COMMA";
  case TOK_PUNCT_COLON:                 return "TOK_PUNCT_COLON";
  case TOK_PUNCT_SEMICOLON:             return "TOK_PUNCT_SEMICOLON";
  case TOK_PUNCT_PAREN_LEFT:            return "TOK_PUNCT_PAREN_LEFT";
  case TOK_PUNCT_PAREN_RIGHT:           return "TOK_PUNCT_PAREN_RIGHT";
  case TOK_PUNCT_BRACKET_LEFT:          return "TOK_PUNCT_BRACKET_LEFT";
  case TOK_PUNCT_BRACKET_RIGHT:         return "TOK_PUNCT_BRACKET_RIGHT";
  case TOK_PUNCT_BRACE_LEFT:            return "TOK_PUNCT_BRACE_LEFT";
  case TOK_PUNCT_BRACE_RIGHT:           return "TOK_PUNCT_BRACE_RIGHT";
  case TOK_PUNCT_HASH:                  return "TOK_PUNCT_HASH";
  case TOK_NEWLINE:                     return "TOK_NEWLINE";
  case TOK_EOF:                         return "TOK_EOF";
  default: UNREACHABLE();
  }

  return NULL;
}

string_t* token_to_string(token_t* tok)
{
  return location_to_string(token_location(tok));
}

string_view_t token_to_string_view(token_t* tok)
{
  return location_to_string_view(token_location(tok));
}

bool token_is_literal(token_t* tok)
{
  switch (tok->kind)
  {
  case TOK_LIT_INT:
  case TOK_LIT_FLT:
  case TOK_LIT_STR:
  case TOK_LIT_CHAR:
  case TOK_LIT_BOOL:
  case TOK_LIT_NULL:
    return true;
  default:
    return false;
  }
}

bool token_is_keyword(token_t* tok)
{
  switch (tok->kind)
  {
  case TOK_KW_IS:
  case TOK_KW_AS:
  case TOK_KW_SIZEOF:
  case TOK_KW_ALIGNOF:
  case TOK_KW_USE:
  case TOK_KW_IN:
  case TOK_KW_PUB:
  case TOK_KW_EXTERN:
  case TOK_KW_FUN:
  case TOK_KW_STRUCT:
  case TOK_KW_UNION:
  case TOK_KW_ENUM:
  case TOK_KW_MOD:
  case TOK_KW_IF:
  case TOK_KW_THEN:
  case TOK_KW_ELSE:
  case TOK_KW_FOR:
  case TOK_KW_WHILE:
  case TOK_KW_DO:
  case TOK_KW_BREAK:
  case TOK_KW_CONTINUE:
  case TOK_KW_RETURN:
  case TOK_KW_DEFER:
  case TOK_KW_MUT:
  case TOK_KW_I8:
  case TOK_KW_I16:
  case TOK_KW_I32:
  case TOK_KW_I64:
  case TOK_KW_ISIZE:
  case TOK_KW_U8:
  case TOK_KW_U16:
  case TOK_KW_U32:
  case TOK_KW_U64:
  case TOK_KW_USIZE:
  case TOK_KW_F32:
  case TOK_KW_F64:
  case TOK_KW_CHAR:
  case TOK_KW_BOOL:
  case TOK_KW_UNIT:
    return true;
  default:
    return false;
  }
}

bool token_is_punctuation(token_t* tok)
{
  switch (tok->kind)
  {
  case TOK_PUNCT_PLUS:
  case TOK_PUNCT_PLUS_PLUS:
  case TOK_PUNCT_PLUS_EQUAL:
  case TOK_PUNCT_MINUS:
  case TOK_PUNCT_MINUS_MINUS:
  case TOK_PUNCT_MINUS_EQUAL:
  case TOK_PUNCT_ASTERISK:
  case TOK_PUNCT_ASTERISK_EQUAL:
  case TOK_PUNCT_ASTERISK_DOT:
  case TOK_PUNCT_SLASH:
  case TOK_PUNCT_SLASH_EQUAL:
  case TOK_PUNCT_PERCENT:
  case TOK_PUNCT_PERCENT_EQUAL:
  case TOK_PUNCT_AMPERSAND:
  case TOK_PUNCT_AMPERSAND_AMPERSAND:
  case TOK_PUNCT_AMPERSAND_EQUAL:
  case TOK_PUNCT_BAR:
  case TOK_PUNCT_BAR_BAR:
  case TOK_PUNCT_BAR_EQUAL:
  case TOK_PUNCT_HAT:
  case TOK_PUNCT_HAT_EQUAL:
  case TOK_PUNCT_TILDE:
  case TOK_PUNCT_LESS:
  case TOK_PUNCT_LESS_LESS:
  case TOK_PUNCT_LESS_LESS_EQUAL:
  case TOK_PUNCT_LESS_EQUAL:
  case TOK_PUNCT_GREATER:
  case TOK_PUNCT_GREATER_GREATER:
  case TOK_PUNCT_GREATER_GREATER_EQUAL:
  case TOK_PUNCT_GREATER_EQUAL:
  case TOK_PUNCT_BANG:
  case TOK_PUNCT_BANG_EQUAL:
  case TOK_PUNCT_DOT:
  case TOK_PUNCT_DOT_DOT:
  case TOK_PUNCT_DOT_DOT_DOT:
  case TOK_PUNCT_QUESTION:
  case TOK_PUNCT_QUESTION_DOT:
  case TOK_PUNCT_EQUAL:
  case TOK_PUNCT_EQUAL_EQUAL:
  case TOK_PUNCT_COMMA:
  case TOK_PUNCT_COLON:
  case TOK_PUNCT_SEMICOLON:
  case TOK_PUNCT_PAREN_LEFT:
  case TOK_PUNCT_PAREN_RIGHT:
  case TOK_PUNCT_BRACKET_LEFT:
  case TOK_PUNCT_BRACKET_RIGHT:
  case TOK_PUNCT_BRACE_LEFT:
  case TOK_PUNCT_BRACE_RIGHT:
  case TOK_PUNCT_HASH:
    return true;
  default:
    return false;
  }
}
