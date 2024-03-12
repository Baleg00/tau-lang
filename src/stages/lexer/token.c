/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/lexer/token.h"

#include "stages/lexer/location.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

token_t* token_init(token_kind_t kind, location_t* loc)
{
  token_t* tok = (token_t*)malloc(sizeof(token_t));

  tok->kind = kind;
  tok->loc = loc;

  return tok;
}

void token_free(token_t* tok)
{
  location_free(tok->loc);
  free(tok);
}

void token_json_dump(FILE* stream, token_t* tok)
{
  fprintf(stream, "{\"kind\":\"%s\",\"loc\":", token_kind_to_cstr(tok->kind));

  location_json_dump(tok->loc, stream);

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
  case TOK_KW_CONST:                    return "TOK_KW_CONST";
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
  return location_to_string(tok->loc);
}

string_view_t token_to_string_view(token_t* tok)
{
  return location_to_string_view(tok->loc);
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
  case TOK_KW_CONST:
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
