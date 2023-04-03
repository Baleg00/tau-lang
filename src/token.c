#include "token.h"

#include <stdlib.h>

#include "util.h"
#include "list.h"
#include "location.h"
#include "memtrace.h"

token_t* token_init(token_kind_t kind, location_t* loc)
{
  token_t* tok = (token_t*)malloc(sizeof(token_t));
  assert(tok != NULL);
  tok->kind = kind;
  tok->loc = loc;
  return tok;
}

token_id_t* token_id_init(location_t* loc, char* value)
{
  token_id_t* tok = (token_id_t*)malloc(sizeof(token_id_t));
  assert(tok != NULL);
  tok->kind = TOK_ID;
  tok->loc = loc;
  tok->value = value;
  return tok;
}

token_lit_int_t* token_lit_int_init(location_t* loc, uint64_t value)
{
  token_lit_int_t* tok = (token_lit_int_t*)malloc(sizeof(token_lit_int_t));
  assert(tok != NULL);
  tok->kind = TOK_LIT_INT;
  tok->loc = loc;
  tok->value = value;
  return tok;
}

token_lit_flt_t* token_lit_flt_init(location_t* loc, long double value)
{
  token_lit_flt_t* tok = (token_lit_flt_t*)malloc(sizeof(token_lit_flt_t));
  assert(tok != NULL);
  tok->kind = TOK_LIT_FLT;
  tok->loc = loc;
  tok->value = value;
  return tok;
}

token_lit_str_t* token_lit_str_init(location_t* loc, char* value)
{
  token_lit_str_t* tok = (token_lit_str_t*)malloc(sizeof(token_lit_str_t));
  assert(tok != NULL);
  tok->kind = TOK_LIT_STR;
  tok->loc = loc;
  tok->value = value;
  return tok;
}

token_lit_char_t* token_lit_char_init(location_t* loc, char value)
{
  token_lit_char_t* tok = (token_lit_char_t*)malloc(sizeof(token_lit_char_t));
  assert(tok != NULL);
  tok->kind = TOK_LIT_CHAR;
  tok->loc = loc;
  tok->value = value;
  return tok;
}

void token_free(token_t* tok)
{
  switch (tok->kind)
  {
  case TOK_ID:       free(((token_id_t*)      tok)->value); break;
  case TOK_LIT_STR:  free(((token_lit_str_t*) tok)->value); break;
  }

  location_free(tok->loc);
  free(tok);
}

void token_list_json_dump(FILE* stream, list_t* list)
{
  fputc('[', stream);

  for (list_node_t* elem = list_front_node(list); elem != NULL; elem = list_node_next(elem))
  {
    token_t* tok = (token_t*)list_node_get(elem);

    fprintf(stream, "{\"kind\":\"%s\"", token_kind_to_string(tok->kind));

    if (token_is_literal(tok) || tok->kind == TOK_ID)
    {
      fprintf(stream, ",\"value\":");

      switch (tok->kind)
      {
      case TOK_ID:
        fprintf(stream, "\"%s\"", ((token_id_t*)tok)->value);
        break;
      case TOK_LIT_INT:
        fprintf(stream, "%llu", ((token_lit_int_t*)tok)->value);
        break;
      case TOK_LIT_FLT:
        fprintf(stream, "%lf", ((token_lit_flt_t*)tok)->value);
        break;
      case TOK_LIT_STR:
        fprintf(stream, "\"%s\"", ((token_lit_str_t*)tok)->value);
        break;
      case TOK_LIT_CHAR:
        fprintf(stream, "\"%.*s\"", tok->loc->len - 2, tok->loc->cur + 1);
        break;
      case TOK_LIT_BOOL:
        fprintf(stream, "%s", ((token_lit_bool_t*)tok)->value ? "true" : "false");
        break;
      case TOK_LIT_NULL:
        fprintf(stream, "null");
        break;
      }
    }

    fputc('}', stream);

    if (list_node_next(elem) != NULL)
      fputc(',', stream);
  }

  fputc(']', stream);
}

const char* token_kind_to_string(token_kind_t kind)
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
    case TOK_KW_TYPEOF:                   return "TOK_KW_TYPEOF";
    case TOK_KW_IN:                       return "TOK_KW_IN";
    case TOK_KW_FUN:                      return "TOK_KW_FUN";
    case TOK_KW_GEN:                      return "TOK_KW_GEN";
    case TOK_KW_STRUCT:                   return "TOK_KW_STRUCT";
    case TOK_KW_UNION:                    return "TOK_KW_UNION";
    case TOK_KW_ENUM:                     return "TOK_KW_ENUM";
    case TOK_KW_MOD:                      return "TOK_KW_MOD";
    case TOK_KW_USE:                      return "TOK_KW_USE";
    case TOK_KW_IF:                       return "TOK_KW_IF";
    case TOK_KW_THEN:                     return "TOK_KW_THEN";
    case TOK_KW_ELSE:                     return "TOK_KW_ELSE";
    case TOK_KW_FOR:                      return "TOK_KW_FOR";
    case TOK_KW_WHILE:                    return "TOK_KW_WHILE";
    case TOK_KW_DO:                       return "TOK_KW_DO";
    case TOK_KW_BREAK:                    return "TOK_KW_BREAK";
    case TOK_KW_CONTINUE:                 return "TOK_KW_CONTINUE";
    case TOK_KW_RETURN:                   return "TOK_KW_RETURN";
    case TOK_KW_YIELD:                    return "TOK_KW_YIELD";
    case TOK_KW_MUT:                      return "TOK_KW_MUT";
    case TOK_KW_CONST:                    return "TOK_KW_CONST";
    case TOK_KW_TYPE:                     return "TOK_KW_TYPE";
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
    case TOK_KW_BOOL:                     return "TOK_KW_BOOL";
    case TOK_KW_UNIT:                     return "TOK_KW_UNIT";
    case TOK_KW_SELF:                     return "TOK_KW_SELF";
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
    case TOK_EOF:                         return "TOK_EOF";
    default: unreachable();
  }

  return NULL;
}

bool token_is_literal(token_t* tok)
{
  return tok->kind & TOK_FLAG_LIT;
}

bool token_is_literal_integer(token_t* tok)
{
  return tok->kind == TOK_LIT_INT;
}

bool token_is_literal_float(token_t* tok)
{
  return tok->kind == TOK_LIT_FLT;
}

bool token_is_keyword(token_t* tok)
{
  return tok->kind & TOK_FLAG_KW;
}

bool token_is_punctuation(token_t* tok)
{
  return tok->kind & TOK_FLAG_PUNCT;
}
