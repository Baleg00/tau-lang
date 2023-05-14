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
  typedesc_t* type = (typedesc_t*)malloc(size);
  assert(type != NULL);
  type->kind = kind;
  return type;
}

typedesc_t* typedesc_builtin(typedesc_kind_t kind)
{
  static typedesc_t typedesc_type  = { .kind = TYPEDESC_TYPE  };
  static typedesc_t typedesc_i8    = { .kind = TYPEDESC_I8    };
  static typedesc_t typedesc_i16   = { .kind = TYPEDESC_I16   };
  static typedesc_t typedesc_i32   = { .kind = TYPEDESC_I32   };
  static typedesc_t typedesc_i64   = { .kind = TYPEDESC_I64   };
  static typedesc_t typedesc_isize = { .kind = TYPEDESC_ISIZE };
  static typedesc_t typedesc_u8    = { .kind = TYPEDESC_U8    };
  static typedesc_t typedesc_u16   = { .kind = TYPEDESC_U16   };
  static typedesc_t typedesc_u32   = { .kind = TYPEDESC_U32   };
  static typedesc_t typedesc_u64   = { .kind = TYPEDESC_U64   };
  static typedesc_t typedesc_usize = { .kind = TYPEDESC_USIZE };
  static typedesc_t typedesc_f32   = { .kind = TYPEDESC_F32   };
  static typedesc_t typedesc_f64   = { .kind = TYPEDESC_F64   };
  static typedesc_t typedesc_bool  = { .kind = TYPEDESC_BOOL  };
  static typedesc_t typedesc_unit  = { .kind = TYPEDESC_UNIT  };
  static typedesc_t typedesc_null  = { .kind = TYPEDESC_NULL  };

  switch (kind)
  {
  case TYPEDESC_TYPE:  return &typedesc_type;
  case TYPEDESC_I8:    return &typedesc_i8;
  case TYPEDESC_I16:   return &typedesc_i16;
  case TYPEDESC_I32:   return &typedesc_i32;
  case TYPEDESC_I64:   return &typedesc_i64;
  case TYPEDESC_ISIZE: return &typedesc_isize;
  case TYPEDESC_U8:    return &typedesc_u8;
  case TYPEDESC_U16:   return &typedesc_u16;
  case TYPEDESC_U32:   return &typedesc_u32;
  case TYPEDESC_U64:   return &typedesc_u64;
  case TYPEDESC_USIZE: return &typedesc_usize;
  case TYPEDESC_F32:   return &typedesc_f32;
  case TYPEDESC_F64:   return &typedesc_f64;
  case TYPEDESC_BOOL:  return &typedesc_bool;
  case TYPEDESC_UNIT:  return &typedesc_unit;
  case TYPEDESC_NULL:  return &typedesc_null;
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
  case TYPEDESC_UNION:
  case TYPEDESC_ENUM:
  case TYPEDESC_MOD:
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

size_t typedesc_size(typedesc_t* type)
{
  switch (type->kind)
  {
  case TYPEDESC_I8:    return 1;
  case TYPEDESC_I16:   return 2;
  case TYPEDESC_I32:   return 4;
  case TYPEDESC_I64:   return 8;
  case TYPEDESC_ISIZE: return 8;
  case TYPEDESC_U8:    return 1;
  case TYPEDESC_U16:   return 2;
  case TYPEDESC_U32:   return 4;
  case TYPEDESC_U64:   return 8;
  case TYPEDESC_USIZE: return 8;
  case TYPEDESC_F32:   return 4;
  case TYPEDESC_F64:   return 8;
  default: unreachable();
  }

  return 0;
}

typedesc_t* typedesc_promote(typedesc_t* lhs, typedesc_t* rhs)
{
  assert(typedesc_is_arithmetic(lhs) && typedesc_is_arithmetic(rhs));

  if (typedesc_is_integer(lhs) != typedesc_is_integer(rhs))
    return typedesc_is_float(lhs) ? lhs : rhs;
  
  if (typedesc_is_integer(lhs))
    if (typedesc_is_signed(lhs) != typedesc_is_signed(rhs))
      return typedesc_promote(typedesc_make_signed(lhs), typedesc_make_signed(rhs));

  return typedesc_size(lhs) > typedesc_size(rhs) ? lhs : rhs;
}

typedesc_t* typedesc_of_mut(ast_type_mut_t* node)
{
  typedesc_mut_t* type = typedesc_mut_init();
  type->base_type = typedesc_of(node->base_type);
  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_const(ast_type_const_t* node)
{
  typedesc_const_t* type = typedesc_const_init();
  type->base_type = typedesc_of(node->base_type);
  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_ptr(ast_type_ptr_t* node)
{
  typedesc_ptr_t* type = typedesc_ptr_init();
  type->base_type = typedesc_of(node->base_type);
  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_array(ast_type_array_t* node)
{
  typedesc_array_t* type = typedesc_array_init();
  type->base_type = typedesc_of(node->base_type);
  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_ref(ast_type_ref_t* node)
{
  typedesc_ref_t* type = typedesc_ref_init();
  type->base_type = typedesc_of(node->base_type);
  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_opt(ast_type_opt_t* node)
{
  typedesc_opt_t* type = typedesc_opt_init();
  type->base_type = typedesc_of(node->base_type);
  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_fun(ast_type_fun_t* node)
{
  typedesc_fun_t* type = typedesc_fun_init();
  type->param_types = NULL;

  if (node->params != NULL)
  {
    type->param_types = list_init();
    
    LIST_FOR_LOOP(it, node->params)
      list_push_back(type->param_types, typedesc_of((ast_node_t*)list_node_get(it)));
  }

  type->return_type = typedesc_of(node->return_type);

  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_gen(ast_type_gen_t* node)
{
  typedesc_gen_t* type = typedesc_gen_init();
  type->param_types = NULL;

  if (node->params != NULL)
  {
    type->param_types = list_init();
    
    LIST_FOR_LOOP(it, node->params)
      list_push_back(type->param_types, typedesc_of((ast_node_t*)list_node_get(it)));
  }

  type->yield_type = typedesc_of(node->yield_type);

  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_decl_fun(ast_decl_fun_t* node)
{
  typedesc_fun_t* type = typedesc_fun_init();
  type->param_types = NULL;

  if (node->params != NULL)
  {
    type->param_types = list_init();
    
    LIST_FOR_LOOP(it, node->params)
      list_push_back(type->param_types, typedesc_of((ast_node_t*)list_node_get(it)));
  }

  type->return_type = typedesc_of(node->return_type);

  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_decl_gen(ast_decl_gen_t* node)
{
  typedesc_gen_t* type = typedesc_gen_init();
  type->param_types = NULL;

  if (node->params != NULL)
  {
    type->param_types = list_init();
    
    LIST_FOR_LOOP(it, node->params)
      list_push_back(type->param_types, typedesc_of((ast_node_t*)list_node_get(it)));
  }

  type->yield_type = typedesc_of(node->yield_type);

  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_decl_struct(ast_decl_struct_t* node)
{
  typedesc_struct_t* type = typedesc_struct_init();
  type->node = (ast_decl_t*)node;
  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_decl_union(ast_decl_union_t* node)
{
  typedesc_union_t* type = typedesc_union_init();
  type->node = (ast_decl_t*)node;
  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_decl_enum(ast_decl_enum_t* node)
{
  typedesc_enum_t* type = typedesc_enum_init();
  type->node = (ast_decl_t*)node;
  return (typedesc_t*)type;
}

typedesc_t* typedesc_of_decl_mod(ast_decl_mod_t* node)
{
  typedesc_mod_t* type = typedesc_mod_init();
  type->node = (ast_decl_t*)node;
  return (typedesc_t*)type;
}

typedesc_t* typedesc_of(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_TYPE_MUT:       return typedesc_of_mut((ast_type_mut_t*)node);
  case AST_TYPE_CONST:     return typedesc_of_const((ast_type_const_t*)node);
  case AST_TYPE_PTR:       return typedesc_of_ptr((ast_type_ptr_t*)node);
  case AST_TYPE_ARRAY:     return typedesc_of_array((ast_type_array_t*)node);
  case AST_TYPE_REF:       return typedesc_of_ref((ast_type_ref_t*)node);
  case AST_TYPE_OPT:       return typedesc_of_opt((ast_type_opt_t*)node);
  case AST_TYPE_FUN:       return typedesc_of_fun((ast_type_fun_t*)node);
  case AST_TYPE_GEN:       return typedesc_of_gen((ast_type_gen_t*)node);
  case AST_TYPE_TYPE:      return typedesc_builtin(TYPEDESC_TYPE);
  case AST_TYPE_I8:        return typedesc_builtin(TYPEDESC_I8);
  case AST_TYPE_I16:       return typedesc_builtin(TYPEDESC_I16);
  case AST_TYPE_I32:       return typedesc_builtin(TYPEDESC_I32);
  case AST_TYPE_I64:       return typedesc_builtin(TYPEDESC_I64);
  case AST_TYPE_ISIZE:     return typedesc_builtin(TYPEDESC_ISIZE);
  case AST_TYPE_U8:        return typedesc_builtin(TYPEDESC_U8);
  case AST_TYPE_U16:       return typedesc_builtin(TYPEDESC_U16);
  case AST_TYPE_U32:       return typedesc_builtin(TYPEDESC_U32);
  case AST_TYPE_U64:       return typedesc_builtin(TYPEDESC_U64);
  case AST_TYPE_USIZE:     return typedesc_builtin(TYPEDESC_USIZE);
  case AST_TYPE_F32:       return typedesc_builtin(TYPEDESC_F32);
  case AST_TYPE_F64:       return typedesc_builtin(TYPEDESC_F64);
  case AST_TYPE_BOOL:      return typedesc_builtin(TYPEDESC_BOOL);
  case AST_TYPE_UNIT:      return typedesc_builtin(TYPEDESC_UNIT);
  case AST_DECL_VAR:       return typedesc_of(((ast_decl_var_t*)node)->type);
  case AST_DECL_LOOP_VAR:  return typedesc_of(((ast_decl_loop_var_t*)node)->type);
  case AST_DECL_FUN:       return typedesc_of_decl_fun((ast_decl_fun_t*)node);
  case AST_DECL_GEN:       return typedesc_of_decl_gen((ast_decl_gen_t*)node);
  case AST_DECL_STRUCT:    return typedesc_of_decl_struct((ast_decl_struct_t*)node);
  case AST_DECL_UNION:     return typedesc_of_decl_union((ast_decl_union_t*)node);
  case AST_DECL_ENUM:      return typedesc_of_decl_enum((ast_decl_enum_t*)node);
  case AST_DECL_MOD:       return typedesc_of_decl_mod((ast_decl_mod_t*)node);
  case AST_PARAM:          return typedesc_of(((ast_param_t*)node)->type);
  case AST_PARAM_VARIADIC: return typedesc_of(((ast_param_variadic_t*)node)->type);
  case AST_PARAM_GENERIC:  return typedesc_of(((ast_param_generic_t*)node)->type);
  default: unreachable();
  }

  return NULL;
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
