/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/lexer/token/token.h"

#include "stages/lexer/token/registry.h"

static const struct
{
  tau_token_kind_t kind;
  size_t len;
} g_token_len_lookup[] = {
  { TAU_TOK_UNKNOWN, 0 },
  { TAU_TOK_LIT_NULL, 4 },
  { TAU_TOK_KW_IS, 2 },
  { TAU_TOK_KW_AS, 2 },
  { TAU_TOK_KW_SIZEOF, 6 },
  { TAU_TOK_KW_ALIGNOF, 7 },
  { TAU_TOK_KW_USE, 3 },
  { TAU_TOK_KW_IN, 2 },
  { TAU_TOK_KW_PUB, 3 },
  { TAU_TOK_KW_EXTERN, 6 },
  { TAU_TOK_KW_FUN, 3 },
  { TAU_TOK_KW_STRUCT, 6 },
  { TAU_TOK_KW_UNION, 5 },
  { TAU_TOK_KW_ENUM, 4 },
  { TAU_TOK_KW_MOD, 3 },
  { TAU_TOK_KW_IF, 2 },
  { TAU_TOK_KW_THEN, 4 },
  { TAU_TOK_KW_ELSE, 4 },
  { TAU_TOK_KW_FOR, 3 },
  { TAU_TOK_KW_WHILE, 5 },
  { TAU_TOK_KW_DO, 2 },
  { TAU_TOK_KW_LOOP, 4 },
  { TAU_TOK_KW_BREAK, 5 },
  { TAU_TOK_KW_CONTINUE, 8 },
  { TAU_TOK_KW_RETURN, 6 },
  { TAU_TOK_KW_DEFER, 5 },
  { TAU_TOK_KW_MUT, 3 },
  { TAU_TOK_KW_I8, 2 },
  { TAU_TOK_KW_I16, 3 },
  { TAU_TOK_KW_I32, 3 },
  { TAU_TOK_KW_I64, 3 },
  { TAU_TOK_KW_ISIZE, 5 },
  { TAU_TOK_KW_U8, 2 },
  { TAU_TOK_KW_U16, 3 },
  { TAU_TOK_KW_U32, 3 },
  { TAU_TOK_KW_U64, 3 },
  { TAU_TOK_KW_USIZE, 5 },
  { TAU_TOK_KW_F32, 3 },
  { TAU_TOK_KW_F64, 3 },
  { TAU_TOK_KW_C64, 3 },
  { TAU_TOK_KW_C128, 4 },
  { TAU_TOK_KW_CHAR, 4 },
  { TAU_TOK_KW_BOOL, 4 },
  { TAU_TOK_KW_UNIT, 4 },
  { TAU_TOK_KW_TYPE, 4 },
  { TAU_TOK_KW_UNDEF, 5 },
  { TAU_TOK_PUNCT_PLUS, 1 },
  { TAU_TOK_PUNCT_PLUS_PLUS, 2 },
  { TAU_TOK_PUNCT_PLUS_EQUAL, 2 },
  { TAU_TOK_PUNCT_MINUS, 1 },
  { TAU_TOK_PUNCT_MINUS_MINUS, 2 },
  { TAU_TOK_PUNCT_MINUS_EQUAL, 2 },
  { TAU_TOK_PUNCT_MINUS_GREATER, 2 },
  { TAU_TOK_PUNCT_ASTERISK, 1 },
  { TAU_TOK_PUNCT_ASTERISK_EQUAL, 2 },
  { TAU_TOK_PUNCT_SLASH, 1 },
  { TAU_TOK_PUNCT_SLASH_EQUAL, 2 },
  { TAU_TOK_PUNCT_PERCENT, 1 },
  { TAU_TOK_PUNCT_PERCENT_EQUAL, 2 },
  { TAU_TOK_PUNCT_AMPERSAND, 1 },
  { TAU_TOK_PUNCT_AMPERSAND_AMPERSAND, 2 },
  { TAU_TOK_PUNCT_AMPERSAND_EQUAL, 2 },
  { TAU_TOK_PUNCT_BAR, 1 },
  { TAU_TOK_PUNCT_BAR_BAR, 1 },
  { TAU_TOK_PUNCT_BAR_EQUAL, 2 },
  { TAU_TOK_PUNCT_HAT, 1 },
  { TAU_TOK_PUNCT_HAT_EQUAL, 2 },
  { TAU_TOK_PUNCT_TILDE, 1 },
  { TAU_TOK_PUNCT_LESS, 1 },
  { TAU_TOK_PUNCT_LESS_LESS, 2 },
  { TAU_TOK_PUNCT_LESS_LESS_EQUAL, 3 },
  { TAU_TOK_PUNCT_LESS_EQUAL, 2 },
  { TAU_TOK_PUNCT_GREATER, 1 },
  { TAU_TOK_PUNCT_GREATER_GREATER, 2 },
  { TAU_TOK_PUNCT_GREATER_GREATER_EQUAL, 3 },
  { TAU_TOK_PUNCT_GREATER_EQUAL, 2 },
  { TAU_TOK_PUNCT_BANG, 1 },
  { TAU_TOK_PUNCT_BANG_EQUAL, 2 },
  { TAU_TOK_PUNCT_DOT, 1 },
  { TAU_TOK_PUNCT_DOT_DOT, 2 },
  { TAU_TOK_PUNCT_DOT_DOT_DOT, 3 },
  { TAU_TOK_PUNCT_DOT_LESS, 2 },
  { TAU_TOK_PUNCT_QUESTION, 1 },
  { TAU_TOK_PUNCT_QUESTION_DOT, 2 },
  { TAU_TOK_PUNCT_EQUAL, 1 },
  { TAU_TOK_PUNCT_EQUAL_EQUAL, 2 },
  { TAU_TOK_PUNCT_COMMA, 1 },
  { TAU_TOK_PUNCT_COLON, 1 },
  { TAU_TOK_PUNCT_SEMICOLON, 1 },
  { TAU_TOK_PUNCT_PAREN_LEFT, 1 },
  { TAU_TOK_PUNCT_PAREN_RIGHT, 1 },
  { TAU_TOK_PUNCT_BRACKET_LEFT, 1 },
  { TAU_TOK_PUNCT_BRACKET_RIGHT, 1 },
  { TAU_TOK_PUNCT_BRACKET_ANGLE_LEFT, 2 },
  { TAU_TOK_PUNCT_BRACKET_ANGLE_RIGHT, 2 },
  { TAU_TOK_PUNCT_BRACE_LEFT, 1 },
  { TAU_TOK_PUNCT_BRACE_RIGHT, 1 },
  { TAU_TOK_PUNCT_HASH, 1 },
  { TAU_TOK_NEWLINE, 0 },
  { TAU_TOK_EOF, 0 },
};

static size_t tau_token_len_lit_int_hex(const char* src, size_t pos)
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

static size_t tau_token_len_lit_int_oct(const char* src, size_t pos)
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

static size_t tau_token_len_lit_int_bin(const char* src, size_t pos)
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

static size_t tau_token_len_lit_int_dec(const char* src, size_t pos)
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

static size_t tau_token_len_id(const char* src, size_t pos)
{
  size_t begin = pos;

  while (isalnum(src[pos]) || src[pos] == '_')
    pos++;

  return pos - begin;
}

static size_t tau_token_len_lit_int(const char* src, size_t pos)
{
  if (src[pos] == '0')
  {
    if (src[pos + 1] == 'x' || src[pos + 1] == 'X')
      return tau_token_len_lit_int_hex(src, pos);

    if (src[pos + 1] == 'o' || src[pos + 1] == 'O')
      return tau_token_len_lit_int_oct(src, pos);

    if (src[pos + 1] == 'b' || src[pos + 1] == 'B')
      return tau_token_len_lit_int_bin(src, pos);
  }

  return tau_token_len_lit_int_dec(src, pos);
}

static size_t tau_token_len_lit_flt(const char* src, size_t pos)
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

static size_t tau_token_len_lit_str(const char* src, size_t pos)
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

static size_t tau_token_len_lit_char(const char* src, size_t pos)
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

static size_t tau_token_len_lit_bool(const char* src, size_t pos)
{
  return src[pos] == 't' ? 4 : 5;
}

static size_t tau_token_len_kw_vec(const char* src, size_t pos)
{
  size_t begin = pos;

  while (isalnum(src[pos]))
    pos++;

  return pos - begin;
}

static size_t tau_token_len_kw_mat(const char* src, size_t pos)
{
  size_t begin = pos;

  while (isalnum(src[pos]))
    pos++;

  return pos - begin;
}

static size_t tau_token_len_by_kind(tau_token_kind_t kind)
{
  for (size_t i = 0; i < TAU_COUNTOF(g_token_len_lookup); i++)
  {
    if (g_token_len_lookup[i].kind == kind)
    {
      return g_token_len_lookup[i].len;
    }
  }

  return 0;
}

tau_location_t tau_token_location(tau_token_t* tok)
{
  const char* path = NULL;
  const char* src = NULL;

  tau_token_registry_path_and_src(tok, &path, &src);

  TAU_ASSERT(path != NULL);
  TAU_ASSERT(src != NULL);

  size_t row = 0;
  size_t col = 0;

  for (size_t i = 0; i < tok->pos && src[i] != '\0'; i++, col++)
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
  case TAU_TOK_ID:       len = tau_token_len_id      (src, tok->pos); break;
  case TAU_TOK_LIT_INT:  len = tau_token_len_lit_int (src, tok->pos); break;
  case TAU_TOK_LIT_FLT:  len = tau_token_len_lit_flt (src, tok->pos); break;
  case TAU_TOK_LIT_STR:  len = tau_token_len_lit_str (src, tok->pos); break;
  case TAU_TOK_LIT_CHAR: len = tau_token_len_lit_char(src, tok->pos); break;
  case TAU_TOK_LIT_BOOL: len = tau_token_len_lit_bool(src, tok->pos); break;
  case TAU_TOK_KW_VEC:   len = tau_token_len_kw_vec  (src, tok->pos); break;
  case TAU_TOK_KW_MAT:   len = tau_token_len_kw_mat  (src, tok->pos); break;
  default:           len = tau_token_len_by_kind (tok->kind    ); break;
  }

  tau_location_t loc = {
    .path = path,
    .src = src,
    .ptr = src + tok->pos,
    .row = row,
    .col = col,
    .len = len
  };

  return loc;
}

void tau_token_json_dump(FILE* stream, tau_token_t* tok)
{
  fprintf(stream, "{\"kind\":\"%s\",\"loc\":", tau_token_kind_to_cstr(tok->kind));

  const char* path = NULL;
  const char* src = NULL;

  tau_token_registry_path_and_src(tok, &path, &src);

  TAU_ASSERT(path != NULL);
  TAU_ASSERT(src != NULL);

  tau_location_t loc = tau_token_location(tok);

  tau_location_json_dump(loc, stream);

  if (tau_token_is_literal(tok) || tok->kind == TAU_TOK_ID)
  {
    fprintf(stream, ",\"value\":\"");

    tau_string_view_t view = tau_token_to_string_view(tok);
    tau_string_view_print_escaped(stream, view);
    fputc('"', stream);
  }

  fputc('}', stream);
}

void tau_token_json_dump_vector(FILE* stream, tau_vector_t* vec)
{
  fputc('[', stream);

  TAU_VECTOR_FOR_LOOP(i, vec)
  {
    tau_token_t* tok = (tau_token_t*)tau_vector_get(vec, i);

    tau_token_json_dump(stream, tok);

    if (i + 1 < tau_vector_size(vec))
      fputc(',', stream);
  }

  fputc(']', stream);
}

const char* tau_token_kind_to_cstr(tau_token_kind_t kind)
{
  switch (kind)
  {
  case TAU_TOK_UNKNOWN:                     return "TAU_TOK_UNKNOWN";
  case TAU_TOK_ID:                          return "TAU_TOK_ID";
  case TAU_TOK_LIT_INT:                     return "TAU_TOK_LIT_INT";
  case TAU_TOK_LIT_FLT:                     return "TAU_TOK_LIT_FLT";
  case TAU_TOK_LIT_STR:                     return "TAU_TOK_LIT_STR";
  case TAU_TOK_LIT_CHAR:                    return "TAU_TOK_LIT_CHAR";
  case TAU_TOK_LIT_BOOL:                    return "TAU_TOK_LIT_BOOL";
  case TAU_TOK_LIT_NULL:                    return "TAU_TOK_LIT_NULL";
  case TAU_TOK_KW_IS:                       return "TAU_TOK_KW_IS";
  case TAU_TOK_KW_AS:                       return "TAU_TOK_KW_AS";
  case TAU_TOK_KW_SIZEOF:                   return "TAU_TOK_KW_SIZEOF";
  case TAU_TOK_KW_ALIGNOF:                  return "TAU_TOK_KW_ALIGNOF";
  case TAU_TOK_KW_USE:                      return "TAU_TOK_KW_USE";
  case TAU_TOK_KW_IN:                       return "TAU_TOK_KW_IN";
  case TAU_TOK_KW_PUB:                      return "TAU_TOK_KW_PUB";
  case TAU_TOK_KW_EXTERN:                   return "TAU_TOK_KW_EXTERN";
  case TAU_TOK_KW_FUN:                      return "TAU_TOK_KW_FUN";
  case TAU_TOK_KW_STRUCT:                   return "TAU_TOK_KW_STRUCT";
  case TAU_TOK_KW_UNION:                    return "TAU_TOK_KW_UNION";
  case TAU_TOK_KW_ENUM:                     return "TAU_TOK_KW_ENUM";
  case TAU_TOK_KW_MOD:                      return "TAU_TOK_KW_MOD";
  case TAU_TOK_KW_IF:                       return "TAU_TOK_KW_IF";
  case TAU_TOK_KW_THEN:                     return "TAU_TOK_KW_THEN";
  case TAU_TOK_KW_ELSE:                     return "TAU_TOK_KW_ELSE";
  case TAU_TOK_KW_FOR:                      return "TAU_TOK_KW_FOR";
  case TAU_TOK_KW_WHILE:                    return "TAU_TOK_KW_WHILE";
  case TAU_TOK_KW_DO:                       return "TAU_TOK_KW_DO";
  case TAU_TOK_KW_LOOP:                     return "TAU_TOK_KW_LOOP";
  case TAU_TOK_KW_BREAK:                    return "TAU_TOK_KW_BREAK";
  case TAU_TOK_KW_CONTINUE:                 return "TAU_TOK_KW_CONTINUE";
  case TAU_TOK_KW_RETURN:                   return "TAU_TOK_KW_RETURN";
  case TAU_TOK_KW_DEFER:                    return "TAU_TOK_KW_DEFER";
  case TAU_TOK_KW_MUT:                      return "TAU_TOK_KW_MUT";
  case TAU_TOK_KW_I8:                       return "TAU_TOK_KW_I8";
  case TAU_TOK_KW_I16:                      return "TAU_TOK_KW_I16";
  case TAU_TOK_KW_I32:                      return "TAU_TOK_KW_I32";
  case TAU_TOK_KW_I64:                      return "TAU_TOK_KW_I64";
  case TAU_TOK_KW_ISIZE:                    return "TAU_TOK_KW_ISIZE";
  case TAU_TOK_KW_U8:                       return "TAU_TOK_KW_U8";
  case TAU_TOK_KW_U16:                      return "TAU_TOK_KW_U16";
  case TAU_TOK_KW_U32:                      return "TAU_TOK_KW_U32";
  case TAU_TOK_KW_U64:                      return "TAU_TOK_KW_U64";
  case TAU_TOK_KW_USIZE:                    return "TAU_TOK_KW_USIZE";
  case TAU_TOK_KW_F32:                      return "TAU_TOK_KW_F32";
  case TAU_TOK_KW_F64:                      return "TAU_TOK_KW_F64";
  case TAU_TOK_KW_C64:                      return "TAU_TOK_KW_C64";
  case TAU_TOK_KW_C128:                     return "TAU_TOK_KW_C128";
  case TAU_TOK_KW_VEC:                      return "TAU_TOK_KW_VEC";
  case TAU_TOK_KW_MAT:                      return "TAU_TOK_KW_MAT";
  case TAU_TOK_KW_CHAR:                     return "TAU_TOK_KW_CHAR";
  case TAU_TOK_KW_BOOL:                     return "TAU_TOK_KW_BOOL";
  case TAU_TOK_KW_UNIT:                     return "TAU_TOK_KW_UNIT";
  case TAU_TOK_KW_TYPE:                     return "TAU_TOK_KW_TYPE";
  case TAU_TOK_KW_UNDEF:                    return "TAU_TOK_KW_UNDEF";
  case TAU_TOK_PUNCT_PLUS:                  return "TAU_TOK_PUNCT_PLUS";
  case TAU_TOK_PUNCT_PLUS_PLUS:             return "TAU_TOK_PUNCT_PLUS_PLUS";
  case TAU_TOK_PUNCT_PLUS_EQUAL:            return "TAU_TOK_PUNCT_PLUS_EQUAL";
  case TAU_TOK_PUNCT_MINUS:                 return "TAU_TOK_PUNCT_MINUS";
  case TAU_TOK_PUNCT_MINUS_MINUS:           return "TAU_TOK_PUNCT_MINUS_MINUS";
  case TAU_TOK_PUNCT_MINUS_EQUAL:           return "TAU_TOK_PUNCT_MINUS_EQUAL";
  case TAU_TOK_PUNCT_MINUS_GREATER:         return "TAU_TOK_PUNCT_MINUS_GREATER";
  case TAU_TOK_PUNCT_ASTERISK:              return "TAU_TOK_PUNCT_ASTERISK";
  case TAU_TOK_PUNCT_ASTERISK_EQUAL:        return "TAU_TOK_PUNCT_ASTERISK_EQUAL";
  case TAU_TOK_PUNCT_SLASH:                 return "TAU_TOK_PUNCT_SLASH";
  case TAU_TOK_PUNCT_SLASH_EQUAL:           return "TAU_TOK_PUNCT_SLASH_EQUAL";
  case TAU_TOK_PUNCT_PERCENT:               return "TAU_TOK_PUNCT_PERCENT";
  case TAU_TOK_PUNCT_PERCENT_EQUAL:         return "TAU_TOK_PUNCT_PERCENT_EQUAL";
  case TAU_TOK_PUNCT_AMPERSAND:             return "TAU_TOK_PUNCT_AMPERSAND";
  case TAU_TOK_PUNCT_AMPERSAND_AMPERSAND:   return "TAU_TOK_PUNCT_AMPERSAND_AMPERSAND";
  case TAU_TOK_PUNCT_AMPERSAND_EQUAL:       return "TAU_TOK_PUNCT_AMPERSAND_EQUAL";
  case TAU_TOK_PUNCT_BAR:                   return "TAU_TOK_PUNCT_BAR";
  case TAU_TOK_PUNCT_BAR_BAR:               return "TAU_TOK_PUNCT_BAR_BAR";
  case TAU_TOK_PUNCT_BAR_EQUAL:             return "TAU_TOK_PUNCT_BAR_EQUAL";
  case TAU_TOK_PUNCT_HAT:                   return "TAU_TOK_PUNCT_HAT";
  case TAU_TOK_PUNCT_HAT_EQUAL:             return "TAU_TOK_PUNCT_HAT_EQUAL";
  case TAU_TOK_PUNCT_TILDE:                 return "TAU_TOK_PUNCT_TILDE";
  case TAU_TOK_PUNCT_LESS:                  return "TAU_TOK_PUNCT_LESS";
  case TAU_TOK_PUNCT_LESS_LESS:             return "TAU_TOK_PUNCT_LESS_LESS";
  case TAU_TOK_PUNCT_LESS_LESS_EQUAL:       return "TAU_TOK_PUNCT_LESS_LESS_EQUAL";
  case TAU_TOK_PUNCT_LESS_EQUAL:            return "TAU_TOK_PUNCT_LESS_EQUAL";
  case TAU_TOK_PUNCT_GREATER:               return "TAU_TOK_PUNCT_GREATER";
  case TAU_TOK_PUNCT_GREATER_GREATER:       return "TAU_TOK_PUNCT_GREATER_GREATER";
  case TAU_TOK_PUNCT_GREATER_GREATER_EQUAL: return "TAU_TOK_PUNCT_GREATER_GREATER_EQUAL";
  case TAU_TOK_PUNCT_GREATER_EQUAL:         return "TAU_TOK_PUNCT_GREATER_EQUAL";
  case TAU_TOK_PUNCT_BANG:                  return "TAU_TOK_PUNCT_BANG";
  case TAU_TOK_PUNCT_BANG_EQUAL:            return "TAU_TOK_PUNCT_BANG_EQUAL";
  case TAU_TOK_PUNCT_DOT:                   return "TAU_TOK_PUNCT_DOT";
  case TAU_TOK_PUNCT_DOT_DOT:               return "TAU_TOK_PUNCT_DOT_DOT";
  case TAU_TOK_PUNCT_DOT_DOT_DOT:           return "TAU_TOK_PUNCT_DOT_DOT_DOT";
  case TAU_TOK_PUNCT_DOT_LESS:              return "TAU_TOK_PUNCT_DOT_LESS";
  case TAU_TOK_PUNCT_QUESTION:              return "TAU_TOK_PUNCT_QUESTION";
  case TAU_TOK_PUNCT_QUESTION_DOT:          return "TAU_TOK_PUNCT_QUESTION_DOT";
  case TAU_TOK_PUNCT_EQUAL:                 return "TAU_TOK_PUNCT_EQUAL";
  case TAU_TOK_PUNCT_EQUAL_EQUAL:           return "TAU_TOK_PUNCT_EQUAL_EQUAL";
  case TAU_TOK_PUNCT_COMMA:                 return "TAU_TOK_PUNCT_COMMA";
  case TAU_TOK_PUNCT_COLON:                 return "TAU_TOK_PUNCT_COLON";
  case TAU_TOK_PUNCT_SEMICOLON:             return "TAU_TOK_PUNCT_SEMICOLON";
  case TAU_TOK_PUNCT_PAREN_LEFT:            return "TAU_TOK_PUNCT_PAREN_LEFT";
  case TAU_TOK_PUNCT_PAREN_RIGHT:           return "TAU_TOK_PUNCT_PAREN_RIGHT";
  case TAU_TOK_PUNCT_BRACKET_LEFT:          return "TAU_TOK_PUNCT_BRACKET_LEFT";
  case TAU_TOK_PUNCT_BRACKET_RIGHT:         return "TAU_TOK_PUNCT_BRACKET_RIGHT";
  case TAU_TOK_PUNCT_BRACKET_ANGLE_LEFT:    return "TAU_TOK_PUNCT_BRACKET_ANGLE_LEFT";
  case TAU_TOK_PUNCT_BRACKET_ANGLE_RIGHT:   return "TAU_TOK_PUNCT_BRACKET_ANGLE_RIGHT";
  case TAU_TOK_PUNCT_BRACE_LEFT:            return "TAU_TOK_PUNCT_BRACE_LEFT";
  case TAU_TOK_PUNCT_BRACE_RIGHT:           return "TAU_TOK_PUNCT_BRACE_RIGHT";
  case TAU_TOK_PUNCT_HASH:                  return "TAU_TOK_PUNCT_HASH";
  case TAU_TOK_NEWLINE:                     return "TAU_TOK_NEWLINE";
  case TAU_TOK_EOF:                         return "TAU_TOK_EOF";
  default: TAU_UNREACHABLE();
  }

  return NULL;
}

tau_string_t* tau_token_to_string(tau_token_t* tok)
{
  return tau_location_to_string(tau_token_location(tok));
}

tau_string_view_t tau_token_to_string_view(tau_token_t* tok)
{
  return tau_location_to_string_view(tau_token_location(tok));
}

bool tau_token_is_literal(tau_token_t* tok)
{
  switch (tok->kind)
  {
  case TAU_TOK_LIT_INT:
  case TAU_TOK_LIT_FLT:
  case TAU_TOK_LIT_STR:
  case TAU_TOK_LIT_CHAR:
  case TAU_TOK_LIT_BOOL:
  case TAU_TOK_LIT_NULL:
    return true;
  default:
    return false;
  }
}

bool tau_token_is_keyword(tau_token_t* tok)
{
  switch (tok->kind)
  {
  case TAU_TOK_KW_IS:
  case TAU_TOK_KW_AS:
  case TAU_TOK_KW_SIZEOF:
  case TAU_TOK_KW_ALIGNOF:
  case TAU_TOK_KW_USE:
  case TAU_TOK_KW_IN:
  case TAU_TOK_KW_PUB:
  case TAU_TOK_KW_EXTERN:
  case TAU_TOK_KW_FUN:
  case TAU_TOK_KW_STRUCT:
  case TAU_TOK_KW_UNION:
  case TAU_TOK_KW_ENUM:
  case TAU_TOK_KW_MOD:
  case TAU_TOK_KW_IF:
  case TAU_TOK_KW_THEN:
  case TAU_TOK_KW_ELSE:
  case TAU_TOK_KW_FOR:
  case TAU_TOK_KW_WHILE:
  case TAU_TOK_KW_DO:
  case TAU_TOK_KW_LOOP:
  case TAU_TOK_KW_BREAK:
  case TAU_TOK_KW_CONTINUE:
  case TAU_TOK_KW_RETURN:
  case TAU_TOK_KW_DEFER:
  case TAU_TOK_KW_MUT:
  case TAU_TOK_KW_I8:
  case TAU_TOK_KW_I16:
  case TAU_TOK_KW_I32:
  case TAU_TOK_KW_I64:
  case TAU_TOK_KW_ISIZE:
  case TAU_TOK_KW_U8:
  case TAU_TOK_KW_U16:
  case TAU_TOK_KW_U32:
  case TAU_TOK_KW_U64:
  case TAU_TOK_KW_USIZE:
  case TAU_TOK_KW_F32:
  case TAU_TOK_KW_F64:
  case TAU_TOK_KW_C64:
  case TAU_TOK_KW_C128:
  case TAU_TOK_KW_VEC:
  case TAU_TOK_KW_MAT:
  case TAU_TOK_KW_CHAR:
  case TAU_TOK_KW_BOOL:
  case TAU_TOK_KW_UNIT:
  case TAU_TOK_KW_TYPE:
  case TAU_TOK_KW_UNDEF:
    return true;
  default:
    return false;
  }
}

bool tau_token_is_punctuation(tau_token_t* tok)
{
  switch (tok->kind)
  {
  case TAU_TOK_PUNCT_PLUS:
  case TAU_TOK_PUNCT_PLUS_PLUS:
  case TAU_TOK_PUNCT_PLUS_EQUAL:
  case TAU_TOK_PUNCT_MINUS:
  case TAU_TOK_PUNCT_MINUS_MINUS:
  case TAU_TOK_PUNCT_MINUS_EQUAL:
  case TAU_TOK_PUNCT_MINUS_GREATER:
  case TAU_TOK_PUNCT_ASTERISK:
  case TAU_TOK_PUNCT_ASTERISK_EQUAL:
  case TAU_TOK_PUNCT_SLASH:
  case TAU_TOK_PUNCT_SLASH_EQUAL:
  case TAU_TOK_PUNCT_PERCENT:
  case TAU_TOK_PUNCT_PERCENT_EQUAL:
  case TAU_TOK_PUNCT_AMPERSAND:
  case TAU_TOK_PUNCT_AMPERSAND_AMPERSAND:
  case TAU_TOK_PUNCT_AMPERSAND_EQUAL:
  case TAU_TOK_PUNCT_BAR:
  case TAU_TOK_PUNCT_BAR_BAR:
  case TAU_TOK_PUNCT_BAR_EQUAL:
  case TAU_TOK_PUNCT_HAT:
  case TAU_TOK_PUNCT_HAT_EQUAL:
  case TAU_TOK_PUNCT_TILDE:
  case TAU_TOK_PUNCT_LESS:
  case TAU_TOK_PUNCT_LESS_LESS:
  case TAU_TOK_PUNCT_LESS_LESS_EQUAL:
  case TAU_TOK_PUNCT_LESS_EQUAL:
  case TAU_TOK_PUNCT_GREATER:
  case TAU_TOK_PUNCT_GREATER_GREATER:
  case TAU_TOK_PUNCT_GREATER_GREATER_EQUAL:
  case TAU_TOK_PUNCT_GREATER_EQUAL:
  case TAU_TOK_PUNCT_BANG:
  case TAU_TOK_PUNCT_BANG_EQUAL:
  case TAU_TOK_PUNCT_DOT:
  case TAU_TOK_PUNCT_DOT_DOT:
  case TAU_TOK_PUNCT_DOT_DOT_DOT:
  case TAU_TOK_PUNCT_DOT_LESS:
  case TAU_TOK_PUNCT_QUESTION:
  case TAU_TOK_PUNCT_QUESTION_DOT:
  case TAU_TOK_PUNCT_EQUAL:
  case TAU_TOK_PUNCT_EQUAL_EQUAL:
  case TAU_TOK_PUNCT_COMMA:
  case TAU_TOK_PUNCT_COLON:
  case TAU_TOK_PUNCT_SEMICOLON:
  case TAU_TOK_PUNCT_PAREN_LEFT:
  case TAU_TOK_PUNCT_PAREN_RIGHT:
  case TAU_TOK_PUNCT_BRACKET_LEFT:
  case TAU_TOK_PUNCT_BRACKET_RIGHT:
  case TAU_TOK_PUNCT_BRACKET_ANGLE_LEFT:
  case TAU_TOK_PUNCT_BRACKET_ANGLE_RIGHT:
  case TAU_TOK_PUNCT_BRACE_LEFT:
  case TAU_TOK_PUNCT_BRACE_RIGHT:
  case TAU_TOK_PUNCT_HASH:
    return true;
  default:
    return false;
  }
}
