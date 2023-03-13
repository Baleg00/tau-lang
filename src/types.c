#include "types.h"

#include "memtrace.h"
#include "util.h"

type_t* type_init(type_kind_t kind)
{
  type_t* type = (type_t*)malloc(sizeof(type_t));
  assert(type != NULL);
  type->kind = kind;
  return type;
}

void type_free(type_t* type)
{
  if (type == NULL)
    return;

  switch (type->kind)
  {
  case TYPE_MUT:
    type_free(type->type_mut.base_type);
    break;
  case TYPE_CONST:
    type_free(type->type_const.base_type);
    break;
  case TYPE_PTR:
    type_free(type->type_ptr.base_type);
    break;
  case TYPE_ARRAY:
    type_free(type->type_array.base_type);
    break;
  case TYPE_REF:
    type_free(type->type_ref.base_type);
    break;
  case TYPE_NULLABLE:
    type_free(type->type_mut.base_type);
    break;
  case TYPE_FUN:
    if (type->type_fun.param_types != NULL)
    {
      list_for_each(type->type_fun.param_types, type_free);
      list_free(type->type_fun.param_types);
    }
    type_free(type->type_fun.ret_type);
    break;
  case TYPE_GEN:
    if (type->type_gen.param_types != NULL)
    {
      list_for_each(type->type_gen.param_types, type_free);
      list_free(type->type_gen.param_types);
    }
    type_free(type->type_gen.ret_type);
    break;
  case TYPE_TYPE:
  case TYPE_I8:
  case TYPE_I16:
  case TYPE_I32:
  case TYPE_I64:
  case TYPE_ISIZE:
  case TYPE_U8:
  case TYPE_U16:
  case TYPE_U32:
  case TYPE_U64:
  case TYPE_USIZE:
  case TYPE_F32:
  case TYPE_F64:
  case TYPE_BOOL:
  case TYPE_UNIT:
    return;
  case TYPE_DECL:
    break;
  default:
    unreachable();
  }

  free(type);
}

type_t* type_builtin(type_kind_t kind)
{
  static type_t type_type  = { .kind = TYPE_TYPE  };
  static type_t type_i8    = { .kind = TYPE_I8    };
  static type_t type_i16   = { .kind = TYPE_I16   };
  static type_t type_i32   = { .kind = TYPE_I32   };
  static type_t type_i64   = { .kind = TYPE_I64   };
  static type_t type_isize = { .kind = TYPE_ISIZE };
  static type_t type_u8    = { .kind = TYPE_U8    };
  static type_t type_u16   = { .kind = TYPE_U16   };
  static type_t type_u32   = { .kind = TYPE_U32   };
  static type_t type_u64   = { .kind = TYPE_U64   };
  static type_t type_usize = { .kind = TYPE_USIZE };
  static type_t type_f32   = { .kind = TYPE_F32   };
  static type_t type_f64   = { .kind = TYPE_F64   };
  static type_t type_bool  = { .kind = TYPE_BOOL  };
  static type_t type_unit  = { .kind = TYPE_UNIT  };
  static type_t type_null  = { .kind = TYPE_NULL  };
  static type_t type_str   = { .kind = TYPE_PTR, .type_ptr.base_type = &type_u8 };

  switch (kind)
  {
  case TYPE_TYPE:  return &type_type;
  case TYPE_I8:    return &type_i8;
  case TYPE_I16:   return &type_i16;
  case TYPE_I32:   return &type_i32;
  case TYPE_I64:   return &type_i64;
  case TYPE_ISIZE: return &type_isize;
  case TYPE_U8:    return &type_u8;
  case TYPE_U16:   return &type_u16;
  case TYPE_U32:   return &type_u32;
  case TYPE_U64:   return &type_u64;
  case TYPE_USIZE: return &type_usize;
  case TYPE_F32:   return &type_f32;
  case TYPE_F64:   return &type_f64;
  case TYPE_BOOL:  return &type_bool;
  case TYPE_UNIT:  return &type_unit;
  case TYPE_NULL:  return &type_null;
  case TYPE_STR:   return &type_str;
  default:
    unreachable();
  }

  return NULL;
}

bool type_is_same(type_t* lhs, type_t* rhs)
{
  if (lhs->kind != rhs->kind)
    return false;

  switch (lhs->kind)
  {
  case TYPE_MUT:
    return type_is_same(lhs->type_mut.base_type, rhs->type_mut.base_type);
  case TYPE_CONST:
    return type_is_same(lhs->type_const.base_type, rhs->type_const.base_type);
  case TYPE_PTR:
    return type_is_same(lhs->type_ptr.base_type, rhs->type_ptr.base_type);
  case TYPE_ARRAY:
    return type_is_same(lhs->type_array.base_type, rhs->type_array.base_type);
  case TYPE_REF:
    return type_is_same(lhs->type_ref.base_type, rhs->type_ref.base_type);
  case TYPE_NULLABLE:
    return type_is_same(lhs->type_nullable.base_type, rhs->type_nullable.base_type);
  case TYPE_FUN:
    if ((lhs->type_fun.param_types == NULL) != (rhs->type_fun.param_types == NULL))
      return false;

    if (lhs->type_fun.param_types == NULL)
      return true;

    if (list_size(lhs->type_fun.param_types) != list_size(rhs->type_fun.param_types))
      return false;
    
    if (list_size(lhs->type_fun.param_types) == 0)
      return true;

    for (list_elem_t *lit = list_front_elem(lhs->type_fun.param_types), *rit = list_front_elem(rhs->type_fun.param_types);
         lit != NULL && rit != NULL;
         lit = list_elem_next(lit), rit = list_elem_next(rit))
      if (!type_is_same((type_t*)list_elem_get(lit), (type_t*)list_elem_get(rit)))
        return false;

    return type_is_same(lhs->type_fun.ret_type, rhs->type_fun.ret_type);
  case TYPE_GEN:
    if ((lhs->type_gen.param_types == NULL) != (rhs->type_gen.param_types == NULL))
      return false;

    if (lhs->type_gen.param_types == NULL)
      return true;

    if (list_size(lhs->type_gen.param_types) != list_size(rhs->type_gen.param_types))
      return false;
    
    if (list_size(lhs->type_gen.param_types) == 0)
      return true;

    for (list_elem_t *lit = list_front_elem(lhs->type_gen.param_types), *rit = list_front_elem(rhs->type_gen.param_types);
         lit != NULL && rit != NULL;
         lit = list_elem_next(lit), rit = list_elem_next(rit))
      if (!type_is_same((type_t*)list_elem_get(lit), (type_t*)list_elem_get(rit)))
        return false;

    return type_is_same(lhs->type_gen.ret_type, rhs->type_gen.ret_type);
  case TYPE_TYPE:
  case TYPE_I8:
  case TYPE_I16:
  case TYPE_I32:
  case TYPE_I64:
  case TYPE_ISIZE:
  case TYPE_U8:
  case TYPE_U16:
  case TYPE_U32:
  case TYPE_U64:
  case TYPE_USIZE:
  case TYPE_F32:
  case TYPE_F64:
  case TYPE_BOOL:
  case TYPE_UNIT:
    return true;
  case TYPE_DECL:
    return lhs->type_decl.node == rhs->type_decl.node;
  default:
    unreachable();
  }

  return false;
}

bool type_is_integer(type_kind_t kind)
{
  switch (kind)
  {
  case TYPE_I8:
  case TYPE_I16:
  case TYPE_I32:
  case TYPE_I64:
  case TYPE_ISIZE:
  case TYPE_U8:
  case TYPE_U16:
  case TYPE_U32:
  case TYPE_U64:
  case TYPE_USIZE:
    return true;
  }

  return false;
}

bool type_is_arithmetic(type_kind_t kind)
{
  switch (kind)
  {
  case TYPE_I8:
  case TYPE_I16:
  case TYPE_I32:
  case TYPE_I64:
  case TYPE_ISIZE:
  case TYPE_U8:
  case TYPE_U16:
  case TYPE_U32:
  case TYPE_U64:
  case TYPE_USIZE:
  case TYPE_F32:
  case TYPE_F64:
    return true;
  }

  return false;
}

bool type_is_signed(type_kind_t kind)
{
  switch (kind)
  {
  case TYPE_I8:
  case TYPE_I16:
  case TYPE_I32:
  case TYPE_I64:
  case TYPE_ISIZE:
  case TYPE_F32:
  case TYPE_F64:
    return true;
  }

  return false;
}

type_t* type_wider(type_t* a, type_t* b)
{
  assert(type_is_arithmetic(a->kind) && type_is_arithmetic(b->kind));

  if (TYPE_I8 <= a->kind && a->kind <= TYPE_ISIZE && 
      TYPE_I8 <= b->kind && b->kind <= TYPE_ISIZE)
    return a->kind - b->kind > 0 ? a : b;

  if (TYPE_U8 <= a->kind && a->kind <= TYPE_USIZE && 
      TYPE_U8 <= b->kind && b->kind <= TYPE_USIZE)
    return a->kind - b->kind > 0 ? a : b;

  if ((a->kind == TYPE_F32 || a->kind == TYPE_F64) && 
      (b->kind == TYPE_F32 || b->kind == TYPE_F64))
    return a->kind - b->kind > 0 ? a : b;

  if (TYPE_I8 <= a->kind && a->kind <= TYPE_ISIZE && 
      (b->kind == TYPE_F32 || b->kind == TYPE_F64))
    return b;

  if ((a->kind == TYPE_F32 || a->kind == TYPE_F64) && 
      TYPE_I8 <= b->kind && b->kind <= TYPE_ISIZE)
    return a;

  unreachable();

  return NULL;
}

type_t* type_ast_type_to_type(ast_node_t* node)
{
  type_t* type = NULL;

  switch (node->kind)
  {
  case AST_TYPE_MUT:
    type = type_init(TYPE_MUT);
    type->type_mut.base_type = type_ast_type_to_type(node->type_mut.base_type);
    return type;
  case AST_TYPE_CONST:
    type = type_init(TYPE_CONST);
    type->type_const.base_type = type_ast_type_to_type(node->type_const.base_type);
    return type;
  case AST_TYPE_PTR:
    type = type_init(TYPE_PTR);
    type->type_ptr.base_type = type_ast_type_to_type(node->type_ptr.base_type);
    return type;
  case AST_TYPE_ARRAY:
    type = type_init(TYPE_ARRAY);
    type->type_array.base_type = type_ast_type_to_type(node->type_array.base_type);
    return type;
  case AST_TYPE_REF:
    type = type_init(TYPE_REF);
    type->type_ref.base_type = type_ast_type_to_type(node->type_ref.base_type);
    return type;
  case AST_TYPE_NULLABLE:
    type = type_init(TYPE_NULLABLE);
    type->type_nullable.base_type = type_ast_type_to_type(node->type_nullable.base_type);
    return type;
  case AST_TYPE_FUN:
    type = type_init(TYPE_FUN);
    type->type_fun.param_types = NULL;

    if (node->type_fun.params != NULL)
    {
      type->type_fun.param_types = list_init();
      
      for (list_elem_t* it = list_front_elem(node->type_fun.params); it != NULL; it = list_elem_next(it))
        list_push_back(type->type_fun.param_types, type_ast_type_to_type((ast_node_t*)list_elem_get(it)));
    }

    type->type_fun.ret_type = type_ast_type_to_type(node->type_fun.ret_type);
    return type;
  case AST_TYPE_GEN:
    type = type_init(TYPE_GEN);
    type->type_gen.param_types = NULL;

    if (node->type_gen.params != NULL)
    {
      type->type_gen.param_types = list_init();

      for (list_elem_t* it = list_front_elem(node->type_gen.params); it != NULL; it = list_elem_next(it))
        list_push_back(type->type_gen.param_types, type_ast_type_to_type((ast_node_t*)list_elem_get(it)));
    }

    type->type_gen.ret_type = type_ast_type_to_type(node->type_gen.ret_type);
    return type;
  case AST_TYPE_TYPE:
    return type_builtin(TYPE_TYPE);
  case AST_TYPE_BUILTIN_I8:
    return type_builtin(TYPE_I8);
  case AST_TYPE_BUILTIN_I16:
    return type_builtin(TYPE_I16);
  case AST_TYPE_BUILTIN_I32:
    return type_builtin(TYPE_I32);
  case AST_TYPE_BUILTIN_I64:
    return type_builtin(TYPE_I64);
  case AST_TYPE_BUILTIN_ISIZE:
    return type_builtin(TYPE_ISIZE);
  case AST_TYPE_BUILTIN_U8:
    return type_builtin(TYPE_U8);
  case AST_TYPE_BUILTIN_U16:
    return type_builtin(TYPE_U16);
  case AST_TYPE_BUILTIN_U32:
    return type_builtin(TYPE_U32);
  case AST_TYPE_BUILTIN_U64:
    return type_builtin(TYPE_U64);
  case AST_TYPE_BUILTIN_USIZE:
    return type_builtin(TYPE_USIZE);
  case AST_TYPE_BUILTIN_F32:
    return type_builtin(TYPE_F32);
  case AST_TYPE_BUILTIN_F64:
    return type_builtin(TYPE_F64);
  case AST_TYPE_BUILTIN_BOOL:
    return type_builtin(TYPE_BOOL);
  case AST_TYPE_BUILTIN_UNIT:
    return type_builtin(TYPE_UNIT);
  default:
    unreachable();
  }

  return NULL;
}

type_t* type_of(ast_node_t* node)
{
  type_t* type = NULL;

  switch (node->kind)
  {
  case AST_DECL_VAR:
    return type_ast_type_to_type(node->decl_var.type);
  case AST_PARAM:
    return type_ast_type_to_type(node->param.type);
  case AST_VARIADIC_PARAM:
    return type_ast_type_to_type(node->variadic_param.type);
  case AST_GENERIC_PARAM:
    return type_ast_type_to_type(node->generic_param.type);
  case AST_LOOP_VAR:
    return type_ast_type_to_type(node->loop_var.type);
  case AST_ENUMERATOR:
    type = type_init(TYPE_DECL);
    type->type_decl.node = NULL; /* TODO: enumerator type */
  case AST_DECL_FUN:
    type = type_init(TYPE_FUN);
    type->type_fun.param_types = NULL;

    if (node->decl_fun.params != NULL)
    {
      type->type_fun.param_types = list_init();
      
      for (list_elem_t* it = list_front_elem(node->decl_fun.params); it != NULL; it = list_elem_next(it))
      {
        ast_node_t* param = (ast_node_t*)list_elem_get(it);
        list_push_back(type->type_fun.param_types, type_ast_type_to_type(param->param.type));
      }
    }

    type->type_fun.ret_type = type_ast_type_to_type(node->decl_fun.ret_type);
    return type;
  case AST_DECL_GEN:
    type = type_init(TYPE_GEN);
    type->type_gen.param_types = NULL;

    if (node->decl_gen.params != NULL)
    {
      type->type_gen.param_types = list_init();
      
      for (list_elem_t* it = list_front_elem(node->decl_gen.params); it != NULL; it = list_elem_next(it))
      {
        ast_node_t* param = (ast_node_t*)list_elem_get(it);
        list_push_back(type->type_gen.param_types, type_ast_type_to_type(param->param.type));
      }
    }

    type->type_gen.ret_type = type_ast_type_to_type(node->decl_gen.ret_type);
    return type;
  default:
    unreachable();
  }

  return NULL;
}
