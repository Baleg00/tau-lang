#include "token.h"

#include <stdlib.h>
#include <assert.h>

token_t* token_init(token_kind_t kind)
{
  token_t* tok = (token_t*)calloc(1, sizeof(token_t));
  assert(tok != NULL);
  tok->kind = kind;
  return tok;
}

void token_free(token_t* tok)
{
  free(tok);
}

token_kind_t token_kind(token_t* tok)
{
  return tok->kind;
}

token_t* token_next(token_t* tok)
{
  return tok->next;
}

token_t* token_prev(token_t* tok)
{
  return tok->prev;
}

location_t token_location(token_t* tok)
{
  return tok->loc;
}

token_list_t* token_list_init(void)
{
  token_list_t* list = (token_list_t*)calloc(1, sizeof(token_list_t));
  assert(list != NULL);
  return list;
}

void token_list_free(token_list_t* list)
{
  for (token_t *tok = list->root, *next; tok != NULL; tok = next)
  {
    next = tok->next;
    free(tok);
  }

  free(list);
}

void token_list_push(token_list_t* list, token_t* tok)
{
  tok->prev = list->tail;

  if (list->root == NULL)
    list->root = tok;
  else
    list->tail->next = tok;

  list->tail = tok;

  ++list->len;
}

token_t* token_list_pop(token_list_t* list)
{
  if (list->len == 0)
    return NULL;

  token_t* top = list->tail;

  if (list->root == list->tail)
    list->root = list->tail = NULL;
  else
  {
    list->tail = list->tail->prev;
    list->tail->next = NULL;
  }

  top->prev = NULL;

  --list->len;

  return top;
}

token_t* token_list_top(token_list_t* list)
{
  return list->len == 0 ? NULL : list->tail;
}

size_t token_list_size(token_list_t* list)
{
  return list->len;
}

bool token_list_empty(token_list_t* list)
{
  return list->len == 0;
}

const char* token_kind_to_string(token_kind_t kind)
{
  switch (kind)
  {
    case TOK_UNKNOWN: return "TOK_UNKNOWN";
    case TOK_ID: return "TOK_ID";
    case TOK_LIT_INT_DEC: return "TOK_LIT_INT_DEC";
    case TOK_LIT_INT_HEX: return "TOK_LIT_INT_HEX";
    case TOK_LIT_INT_OCT: return "TOK_LIT_INT_OCT";
    case TOK_LIT_INT_BIN: return "TOK_LIT_INT_BIN";
    case TOK_LIT_FLT_DEC: return "TOK_LIT_FLT_DEC";
    case TOK_LIT_FLT_HEX: return "TOK_LIT_FLT_HEX";
    case TOK_LIT_STR: return "TOK_LIT_STR";
    case TOK_LIT_CHAR: return "TOK_LIT_CHAR";
    case TOK_LIT_BOOL_TRUE: return "TOK_LIT_BOOL_TRUE";
    case TOK_LIT_BOOL_FALSE: return "TOK_LIT_BOOL_FALSE";
    case TOK_LIT_NULL: return "TOK_LIT_NULL";
    case TOK_KW_IS: return "TOK_KW_IS";
    case TOK_KW_AS: return "TOK_KW_AS";
    case TOK_KW_SIZEOF: return "TOK_KW_SIZEOF";
    case TOK_KW_ALIGNOF: return "TOK_KW_ALIGNOF";
    case TOK_KW_TYPEOF: return "TOK_KW_TYPEOF";
    case TOK_KW_IN: return "TOK_KW_IN";
    case TOK_KW_VAR: return "TOK_KW_VAR";
    case TOK_KW_FUN: return "TOK_KW_FUN";
    case TOK_KW_GEN: return "TOK_KW_GEN";
    case TOK_KW_STRUCT: return "TOK_KW_STRUCT";
    case TOK_KW_UNION: return "TOK_KW_UNION";
    case TOK_KW_ENUM: return "TOK_KW_ENUM";
    case TOK_KW_MOD: return "TOK_KW_MOD";
    case TOK_KW_USE: return "TOK_KW_USE";
    case TOK_KW_FROM: return "TOK_KW_FROM";
    case TOK_KW_IF: return "TOK_KW_IF";
    case TOK_KW_THEN: return "TOK_KW_THEN";
    case TOK_KW_ELSE: return "TOK_KW_ELSE";
    case TOK_KW_ELIF: return "TOK_KW_ELIF";
    case TOK_KW_FOR: return "TOK_KW_FOR";
    case TOK_KW_WHILE: return "TOK_KW_WHILE";
    case TOK_KW_WHEN: return "TOK_KW_WHEN";
    case TOK_KW_DO: return "TOK_KW_DO";
    case TOK_KW_BREAK: return "TOK_KW_BREAK";
    case TOK_KW_CONTINUE: return "TOK_KW_CONTINUE";
    case TOK_KW_RETURN: return "TOK_KW_RETURN";
    case TOK_KW_YIELD: return "TOK_KW_YIELD";
    case TOK_KW_PUB: return "TOK_KW_PUB";
    case TOK_KW_MUT: return "TOK_KW_MUT";
    case TOK_KW_CONST: return "TOK_KW_CONST";
    case TOK_KW_STATIC: return "TOK_KW_STATIC";
    case TOK_KW_I8: return "TOK_KW_I8";
    case TOK_KW_I16: return "TOK_KW_I16";
    case TOK_KW_I32: return "TOK_KW_I32";
    case TOK_KW_I64: return "TOK_KW_I64";
    case TOK_KW_ISIZE: return "TOK_KW_ISIZE";
    case TOK_KW_U8: return "TOK_KW_U8";
    case TOK_KW_U16: return "TOK_KW_U16";
    case TOK_KW_U32: return "TOK_KW_U32";
    case TOK_KW_U64: return "TOK_KW_U64";
    case TOK_KW_USIZE: return "TOK_KW_USIZE";
    case TOK_KW_F32: return "TOK_KW_F32";
    case TOK_KW_F64: return "TOK_KW_F64";
    case TOK_KW_BOOL: return "TOK_KW_BOOL";
    case TOK_KW_UNIT: return "TOK_KW_UNIT";
    case TOK_PUNCT_PLUS: return "TOK_PUNCT_PLUS";
    case TOK_PUNCT_PLUS_PLUS: return "TOK_PUNCT_PLUS_PLUS";
    case TOK_PUNCT_PLUS_EQUAL: return "TOK_PUNCT_PLUS_EQUAL";
    case TOK_PUNCT_MINUS: return "TOK_PUNCT_MINUS";
    case TOK_PUNCT_MINUS_MINUS: return "TOK_PUNCT_MINUS_MINUS";
    case TOK_PUNCT_MINUS_EQUAL: return "TOK_PUNCT_MINUS_EQUAL";
    case TOK_PUNCT_ASTERISK: return "TOK_PUNCT_ASTERISK";
    case TOK_PUNCT_ASTERISK_EQUAL: return "TOK_PUNCT_ASTERISK_EQUAL";
    case TOK_PUNCT_ASTERISK_DOT: return "TOK_PUNCT_ASTERISK_DOT";
    case TOK_PUNCT_SLASH: return "TOK_PUNCT_SLASH";
    case TOK_PUNCT_SLASH_EQUAL: return "TOK_PUNCT_SLASH_EQUAL";
    case TOK_PUNCT_PERCENT: return "TOK_PUNCT_PERCENT";
    case TOK_PUNCT_PERCENT_EQUAL: return "TOK_PUNCT_PERCENT_EQUAL";
    case TOK_PUNCT_AMPERSAND: return "TOK_PUNCT_AMPERSAND";
    case TOK_PUNCT_AMPERSAND_AMPERSAND: return "TOK_PUNCT_AMPERSAND_AMPERSAND";
    case TOK_PUNCT_AMPERSAND_EQUAL: return "TOK_PUNCT_AMPERSAND_EQUAL";
    case TOK_PUNCT_BAR: return "TOK_PUNCT_BAR";
    case TOK_PUNCT_BAR_BAR: return "TOK_PUNCT_BAR_BAR";
    case TOK_PUNCT_BAR_EQUAL: return "TOK_PUNCT_BAR_EQUAL";
    case TOK_PUNCT_HAT: return "TOK_PUNCT_HAT";
    case TOK_PUNCT_HAT_EQUAL: return "TOK_PUNCT_HAT_EQUAL";
    case TOK_PUNCT_TILDE: return "TOK_PUNCT_TILDE";
    case TOK_PUNCT_LESS: return "TOK_PUNCT_LESS";
    case TOK_PUNCT_LESS_LESS: return "TOK_PUNCT_LESS_LESS";
    case TOK_PUNCT_LESS_LESS_EQUAL: return "TOK_PUNCT_LESS_LESS_EQUAL";
    case TOK_PUNCT_LESS_EQUAL: return "TOK_PUNCT_LESS_EQUAL";
    case TOK_PUNCT_GREATER: return "TOK_PUNCT_GREATER";
    case TOK_PUNCT_GREATER_GREATER: return "TOK_PUNCT_GREATER_GREATER";
    case TOK_PUNCT_GREATER_GREATER_EQUAL: return "TOK_PUNCT_GREATER_GREATER_EQUAL";
    case TOK_PUNCT_GREATER_EQUAL: return "TOK_PUNCT_GREATER_EQUAL";
    case TOK_PUNCT_BANG: return "TOK_PUNCT_BANG";
    case TOK_PUNCT_BANG_EQUAL: return "TOK_PUNCT_BANG_EQUAL";
    case TOK_PUNCT_DOT: return "TOK_PUNCT_DOT";
    case TOK_PUNCT_DOT_DOT: return "TOK_PUNCT_DOT_DOT";
    case TOK_PUNCT_DOT_DOT_DOT: return "TOK_PUNCT_DOT_DOT_DOT";
    case TOK_PUNCT_QUESTION: return "TOK_PUNCT_QUESTION";
    case TOK_PUNCT_QUESTION_DOT: return "TOK_PUNCT_QUESTION_DOT";
    case TOK_PUNCT_EQUAL: return "TOK_PUNCT_EQUAL";
    case TOK_PUNCT_EQUAL_EQUAL: return "TOK_PUNCT_EQUAL_EQUAL";
    case TOK_PUNCT_COMMA: return "TOK_PUNCT_COMMA";
    case TOK_PUNCT_COLON: return "TOK_PUNCT_COLON";
    case TOK_PUNCT_PAREN_LEFT: return "TOK_PUNCT_PAREN_LEFT";
    case TOK_PUNCT_PAREN_RIGHT: return "TOK_PUNCT_PAREN_RIGHT";
    case TOK_PUNCT_BRACKET_LEFT: return "TOK_PUNCT_BRACKET_LEFT";
    case TOK_PUNCT_BRACKET_RIGHT: return "TOK_PUNCT_BRACKET_RIGHT";
    case TOK_PUNCT_BRACE_LEFT: return "TOK_PUNCT_BRACE_LEFT";
    case TOK_PUNCT_BRACE_RIGHT: return "TOK_PUNCT_BRACE_RIGHT";
    case TOK_EOF: return "TOK_EOF";
    default: return "";
  }
}

bool token_is_literal(token_kind_t kind)
{
  return TOK_LIT_INT_DEC <= kind && TOK_LIT_NULL >= kind;
}

bool token_is_literal_integer(token_kind_t kind)
{
  return TOK_LIT_INT_DEC <= kind && TOK_LIT_INT_BIN >= kind;
}

bool token_is_literal_float(token_kind_t kind)
{
  return TOK_LIT_FLT_DEC == kind || TOK_LIT_FLT_HEX == kind;
}

bool token_is_keyword(token_kind_t kind)
{
  return TOK_KW_IS <= kind && TOK_KW_STATIC >= kind;
}

bool token_is_punctuation(token_kind_t kind)
{
  return TOK_PUNCT_PLUS <= kind && TOK_PUNCT_BRACE_RIGHT >= kind;
}
