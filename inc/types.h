#ifndef TAU_TYPES_H
#define TAU_TYPES_H

#include <stdbool.h>
#include <stdio.h>

#include "list.h"
#include "ast.h"

typedef enum type_kind_e
{
  TYPE_MUT,
  TYPE_CONST,
  TYPE_PTR,
  TYPE_ARRAY,
  TYPE_REF,
  TYPE_OPT,
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
  TYPE_STRUCT,
  TYPE_UNION,
  TYPE_ENUM,
  TYPE_MOD,
} type_kind_t;

typedef struct type_s type_t;
typedef struct type_member_s type_member_t;

struct type_member_s
{
  char* id;
  type_t* type;
};

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
      type_opt;

    struct {
      list_t* param_types;
      type_t* return_type;
    } type_fun;

    struct {
      list_t* param_types;
      type_t* yield_type;
    } type_gen;

    struct {
      ast_node_t* node;
      char* id;
      list_t* members;
    } type_struct,
      type_union;

    struct {
      ast_node_t* node;
      char* id;
      list_t* values;
    } type_enum;

    struct {
      ast_node_t* node;
      char* id;
      list_t* decls;
    } type_mod;
  };
};

type_member_t* type_member_init(char* id, type_t* type);
void type_member_free(type_member_t* member);

type_t* type_init(type_kind_t kind);
void type_free(type_t* type);

type_t* type_builtin(type_kind_t kind);

bool type_is_same(type_t* lhs, type_t* rhs);
bool type_is_integer(type_kind_t kind);
bool type_is_arithmetic(type_kind_t kind);
bool type_is_signed(type_kind_t kind);
bool type_is_owner(type_kind_t kind);
type_t* type_wider(type_t* a, type_t* b);

type_t* type_of(ast_node_t* node);

void type_print(FILE* stream, type_t* type);

#endif