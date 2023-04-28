#include "types.h"

#include "util.h"
#include "memtrace.h"

type_t* type_init(type_kind_t kind, size_t size)
{
  type_t* type = (type_t*)malloc(size);
  assert(type != NULL);
  type->kind = kind;
  return type;
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
  default: unreachable();
  }

  return NULL;
}

LIST_FOR_EACH_FUNC_DECL(type_free, type_t);

void type_free(type_t* type)
{
  if (type == NULL)
    return;

  switch (type->kind)
  {
  case TYPE_MUT:
    type_free(((type_mut_t*)type)->base_type);
    break;
  case TYPE_CONST:
    type_free(((type_const_t*)type)->base_type);
    break;
  case TYPE_PTR:
    type_free(((type_ptr_t*)type)->base_type);
    break;
  case TYPE_ARRAY:
    type_free(((type_array_t*)type)->base_type);
    break;
  case TYPE_REF:
    type_free(((type_ref_t*)type)->base_type);
    break;
  case TYPE_OPT:
    type_free(((type_opt_t*)type)->base_type);
    break;
  case TYPE_FUN:
    if (((type_fun_t*)type)->param_types != NULL)
    {
      list_for_each(((type_fun_t*)type)->param_types, LIST_FOR_EACH_FUNC_NAME(type_free));
      list_free(((type_fun_t*)type)->param_types);
    }

    type_free(((type_fun_t*)type)->return_type);
    break;
  case TYPE_GEN:
    if (((type_gen_t*)type)->param_types != NULL)
    {
      list_for_each(((type_gen_t*)type)->param_types, LIST_FOR_EACH_FUNC_NAME(type_free));
      list_free(((type_gen_t*)type)->param_types);
    }

    type_free(((type_gen_t*)type)->yield_type);
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
  case TYPE_NULL:
    return;
  case TYPE_STRUCT:
  case TYPE_UNION:
  case TYPE_ENUM:
  case TYPE_MOD:
    break;
  default:
    unreachable();
  }

  free(type);
}

bool type_is_same_fun(type_fun_t* lhs, type_fun_t* rhs)
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
    if (!type_is_same((type_t*)list_node_get(lit), (type_t*)list_node_get(rit)))
      return false;

  return type_is_same(lhs->return_type, rhs->return_type);
}

bool type_is_same_gen(type_gen_t* lhs, type_gen_t* rhs)
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
    if (!type_is_same((type_t*)list_node_get(lit), (type_t*)list_node_get(rit)))
      return false;

  return type_is_same(lhs->yield_type, rhs->yield_type);
}

bool type_is_modifier(type_t* type)
{
  return type->kind & TYPE_FLAG_MODIFIER;
}

bool type_is_builtin(type_t* type)
{
  return type->kind & TYPE_FLAG_BUILTIN;
}

bool type_is_integer(type_t* type)
{
  return type->kind & TYPE_FLAG_INTEGER;
}

bool type_is_float(type_t* type)
{
  return type->kind & TYPE_FLAG_FLOAT;
}

bool type_is_arithmetic(type_t* type)
{
  return type->kind & TYPE_FLAG_ARITHMETIC;
}

bool type_is_signed(type_t* type)
{
  return type->kind & TYPE_FLAG_SIGNED;
}

bool type_is_unsigned(type_t* type)
{
  return type->kind & TYPE_FLAG_UNSIGNED;
}

bool type_is_owner(type_t* type)
{
  return type->kind & TYPE_FLAG_OWNER;
}

bool type_is_same(type_t* lhs, type_t* rhs)
{
  if (lhs->kind != rhs->kind)
    return false;

  switch (lhs->kind)
  {
  case TYPE_MUT:    return type_is_same(((type_mut_t*)lhs)->base_type, ((type_mut_t*)rhs)->base_type);
  case TYPE_CONST:  return type_is_same(((type_const_t*)lhs)->base_type, ((type_const_t*)rhs)->base_type);
  case TYPE_PTR:    return type_is_same(((type_ptr_t*)lhs)->base_type, ((type_ptr_t*)rhs)->base_type);
  case TYPE_ARRAY:  return type_is_same(((type_array_t*)lhs)->base_type, ((type_array_t*)rhs)->base_type);
  case TYPE_REF:    return type_is_same(((type_ref_t*)lhs)->base_type, ((type_ref_t*)rhs)->base_type);
  case TYPE_OPT:    return type_is_same(((type_opt_t*)lhs)->base_type, ((type_opt_t*)rhs)->base_type);
  case TYPE_FUN:    return type_is_same_fun((type_fun_t*)lhs, (type_fun_t*)rhs);
  case TYPE_GEN:    return type_is_same_gen((type_gen_t*)lhs, (type_gen_t*)rhs);
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
  case TYPE_UNIT:   return true;
  case TYPE_STRUCT:
  case TYPE_UNION:
  case TYPE_ENUM:
  case TYPE_MOD:    return ((type_decl_t*)lhs)->node == ((type_decl_t*)rhs)->node;
  default: unreachable();
  }

  return false;
}

bool type_is_assignable(type_t* to, type_t* from)
{
  if (to->kind == TYPE_REF && from->kind != TYPE_REF)
    return false;

  return type_is_same(type_remove_ref(to), type_remove_ref(from));
}

type_t* type_make_signed(type_t* type)
{
  assert(type_is_integer(type));

  switch (type->kind)
  {
  case TYPE_U8: return type_builtin(TYPE_I8);
  case TYPE_U16: return type_builtin(TYPE_I16);
  case TYPE_U32: return type_builtin(TYPE_I32);
  case TYPE_U64: return type_builtin(TYPE_I64);
  case TYPE_USIZE: return type_builtin(TYPE_ISIZE);
  }

  return type;
}

type_t* type_make_ptr(type_t* type)
{
  type_ptr_t* ptr_type = type_ptr_init();
  ptr_type->base_type = type;
  return (type_t*)ptr_type;
}

type_t* type_make_ref(type_t* type)
{
  type_ref_t* ref_type = type_ref_init();
  ref_type->base_type = type;
  return (type_t*)ref_type;
}

type_t* type_remove_ref(type_t* type)
{
  return type->kind == TYPE_REF ? ((type_ref_t*)type)->base_type : type;
}

size_t type_size(type_t* type)
{
  switch (type->kind)
  {
  case TYPE_I8:    return 1;
  case TYPE_I16:   return 2;
  case TYPE_I32:   return 4;
  case TYPE_I64:   return 8;
  case TYPE_ISIZE: return 8;
  case TYPE_U8:    return 1;
  case TYPE_U16:   return 2;
  case TYPE_U32:   return 4;
  case TYPE_U64:   return 8;
  case TYPE_USIZE: return 8;
  case TYPE_F32:   return 4;
  case TYPE_F64:   return 8;
  default: unreachable();
  }

  return 0;
}

type_t* type_promote(type_t* lhs, type_t* rhs)
{
  assert(type_is_arithmetic(lhs) && type_is_arithmetic(rhs));

  if (type_is_integer(lhs) != type_is_integer(rhs))
    return type_is_float(lhs) ? lhs : rhs;
  
  if (type_is_integer(lhs))
    if (type_is_signed(lhs) != type_is_signed(rhs))
      return type_promote(type_make_signed(lhs), type_make_signed(rhs));

  return type_size(lhs) > type_size(rhs) ? lhs : rhs;
}

type_t* type_of_type_mut(ast_type_mut_t* node)
{
  type_mut_t* type = type_mut_init();
  type->base_type = type_of(node->base_type);
  return (type_t*)type;
}

type_t* type_of_type_const(ast_type_const_t* node)
{
  type_const_t* type = type_const_init();
  type->base_type = type_of(node->base_type);
  return (type_t*)type;
}

type_t* type_of_type_ptr(ast_type_ptr_t* node)
{
  type_ptr_t* type = type_ptr_init();
  type->base_type = type_of(node->base_type);
  return (type_t*)type;
}

type_t* type_of_type_array(ast_type_array_t* node)
{
  type_array_t* type = type_array_init();
  type->base_type = type_of(node->base_type);
  return (type_t*)type;
}

type_t* type_of_type_ref(ast_type_ref_t* node)
{
  type_ref_t* type = type_ref_init();
  type->base_type = type_of(node->base_type);
  return (type_t*)type;
}

type_t* type_of_type_opt(ast_type_opt_t* node)
{
  type_opt_t* type = type_opt_init();
  type->base_type = type_of(node->base_type);
  return (type_t*)type;
}

type_t* type_of_type_fun(ast_type_fun_t* node)
{
  type_fun_t* type = type_fun_init();
  type->param_types = NULL;

  if (node->params != NULL)
  {
    type->param_types = list_init();
    
    LIST_FOR_LOOP(it, node->params)
      list_push_back(type->param_types, type_of((ast_node_t*)list_node_get(it)));
  }

  type->return_type = type_of(node->return_type);

  return (type_t*)type;
}

type_t* type_of_type_gen(ast_type_gen_t* node)
{
  type_gen_t* type = type_gen_init();
  type->param_types = NULL;

  if (node->params != NULL)
  {
    type->param_types = list_init();
    
    LIST_FOR_LOOP(it, node->params)
      list_push_back(type->param_types, type_of((ast_node_t*)list_node_get(it)));
  }

  type->yield_type = type_of(node->yield_type);

  return (type_t*)type;
}

type_t* type_of_decl_fun(ast_decl_fun_t* node)
{
  type_fun_t* type = type_fun_init();
  type->param_types = NULL;

  if (node->params != NULL)
  {
    type->param_types = list_init();
    
    LIST_FOR_LOOP(it, node->params)
      list_push_back(type->param_types, type_of((ast_node_t*)list_node_get(it)));
  }

  type->return_type = type_of(node->return_type);

  return (type_t*)type;
}

type_t* type_of_decl_gen(ast_decl_gen_t* node)
{
  type_gen_t* type = type_gen_init();
  type->param_types = NULL;

  if (node->params != NULL)
  {
    type->param_types = list_init();
    
    LIST_FOR_LOOP(it, node->params)
      list_push_back(type->param_types, type_of((ast_node_t*)list_node_get(it)));
  }

  type->yield_type = type_of(node->yield_type);

  return (type_t*)type;
}

type_t* type_of_decl_struct(ast_decl_struct_t* node)
{
  type_struct_t* type = type_struct_init();
  type->node = (ast_decl_t*)node;
  return (type_t*)type;
}

type_t* type_of_decl_union(ast_decl_union_t* node)
{
  type_union_t* type = type_union_init();
  type->node = (ast_decl_t*)node;
  return (type_t*)type;
}

type_t* type_of_decl_enum(ast_decl_enum_t* node)
{
  type_enum_t* type = type_enum_init();
  type->node = (ast_decl_t*)node;
  return (type_t*)type;
}

type_t* type_of_decl_mod(ast_decl_mod_t* node)
{
  type_mod_t* type = type_mod_init();
  type->node = (ast_decl_t*)node;
  return (type_t*)type;
}

type_t* type_of(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_TYPE_MUT:       return type_of_type_mut((ast_type_mut_t*)node);
  case AST_TYPE_CONST:     return type_of_type_const((ast_type_const_t*)node);
  case AST_TYPE_PTR:       return type_of_type_ptr((ast_type_ptr_t*)node);
  case AST_TYPE_ARRAY:     return type_of_type_array((ast_type_array_t*)node);
  case AST_TYPE_REF:       return type_of_type_ref((ast_type_ref_t*)node);
  case AST_TYPE_OPT:       return type_of_type_opt((ast_type_opt_t*)node);
  case AST_TYPE_FUN:       return type_of_type_fun((ast_type_fun_t*)node);
  case AST_TYPE_GEN:       return type_of_type_gen((ast_type_gen_t*)node);
  case AST_TYPE_TYPE:      return type_builtin(TYPE_TYPE);
  case AST_TYPE_I8:        return type_builtin(TYPE_I8);
  case AST_TYPE_I16:       return type_builtin(TYPE_I16);
  case AST_TYPE_I32:       return type_builtin(TYPE_I32);
  case AST_TYPE_I64:       return type_builtin(TYPE_I64);
  case AST_TYPE_ISIZE:     return type_builtin(TYPE_ISIZE);
  case AST_TYPE_U8:        return type_builtin(TYPE_U8);
  case AST_TYPE_U16:       return type_builtin(TYPE_U16);
  case AST_TYPE_U32:       return type_builtin(TYPE_U32);
  case AST_TYPE_U64:       return type_builtin(TYPE_U64);
  case AST_TYPE_USIZE:     return type_builtin(TYPE_USIZE);
  case AST_TYPE_F32:       return type_builtin(TYPE_F32);
  case AST_TYPE_F64:       return type_builtin(TYPE_F64);
  case AST_TYPE_BOOL:      return type_builtin(TYPE_BOOL);
  case AST_TYPE_UNIT:      return type_builtin(TYPE_UNIT);
  case AST_DECL_VAR:       return type_of(((ast_decl_var_t*)node)->type);
  case AST_DECL_LOOP_VAR:  return type_of(((ast_decl_loop_var_t*)node)->type);
  case AST_DECL_FUN:       return type_of_decl_fun((ast_decl_fun_t*)node);
  case AST_DECL_GEN:       return type_of_decl_gen((ast_decl_gen_t*)node);
  case AST_DECL_STRUCT:    return type_of_decl_struct((ast_decl_struct_t*)node);
  case AST_DECL_UNION:     return type_of_decl_union((ast_decl_union_t*)node);
  case AST_DECL_ENUM:      return type_of_decl_enum((ast_decl_enum_t*)node);
  case AST_DECL_MOD:       return type_of_decl_mod((ast_decl_mod_t*)node);
  case AST_PARAM:          return type_of(((ast_param_t*)node)->type);
  case AST_PARAM_VARIADIC: return type_of(((ast_param_variadic_t*)node)->type);
  case AST_PARAM_GENERIC:  return type_of(((ast_param_generic_t*)node)->type);
  default: unreachable();
  }

  return NULL;
}

size_t type_print(FILE* stream, type_t* type)
{
  size_t count = 0;

  switch (type->kind)
  {
  case TYPE_MUT:
    count += (size_t)fprintf(stream, "mut ");
    count += type_print(stream, ((type_mut_t*)type)->base_type);
    break;
  case TYPE_CONST:
    count += (size_t)fprintf(stream, "const ");
    count += type_print(stream, ((type_const_t*)type)->base_type);
    break;
  case TYPE_PTR:
    count++; fputc('*', stream);
    count += type_print(stream, ((type_ptr_t*)type)->base_type);
    break;
  case TYPE_ARRAY:
    count += (size_t)fprintf(stream, "[]");
    count += type_print(stream, ((type_array_t*)type)->base_type);
    break;
  case TYPE_REF:
    count++; fputc('&', stream);
    count += type_print(stream, ((type_ref_t*)type)->base_type);
    break;
  case TYPE_OPT:
    count++; fputc('?', stream);
    count += type_print(stream, ((type_opt_t*)type)->base_type);
    break;
  case TYPE_FUN:
    count += (size_t)fprintf(stream, "fun(");

    if (((type_fun_t*)type)->param_types != NULL)
      LIST_FOR_LOOP(it, ((type_fun_t*)type)->param_types)
      {
        count += type_print(stream, (type_t*)list_node_get(it));

        if (list_node_next(it) != NULL)
          count += (size_t)fprintf(stream, ", ");
      }

    count += (size_t)fprintf(stream, "): ");
    count += type_print(stream, ((type_fun_t*)type)->return_type);
    break;
  case TYPE_GEN:
    count += (size_t)fprintf(stream, "gen(");

    if (((type_gen_t*)type)->param_types != NULL)
      LIST_FOR_LOOP(it, ((type_gen_t*)type)->param_types)
      {
        count += type_print(stream, (type_t*)list_node_get(it));

        if (list_node_next(it) != NULL)
          count += (size_t)fprintf(stream, ", ");
      }

    count += (size_t)fprintf(stream, "): ");
    count += type_print(stream, ((type_gen_t*)type)->yield_type);
    break;
  case TYPE_I8:    count += (size_t)fprintf(stream, "i8"); break;
  case TYPE_I16:   count += (size_t)fprintf(stream, "i16"); break;
  case TYPE_I32:   count += (size_t)fprintf(stream, "i32"); break;
  case TYPE_I64:   count += (size_t)fprintf(stream, "i64"); break;
  case TYPE_ISIZE: count += (size_t)fprintf(stream, "isize"); break;
  case TYPE_U8:    count += (size_t)fprintf(stream, "u8"); break;
  case TYPE_U16:   count += (size_t)fprintf(stream, "u16"); break;
  case TYPE_U32:   count += (size_t)fprintf(stream, "u32"); break;
  case TYPE_U64:   count += (size_t)fprintf(stream, "u64"); break;
  case TYPE_USIZE: count += (size_t)fprintf(stream, "usize"); break;
  case TYPE_F32:   count += (size_t)fprintf(stream, "f32"); break;
  case TYPE_F64:   count += (size_t)fprintf(stream, "f64"); break;
  case TYPE_BOOL:  count += (size_t)fprintf(stream, "bool"); break;
  case TYPE_UNIT:  count += (size_t)fprintf(stream, "unit"); break;
  case TYPE_STRUCT:
    count += (size_t)fprintf(stream, "%.*s", ast_id_len(((type_struct_t*)type)->node->id), ast_id_ptr(((type_struct_t*)type)->node->id));
    break;
  case TYPE_UNION:
    count += (size_t)fprintf(stream, "%.*s", ast_id_len(((type_union_t*)type)->node->id), ast_id_ptr(((type_union_t*)type)->node->id));
    break;
  case TYPE_ENUM:
    count += (size_t)fprintf(stream, "%.*s", ast_id_len(((type_enum_t*)type)->node->id), ast_id_ptr(((type_enum_t*)type)->node->id));
    break;
  default:
    unreachable();
  }

  return count;
}
