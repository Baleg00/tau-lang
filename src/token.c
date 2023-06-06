#include "token.h"

#include "util.h"

#include "list.h"

#include "location.h"

void token_init(token_t* tok, token_kind_t kind, location_t* loc)
{
  tok->kind = kind;
  tok->loc = loc;
}

void token_free(token_t* tok)
{
  location_free(tok->loc);
}

void token_list_json_dump(FILE* stream, list_t* list)
{
  fputc('[', stream);

  LIST_FOR_LOOP(it, list)
  {
    token_t* tok = (token_t*)list_node_get(it);

    fprintf(stream, "{\"kind\":\"%s\"", token_kind_to_string(tok->kind));

    if (token_is_literal(tok) || tok->kind == TOK_ID)
      fprintf(stream, ",\"value\":\"%.*s\"", (int)tok->loc->len, tok->loc->cur);

    fputc('}', stream);

    if (list_node_next(it) != NULL)
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
