#ifndef TAU_TYPES_H
#define TAU_TYPES_H

#include <stdbool.h>

#include "list.h"
#include "ast.h"

typedef enum type_kind_e
{
  TYPE_MUT,
  TYPE_CONST,
  TYPE_PTR,
  TYPE_ARRAY,
  TYPE_REF,
  TYPE_NULLABLE,
  TYPE_FUN,
  TYPE_GEN,
  TYPE_TYPE,
  TYPE_I8,
  TYPE_I16,
  TYPE_I32,
  TYPE_I64,
  TYPE_ISIZE,
  TYPE_U8,
  TYPE_U16,
  TYPE_U32,
  TYPE_U64,
  TYPE_USIZE,
  TYPE_F32,
  TYPE_F64,
  TYPE_BOOL,
  TYPE_UNIT,
  TYPE_NULL,
  TYPE_STR,
  TYPE_DECL
} type_kind_t;

typedef struct type_s type_t;

struct type_s
{
  type_kind_t kind;

  union {
    struct {
      type_t* base_type;
    } type_mut,
      type_const,
      type_ptr,
      type_array,
      type_ref,
      type_nullable;

    struct {
      list_t* param_types;
      type_t *ret_type;
    } type_fun,
      type_gen;

    struct {
      ast_node_t* node;
    } type_decl;
  };
};

type_t* type_init(type_kind_t kind);
void type_free(type_t* type);

type_t* type_builtin(type_kind_t kind);

bool type_is_same(type_t* lhs, type_t* rhs);
bool type_is_integer(type_kind_t kind);
bool type_is_arithmetic(type_kind_t kind);
bool type_is_signed(type_kind_t kind);
type_t* type_wider(type_t* a, type_t* b);

type_t* type_ast_type_to_type(ast_node_t* node);
type_t* type_of(ast_node_t* node);

#endif