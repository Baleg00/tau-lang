#include "types.h"

#include "memtrace.h"
#include "util.h"

type_member_t* type_member_init(char* id, type_t* type)
{
  type_member_t* member = (type_member_t*)malloc(sizeof(type_member_t));
  assert(member != NULL);
  member->id = id;
  member->type = type;
  return member;
}

void type_member_free(type_member_t* member)
{
  type_free(member->type);
  free(member);
}

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
  case TYPE_STRUCT:
    if (type->type_struct.members != NULL)
    {
      list_for_each(type->type_struct.members, type_member_free);
      list_free(type->type_struct.members);
    }
    break;
  case TYPE_UNION:
    if (type->type_union.members != NULL)
    {
      list_for_each(type->type_union.members, type_member_free);
      list_free(type->type_union.members);
    }
    break;
  case TYPE_ENUM:
    if (type->type_enum.members != NULL)
    {
      list_for_each(type->type_enum.members, type_member_free);
      list_free(type->type_enum.members);
    }
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

    for (list_node_t *lit = list_front_node(lhs->type_fun.param_types), *rit = list_front_node(rhs->type_fun.param_types);
         lit != NULL && rit != NULL;
         lit = list_node_next(lit), rit = list_node_next(rit))
      if (!type_is_same((type_t*)list_node_get(lit), (type_t*)list_node_get(rit)))
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

    for (list_node_t *lit = list_front_node(lhs->type_gen.param_types), *rit = list_front_node(rhs->type_gen.param_types);
         lit != NULL && rit != NULL;
         lit = list_node_next(lit), rit = list_node_next(rit))
      if (!type_is_same((type_t*)list_node_get(lit), (type_t*)list_node_get(rit)))
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
  case TYPE_STRUCT:
    return lhs->type_struct.node == rhs->type_struct.node;
  case TYPE_UNION:
    return lhs->type_union.node == rhs->type_union.node;
  case TYPE_ENUM:
    return lhs->type_enum.node == rhs->type_enum.node;
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

bool type_is_owner(type_kind_t kind)
{
  switch (kind)
  {
  case TYPE_STRUCT:
  case TYPE_UNION:
  case TYPE_ENUM:
  case TYPE_MOD:
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

type_t* type_of(ast_node_t* node)
{
  type_t* type = NULL;

  switch (node->kind)
  {
  case AST_TYPE_MUT:
    type = type_init(TYPE_MUT);
    type->type_mut.base_type = type_of(node->type_mut.base_type);
    return type;
  case AST_TYPE_CONST:
    type = type_init(TYPE_CONST);
    type->type_const.base_type = type_of(node->type_const.base_type);
    return type;
  case AST_TYPE_PTR:
    type = type_init(TYPE_PTR);
    type->type_ptr.base_type = type_of(node->type_ptr.base_type);
    return type;
  case AST_TYPE_ARRAY:
    type = type_init(TYPE_ARRAY);
    type->type_array.base_type = type_of(node->type_array.base_type);
    return type;
  case AST_TYPE_REF:
    type = type_init(TYPE_REF);
    type->type_ref.base_type = type_of(node->type_ref.base_type);
    return type;
  case AST_TYPE_OPTIONAL:
    type = type_init(TYPE_NULLABLE);
    type->type_nullable.base_type = type_of(node->type_nullable.base_type);
    return type;
  case AST_TYPE_FUN:
    type = type_init(TYPE_FUN);
    type->type_fun.param_types = NULL;

    if (node->type_fun.params != NULL)
    {
      type->type_fun.param_types = list_init();
      
      for (list_node_t* it = list_front_node(node->type_fun.params); it != NULL; it = list_node_next(it))
        list_push_back(type->type_fun.param_types, type_of((ast_node_t*)list_node_get(it)));
    }

    type->type_fun.ret_type = type_of(node->type_fun.ret_type);
    return type;
  case AST_TYPE_GEN:
    type = type_init(TYPE_GEN);
    type->type_gen.param_types = NULL;

    if (node->type_gen.params != NULL)
    {
      type->type_gen.param_types = list_init();

      for (list_node_t* it = list_front_node(node->type_gen.params); it != NULL; it = list_node_next(it))
        list_push_back(type->type_gen.param_types, type_of((ast_node_t*)list_node_get(it)));
    }

    type->type_gen.ret_type = type_of(node->type_gen.ret_type);
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
  case AST_DECL_VAR:
    return type_of(node->decl_var.type);
  case AST_DECL_FUN:
    type = type_init(TYPE_FUN);
    type->type_fun.param_types = NULL;

    if (node->decl_fun.params != NULL)
    {
      type->type_fun.param_types = list_init();
      
      for (list_node_t* it = list_front_node(node->decl_fun.params); it != NULL; it = list_node_next(it))
      {
        ast_node_t* param = (ast_node_t*)list_node_get(it);
        list_push_back(type->type_fun.param_types, type_of(param->param.type));
      }
    }

    type->type_fun.ret_type = type_of(node->decl_fun.ret_type);
    return type;
  case AST_DECL_GEN:
    type = type_init(TYPE_GEN);
    type->type_gen.param_types = NULL;

    if (node->decl_gen.params != NULL)
    {
      type->type_gen.param_types = list_init();
      
      for (list_node_t* it = list_front_node(node->decl_gen.params); it != NULL; it = list_node_next(it))
      {
        ast_node_t* param = (ast_node_t*)list_node_get(it);
        list_push_back(type->type_gen.param_types, type_of(param->param.type));
      }
    }

    type->type_gen.ret_type = type_of(node->decl_gen.ret_type);
    return type;
  case AST_DECL_STRUCT:
    type = type_init(TYPE_STRUCT);
    type->type_struct.node = node;
    type->type_struct.id = ((token_id_t*)node->decl_struct.id->tok)->value;
    type->type_struct.members = NULL;
    
    if (node->decl_struct.members != NULL)
    {
      type->type_struct.members = list_init();

      LIST_FOR_LOOP(item, node->decl_struct.members)
      {
        ast_node_t* member_node = (ast_node_t*)list_node_get(item);
        token_id_t* tok_id = (token_id_t*)member_node->decl_member.decl->decl_var.id->tok;
        type_member_t* member_type = type_member_init(tok_id->value, type_of(member_node->decl_member.decl));
        list_push_back(type->type_struct.members, member_type);
      }
    }

    return type;
  case AST_DECL_UNION:
    type = type_init(TYPE_UNION);
    type->type_union.node = node;
    type->type_union.id = ((token_id_t*)node->decl_union.id->tok)->value;
    type->type_union.members = NULL;

    if (node->decl_union.members != NULL)
    {
      type->type_union.members = list_init();

      LIST_FOR_LOOP(item, node->decl_union.members)
      {
        ast_node_t* member_node = (ast_node_t*)list_node_get(item);
        token_id_t* tok_id = (token_id_t*)member_node->decl_member.decl->decl_var.id->tok;
        type_member_t* member_type = type_member_init(tok_id->value, type_of(member_node->decl_member.decl));
        list_push_back(type->type_union.members, member_type);
      }
    }

    return type;
  case AST_DECL_ENUM:
    type = type_init(TYPE_ENUM);
    type->type_enum.node = node;
    type->type_enum.id = ((token_id_t*)node->decl_enum.id->tok)->value;
    type->type_enum.members = NULL;
    
    if (node->decl_enum.members != NULL)
    {
      type->type_enum.members = list_init();

      LIST_FOR_LOOP(item, node->decl_enum.members)
      {
        ast_node_t* member_node = (ast_node_t*)list_node_get(item);
        token_id_t* tok_id = (token_id_t*)member_node->decl_member.decl->enumerator.id->tok;
        type_member_t* member_type = type_member_init(tok_id->value, type);
        list_push_back(type->type_enum.members, member_type);
      }
    }

    return type;
  case AST_DECL_MOD:
    type = type_init(TYPE_MOD);
    type->type_mod.node = node;
    type->type_mod.id = ((token_id_t*)node->decl_mod.id->tok)->value;
    type->type_mod.members = NULL;

    if (node->decl_mod.members != NULL)
    {
      type->type_mod.members = list_init();

      LIST_FOR_LOOP(item, node->decl_mod.members)
      {
        ast_node_t* member_node = (ast_node_t*)list_node_get(item);
        ast_node_t* decl = member_node->decl_member.decl;
        token_id_t* tok_id = NULL;

        switch (decl->kind)
        {
        case AST_DECL_VAR:    tok_id = (token_id_t*)decl->decl_var.id->tok; break;
        case AST_DECL_FUN:    tok_id = (token_id_t*)decl->decl_fun.id->tok; break;
        case AST_DECL_GEN:    tok_id = (token_id_t*)decl->decl_gen.id->tok; break;
        case AST_DECL_STRUCT: tok_id = (token_id_t*)decl->decl_struct.id->tok; break;
        case AST_DECL_UNION:  tok_id = (token_id_t*)decl->decl_union.id->tok; break;
        case AST_DECL_ENUM:   tok_id = (token_id_t*)decl->decl_enum.id->tok; break;
        case AST_DECL_MOD:    tok_id = (token_id_t*)decl->decl_mod.id->tok; break;
        default:
          unreachable();
        }

        type_member_t* member_type = type_member_init(tok_id->value, type_of(decl));
        list_push_back(type->type_mod.members, member_type);
      }
    }

    return type;
  case AST_PARAM:
    return type_of(node->param.type);
  case AST_VARIADIC_PARAM:
    return type_of(node->variadic_param.type);
  case AST_GENERIC_PARAM:
    return type_of(node->generic_param.type);
  case AST_LOOP_VAR:
    return type_of(node->loop_var.type);
  default:
    unreachable();
  }

  return NULL;
}

void type_print(FILE* stream, type_t* type)
{
  switch (type->kind)
  {
  case TYPE_MUT:
    fprintf(stream, "mut ");
    type_print(stream, type->type_mut.base_type);
    break;
  case TYPE_CONST:
    fprintf(stream, "const ");
    type_print(stream, type->type_const.base_type);
    break;
  case TYPE_PTR:
    fputc('*', stream);
    type_print(stream, type->type_ptr.base_type);
    break;
  case TYPE_ARRAY:
    fprintf(stream, "[]");
    type_print(stream, type->type_array.base_type);
    break;
  case TYPE_REF:
    fputc('&', stream);
    type_print(stream, type->type_ref.base_type);
    break;
  case TYPE_NULLABLE:
    fputc('?', stream);
    type_print(stream, type->type_nullable.base_type);
    break;
  case TYPE_FUN:
    fprintf(stream, "fun(");
    if (type->type_fun.param_types != NULL)
      LIST_FOR_LOOP(elem, type->type_fun.param_types)
      {
        type_print(stream, (type_t*)list_node_get(elem));

        if (list_node_next(elem) != NULL)
          fprintf(stream, ", ");
      }
    fprintf(stream, "): ");
    type_print(stream, type->type_fun.ret_type);
    break;
  case TYPE_GEN:
    fprintf(stream, "gen(");
    if (type->type_gen.param_types != NULL)
      LIST_FOR_LOOP(elem, type->type_gen.param_types)
      {
        type_print(stream, (type_t*)list_node_get(elem));

        if (list_node_next(elem) != NULL)
          fprintf(stream, ", ");
      }
    fprintf(stream, "): ");
    type_print(stream, type->type_gen.ret_type);
    break;
  case TYPE_I8:
    fprintf(stream, "i8");
    break;
  case TYPE_I16:
    fprintf(stream, "i16");
    break;
  case TYPE_I32:
    fprintf(stream, "i32");
    break;
  case TYPE_I64:
    fprintf(stream, "i64");
    break;
  case TYPE_ISIZE:
    fprintf(stream, "isize");
    break;
  case TYPE_U8:
    fprintf(stream, "u8");
    break;
  case TYPE_U16:
    fprintf(stream, "u16");
    break;
  case TYPE_U32:
    fprintf(stream, "u32");
    break;
  case TYPE_U64:
    fprintf(stream, "u64");
    break;
  case TYPE_USIZE:
    fprintf(stream, "usize");
    break;
  case TYPE_F32:
    fprintf(stream, "f32");
    break;
  case TYPE_F64:
    fprintf(stream, "f64");
    break;
  case TYPE_BOOL:
    fprintf(stream, "bool");
    break;
  case TYPE_UNIT:
    fprintf(stream, "unit");
    break;
  case TYPE_STRUCT:
    fprintf(stream, type->type_struct.id);
    break;
  case TYPE_UNION:
    fprintf(stream, type->type_union.id);
    break;
  case TYPE_ENUM:
    fprintf(stream, type->type_enum.id);
    break;
  default:
    unreachable();
  }
}
