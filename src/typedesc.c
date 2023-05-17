#include "typedesc.h"

#include "util.h"

#include "list.h"

#include "location.h"
#include "token.h"
#include "op.h"
#include "ast.h"

#include "memtrace.h"

typedesc_t* typedesc_init(typedesc_kind_t kind, size_t size)
{
  typedesc_t* desc = (typedesc_t*)calloc(1, size);
  assert(desc != NULL);

  desc->kind = kind;
  desc->size = 0;
  desc->align = 0;
  
  return desc;
}

typedesc_t* typedesc_builtin(typedesc_kind_t kind)
{
  static typedesc_t desc_type  = { .kind = TYPEDESC_TYPE,  .size = 0, .align = 0 };
  static typedesc_t desc_i8    = { .kind = TYPEDESC_I8,    .size = 1, .align = 1 };
  static typedesc_t desc_i16   = { .kind = TYPEDESC_I16,   .size = 2, .align = 2 };
  static typedesc_t desc_i32   = { .kind = TYPEDESC_I32,   .size = 4, .align = 4 };
  static typedesc_t desc_i64   = { .kind = TYPEDESC_I64,   .size = 8, .align = 8 };
  static typedesc_t desc_isize = { .kind = TYPEDESC_ISIZE, .size = 8, .align = 8 };
  static typedesc_t desc_u8    = { .kind = TYPEDESC_U8,    .size = 1, .align = 1 };
  static typedesc_t desc_u16   = { .kind = TYPEDESC_U16,   .size = 2, .align = 2 };
  static typedesc_t desc_u32   = { .kind = TYPEDESC_U32,   .size = 4, .align = 4 };
  static typedesc_t desc_u64   = { .kind = TYPEDESC_U64,   .size = 8, .align = 8 };
  static typedesc_t desc_usize = { .kind = TYPEDESC_USIZE, .size = 8, .align = 8 };
  static typedesc_t desc_f32   = { .kind = TYPEDESC_F32,   .size = 4, .align = 4 };
  static typedesc_t desc_f64   = { .kind = TYPEDESC_F64,   .size = 8, .align = 8 };
  static typedesc_t desc_bool  = { .kind = TYPEDESC_BOOL,  .size = 1, .align = 1 };
  static typedesc_t desc_unit  = { .kind = TYPEDESC_UNIT,  .size = 0, .align = 0 };
  static typedesc_t desc_null  = { .kind = TYPEDESC_NULL,  .size = 0, .align = 0 };

  switch (kind)
  {
  case TYPEDESC_TYPE:  return &desc_type;
  case TYPEDESC_I8:    return &desc_i8;
  case TYPEDESC_I16:   return &desc_i16;
  case TYPEDESC_I32:   return &desc_i32;
  case TYPEDESC_I64:   return &desc_i64;
  case TYPEDESC_ISIZE: return &desc_isize;
  case TYPEDESC_U8:    return &desc_u8;
  case TYPEDESC_U16:   return &desc_u16;
  case TYPEDESC_U32:   return &desc_u32;
  case TYPEDESC_U64:   return &desc_u64;
  case TYPEDESC_USIZE: return &desc_usize;
  case TYPEDESC_F32:   return &desc_f32;
  case TYPEDESC_F64:   return &desc_f64;
  case TYPEDESC_BOOL:  return &desc_bool;
  case TYPEDESC_UNIT:  return &desc_unit;
  case TYPEDESC_NULL:  return &desc_null;
  default: unreachable();
  }

  return NULL;
}

LIST_FOR_EACH_FUNC_DECL(typedesc_free, typedesc_t);

void typedesc_free(typedesc_t* type)
{
  if (type == NULL)
    return;

  switch (type->kind)
  {
  case TYPEDESC_MUT:
    typedesc_free(((typedesc_mut_t*)type)->base_type);
    break;
  case TYPEDESC_CONST:
    typedesc_free(((typedesc_const_t*)type)->base_type);
    break;
  case TYPEDESC_PTR:
    typedesc_free(((typedesc_ptr_t*)type)->base_type);
    break;
  case TYPEDESC_ARRAY:
    typedesc_free(((typedesc_array_t*)type)->base_type);
    break;
  case TYPEDESC_REF:
    typedesc_free(((typedesc_ref_t*)type)->base_type);
    break;
  case TYPEDESC_OPT:
    typedesc_free(((typedesc_opt_t*)type)->base_type);
    break;
  case TYPEDESC_FUN:
    if (((typedesc_fun_t*)type)->param_types != NULL)
    {
      list_for_each(((typedesc_fun_t*)type)->param_types, LIST_FOR_EACH_FUNC_NAME(typedesc_free));
      list_free(((typedesc_fun_t*)type)->param_types);
    }

    typedesc_free(((typedesc_fun_t*)type)->return_type);
    break;
  case TYPEDESC_GEN:
    if (((typedesc_gen_t*)type)->param_types != NULL)
    {
      list_for_each(((typedesc_gen_t*)type)->param_types, LIST_FOR_EACH_FUNC_NAME(typedesc_free));
      list_free(((typedesc_gen_t*)type)->param_types);
    }

    typedesc_free(((typedesc_gen_t*)type)->yield_type);
    break;
  case TYPEDESC_TYPE:
  case TYPEDESC_I8:
  case TYPEDESC_I16:
  case TYPEDESC_I32:
  case TYPEDESC_I64:
  case TYPEDESC_ISIZE:
  case TYPEDESC_U8:
  case TYPEDESC_U16:
  case TYPEDESC_U32:
  case TYPEDESC_U64:
  case TYPEDESC_USIZE:
  case TYPEDESC_F32:
  case TYPEDESC_F64:
  case TYPEDESC_BOOL:
  case TYPEDESC_UNIT:
  case TYPEDESC_NULL:
    return;
  case TYPEDESC_STRUCT:
    if (((typedesc_struct_t*)type)->member_types != NULL)
    {
      list_for_each(((typedesc_struct_t*)type)->member_types, LIST_FOR_EACH_FUNC_NAME(typedesc_free));
      list_free(((typedesc_struct_t*)type)->member_types);
    }
    break;
  case TYPEDESC_UNION:
    if (((typedesc_union_t*)type)->member_types != NULL)
    {
      list_for_each(((typedesc_union_t*)type)->member_types, LIST_FOR_EACH_FUNC_NAME(typedesc_free));
      list_free(((typedesc_union_t*)type)->member_types);
    }
    break;
  case TYPEDESC_ENUM:
    break;
  case TYPEDESC_MOD:
    if (((typedesc_mod_t*)type)->member_types != NULL)
    {
      list_for_each(((typedesc_mod_t*)type)->member_types, LIST_FOR_EACH_FUNC_NAME(typedesc_free));
      list_free(((typedesc_mod_t*)type)->member_types);
    }
    break;
  default:
    unreachable();
  }

  free(type);
}

bool typedesc_is_same_fun(typedesc_fun_t* lhs, typedesc_fun_t* rhs)
{
  if ((lhs->param_types == NULL) != (rhs->param_types == NULL))
      return false;

  if (lhs->param_types == NULL)
    return true;

  if (list_size(lhs->param_types) != list_size(rhs->param_types))
    return false;
  
  if (list_size(lhs->param_types) == 0)
    return true;

  for (list_node_t *lit = list_front_node(lhs->param_types), *rit = list_front_node(rhs->param_types);
        lit != NULL && rit != NULL;
        lit = list_node_next(lit), rit = list_node_next(rit))
    if (!typedesc_is_same((typedesc_t*)list_node_get(lit), (typedesc_t*)list_node_get(rit)))
      return false;

  return typedesc_is_same(lhs->return_type, rhs->return_type);
}

bool typedesc_is_same_gen(typedesc_gen_t* lhs, typedesc_gen_t* rhs)
{
  if ((lhs->param_types == NULL) != (rhs->param_types == NULL))
    return false;

  if (lhs->param_types == NULL)
    return true;

  if (list_size(lhs->param_types) != list_size(rhs->param_types))
    return false;
  
  if (list_size(lhs->param_types) == 0)
    return true;

  for (list_node_t *lit = list_front_node(lhs->param_types),
                   *rit = list_front_node(rhs->param_types);
        lit != NULL && rit != NULL;
        lit = list_node_next(lit), rit = list_node_next(rit))
    if (!typedesc_is_same((typedesc_t*)list_node_get(lit), (typedesc_t*)list_node_get(rit)))
      return false;

  return typedesc_is_same(lhs->yield_type, rhs->yield_type);
}

bool typedesc_is_modifier(typedesc_t* type)
{
  return type->kind & TYPEDESC_FLAG_MODIFIER;
}

bool typedesc_is_builtin(typedesc_t* type)
{
  return type->kind & TYPEDESC_FLAG_BUILTIN;
}

bool typedesc_is_integer(typedesc_t* type)
{
  return type->kind & TYPEDESC_FLAG_INTEGER;
}

bool typedesc_is_float(typedesc_t* type)
{
  return type->kind & TYPEDESC_FLAG_FLOAT;
}

bool typedesc_is_arithmetic(typedesc_t* type)
{
  return type->kind & TYPEDESC_FLAG_ARITHMETIC;
}

bool typedesc_is_signed(typedesc_t* type)
{
  return type->kind & TYPEDESC_FLAG_SIGNED;
}

bool typedesc_is_unsigned(typedesc_t* type)
{
  return type->kind & TYPEDESC_FLAG_UNSIGNED;
}

bool typedesc_is_owner(typedesc_t* type)
{
  return type->kind & TYPEDESC_FLAG_OWNER;
}

bool typedesc_is_same(typedesc_t* lhs, typedesc_t* rhs)
{
  if (lhs->kind != rhs->kind)
    return false;

  switch (lhs->kind)
  {
  case TYPEDESC_MUT:    return typedesc_is_same(((typedesc_mut_t*)lhs)->base_type, ((typedesc_mut_t*)rhs)->base_type);
  case TYPEDESC_CONST:  return typedesc_is_same(((typedesc_const_t*)lhs)->base_type, ((typedesc_const_t*)rhs)->base_type);
  case TYPEDESC_PTR:    return typedesc_is_same(((typedesc_ptr_t*)lhs)->base_type, ((typedesc_ptr_t*)rhs)->base_type);
  case TYPEDESC_ARRAY:  return typedesc_is_same(((typedesc_array_t*)lhs)->base_type, ((typedesc_array_t*)rhs)->base_type);
  case TYPEDESC_REF:    return typedesc_is_same(((typedesc_ref_t*)lhs)->base_type, ((typedesc_ref_t*)rhs)->base_type);
  case TYPEDESC_OPT:    return typedesc_is_same(((typedesc_opt_t*)lhs)->base_type, ((typedesc_opt_t*)rhs)->base_type);
  case TYPEDESC_FUN:    return typedesc_is_same_fun((typedesc_fun_t*)lhs, (typedesc_fun_t*)rhs);
  case TYPEDESC_GEN:    return typedesc_is_same_gen((typedesc_gen_t*)lhs, (typedesc_gen_t*)rhs);
  case TYPEDESC_TYPE:
  case TYPEDESC_I8:
  case TYPEDESC_I16:
  case TYPEDESC_I32:
  case TYPEDESC_I64:
  case TYPEDESC_ISIZE:
  case TYPEDESC_U8:
  case TYPEDESC_U16:
  case TYPEDESC_U32:
  case TYPEDESC_U64:
  case TYPEDESC_USIZE:
  case TYPEDESC_F32:
  case TYPEDESC_F64:
  case TYPEDESC_BOOL:
  case TYPEDESC_UNIT:   return true;
  case TYPEDESC_STRUCT:
  case TYPEDESC_UNION:
  case TYPEDESC_ENUM:
  case TYPEDESC_MOD:    return ((typedesc_decl_t*)lhs)->node == ((typedesc_decl_t*)rhs)->node;
  default: unreachable();
  }

  return false;
}

bool typedesc_is_assignable(typedesc_t* to, typedesc_t* from)
{
  if (to->kind == TYPEDESC_REF && from->kind != TYPEDESC_REF)
    return false;

  return typedesc_is_same(typedesc_remove_ref(to), typedesc_remove_ref(from));
}

typedesc_t* typedesc_make_signed(typedesc_t* type)
{
  assert(typedesc_is_integer(type));

  switch (type->kind)
  {
  case TYPEDESC_U8: return typedesc_builtin(TYPEDESC_I8);
  case TYPEDESC_U16: return typedesc_builtin(TYPEDESC_I16);
  case TYPEDESC_U32: return typedesc_builtin(TYPEDESC_I32);
  case TYPEDESC_U64: return typedesc_builtin(TYPEDESC_I64);
  case TYPEDESC_USIZE: return typedesc_builtin(TYPEDESC_ISIZE);
  }

  return type;
}

typedesc_t* typedesc_make_ptr(typedesc_t* type)
{
  typedesc_ptr_t* ptr_type = typedesc_ptr_init();
  ptr_type->base_type = type;
  return (typedesc_t*)ptr_type;
}

typedesc_t* typedesc_make_ref(typedesc_t* type)
{
  typedesc_ref_t* ref_type = typedesc_ref_init();
  ref_type->base_type = type;
  return (typedesc_t*)ref_type;
}

typedesc_t* typedesc_remove_ref(typedesc_t* type)
{
  return type->kind == TYPEDESC_REF ? ((typedesc_ref_t*)type)->base_type : type;
}

typedesc_t* typedesc_promote(typedesc_t* lhs, typedesc_t* rhs)
{
  assert(typedesc_is_arithmetic(lhs) && typedesc_is_arithmetic(rhs));

  if (typedesc_is_integer(lhs) != typedesc_is_integer(rhs))
    return typedesc_is_float(lhs) ? lhs : rhs;
  
  if (typedesc_is_integer(lhs))
    if (typedesc_is_signed(lhs) != typedesc_is_signed(rhs))
      return typedesc_promote(typedesc_make_signed(lhs), typedesc_make_signed(rhs));

  return lhs->size > rhs->size ? lhs : rhs;
}

size_t typedesc_print(FILE* stream, typedesc_t* type)
{
  size_t count = 0;

  switch (type->kind)
  {
  case TYPEDESC_MUT:
    count += (size_t)fprintf(stream, "mut ");
    count += typedesc_print(stream, ((typedesc_mut_t*)type)->base_type);
    break;
  case TYPEDESC_CONST:
    count += (size_t)fprintf(stream, "const ");
    count += typedesc_print(stream, ((typedesc_const_t*)type)->base_type);
    break;
  case TYPEDESC_PTR:
    count++; fputc('*', stream);
    count += typedesc_print(stream, ((typedesc_ptr_t*)type)->base_type);
    break;
  case TYPEDESC_ARRAY:
    count += (size_t)fprintf(stream, "[]");
    count += typedesc_print(stream, ((typedesc_array_t*)type)->base_type);
    break;
  case TYPEDESC_REF:
    count++; fputc('&', stream);
    count += typedesc_print(stream, ((typedesc_ref_t*)type)->base_type);
    break;
  case TYPEDESC_OPT:
    count++; fputc('?', stream);
    count += typedesc_print(stream, ((typedesc_opt_t*)type)->base_type);
    break;
  case TYPEDESC_FUN:
    count += (size_t)fprintf(stream, "fun(");

    if (((typedesc_fun_t*)type)->param_types != NULL)
      LIST_FOR_LOOP(it, ((typedesc_fun_t*)type)->param_types)
      {
        count += typedesc_print(stream, (typedesc_t*)list_node_get(it));

        if (list_node_next(it) != NULL)
          count += (size_t)fprintf(stream, ", ");
      }

    count += (size_t)fprintf(stream, "): ");
    count += typedesc_print(stream, ((typedesc_fun_t*)type)->return_type);
    break;
  case TYPEDESC_GEN:
    count += (size_t)fprintf(stream, "gen(");

    if (((typedesc_gen_t*)type)->param_types != NULL)
      LIST_FOR_LOOP(it, ((typedesc_gen_t*)type)->param_types)
      {
        count += typedesc_print(stream, (typedesc_t*)list_node_get(it));

        if (list_node_next(it) != NULL)
          count += (size_t)fprintf(stream, ", ");
      }

    count += (size_t)fprintf(stream, "): ");
    count += typedesc_print(stream, ((typedesc_gen_t*)type)->yield_type);
    break;
  case TYPEDESC_I8:    count += (size_t)fprintf(stream, "i8"); break;
  case TYPEDESC_I16:   count += (size_t)fprintf(stream, "i16"); break;
  case TYPEDESC_I32:   count += (size_t)fprintf(stream, "i32"); break;
  case TYPEDESC_I64:   count += (size_t)fprintf(stream, "i64"); break;
  case TYPEDESC_ISIZE: count += (size_t)fprintf(stream, "isize"); break;
  case TYPEDESC_U8:    count += (size_t)fprintf(stream, "u8"); break;
  case TYPEDESC_U16:   count += (size_t)fprintf(stream, "u16"); break;
  case TYPEDESC_U32:   count += (size_t)fprintf(stream, "u32"); break;
  case TYPEDESC_U64:   count += (size_t)fprintf(stream, "u64"); break;
  case TYPEDESC_USIZE: count += (size_t)fprintf(stream, "usize"); break;
  case TYPEDESC_F32:   count += (size_t)fprintf(stream, "f32"); break;
  case TYPEDESC_F64:   count += (size_t)fprintf(stream, "f64"); break;
  case TYPEDESC_BOOL:  count += (size_t)fprintf(stream, "bool"); break;
  case TYPEDESC_UNIT:  count += (size_t)fprintf(stream, "unit"); break;
  case TYPEDESC_STRUCT:
    count += (size_t)fprintf(stream, "%.*s", ast_id_len(((typedesc_struct_t*)type)->node->id), ast_id_ptr(((typedesc_struct_t*)type)->node->id));
    break;
  case TYPEDESC_UNION:
    count += (size_t)fprintf(stream, "%.*s", ast_id_len(((typedesc_union_t*)type)->node->id), ast_id_ptr(((typedesc_union_t*)type)->node->id));
    break;
  case TYPEDESC_ENUM:
    count += (size_t)fprintf(stream, "%.*s", ast_id_len(((typedesc_enum_t*)type)->node->id), ast_id_ptr(((typedesc_enum_t*)type)->node->id));
    break;
  default:
    unreachable();
  }

  return count;
}
