#include "ast.h"

#include <stdlib.h>

#include "util.h"
#include "token.h"
#include "op.h"
#include "list.h"
#include "memtrace.h"

ast_node_t* ast_node_init(ast_kind_t kind)
{
  ast_node_t* node = (ast_node_t*)calloc(1, sizeof(ast_node_t));
  assert(node != NULL);
  node->kind = kind;
  return node;
}

void ast_node_free(ast_node_t* node)
{
  if (node == NULL)
    return;

  switch (node->kind)
  {
  case AST_UNKNOWN:
    break;
  case AST_ID:
    break;
  case AST_TYPE_MUT:
    ast_node_free(node->type_mut.base_type);
    break;
  case AST_TYPE_CONST:
    ast_node_free(node->type_const.base_type);
    break;
  case AST_TYPE_PTR:
    ast_node_free(node->type_ptr.base_type);
    break;
  case AST_TYPE_ARRAY:
    ast_node_free(node->type_array.base_type);
    ast_node_free(node->type_array.size);
    break;
  case AST_TYPE_REF:
    ast_node_free(node->type_ref.base_type);
    break;
  case AST_TYPE_OPTIONAL:
    ast_node_free(node->type_nullable.base_type);
    break;
  case AST_TYPE_FUN:
    list_for_each(node->type_fun.params, ast_node_free);
    list_free(node->type_fun.params);
    ast_node_free(node->type_fun.ret_type);
    break;
  case AST_TYPE_GEN:
    list_for_each(node->type_gen.params, ast_node_free);
    list_free(node->type_gen.params);
    ast_node_free(node->type_gen.ret_type);
    break;
  case AST_TYPE_TYPE:
  case AST_TYPE_BUILTIN_I8:
  case AST_TYPE_BUILTIN_I16:
  case AST_TYPE_BUILTIN_I32:
  case AST_TYPE_BUILTIN_I64:
  case AST_TYPE_BUILTIN_ISIZE:
  case AST_TYPE_BUILTIN_U8:
  case AST_TYPE_BUILTIN_U16:
  case AST_TYPE_BUILTIN_U32:
  case AST_TYPE_BUILTIN_U64:
  case AST_TYPE_BUILTIN_USIZE:
  case AST_TYPE_BUILTIN_F32:
  case AST_TYPE_BUILTIN_F64:
  case AST_TYPE_BUILTIN_BOOL:
  case AST_TYPE_BUILTIN_UNIT:
    break;
  case AST_EXPR_LIT_INT:
  case AST_EXPR_LIT_FLT:
  case AST_EXPR_LIT_STR:
  case AST_EXPR_LIT_CHAR:
  case AST_EXPR_LIT_BOOL:
  case AST_EXPR_LIT_NULL:
    break;
  case AST_EXPR_OP:
    if (op_is_unary(node->expr_op.kind))
      ast_node_free(node->expr_op.op_unary.arg);
    else if (op_is_binary(node->expr_op.kind))
    {
      ast_node_free(node->expr_op.op_binary.lhs);
      ast_node_free(node->expr_op.op_binary.rhs);
    }
    else if (node->expr_op.kind == OP_CALL)
    {
      ast_node_free(node->expr_op.op_call.callee);
      list_for_each(node->expr_op.op_call.args, ast_node_free);
      list_free(node->expr_op.op_call.args);
    }
    break;
  case AST_STMT_IF:
    ast_node_free(node->stmt_if.cond);
    ast_node_free(node->stmt_if.stmt);
    ast_node_free(node->stmt_if.stmt_else);
    break;
  case AST_STMT_FOR:
    ast_node_free(node->stmt_for.var);
    ast_node_free(node->stmt_for.range);
    ast_node_free(node->stmt_for.stmt);
    break;
  case AST_STMT_WHILE:
    ast_node_free(node->stmt_while.cond);
    ast_node_free(node->stmt_while.stmt);
    break;
  case AST_STMT_RETURN:
    ast_node_free(node->stmt_return.expr);
    break;
  case AST_STMT_YIELD:
    ast_node_free(node->stmt_yield.expr);
    break;
  case AST_STMT_BLOCK:
    list_for_each(node->stmt_block.stmts, ast_node_free);
    list_free(node->stmt_block.stmts);
    break;
  case AST_STMT_EXPR:
    ast_node_free(node->stmt_expr.expr);
    break;
  case AST_DECL_VAR:
    ast_node_free(node->decl_var.id);
    ast_node_free(node->decl_var.type);
    ast_node_free(node->decl_var.init);
    break;
  case AST_DECL_FUN:
    if (node->decl_fun.generic_params != NULL)
    {
      list_for_each(node->decl_fun.generic_params, ast_node_free);
      list_free(node->decl_fun.generic_params);
    }
    ast_node_free(node->decl_fun.id);
    if (node->decl_fun.params != NULL)
    {
      list_for_each(node->decl_fun.params, ast_node_free);
      list_free(node->decl_fun.params);
    }
    ast_node_free(node->decl_fun.ret_type);
    ast_node_free(node->decl_fun.stmt);
    break;
  case AST_DECL_GEN:
    if (node->decl_gen.generic_params != NULL)
    {
      list_for_each(node->decl_gen.generic_params, ast_node_free);
      list_free(node->decl_gen.generic_params);
    }
    ast_node_free(node->decl_gen.id);
    if (node->decl_gen.params != NULL)
    {
      list_for_each(node->decl_gen.params, ast_node_free);
      list_free(node->decl_gen.params);
    }
    ast_node_free(node->decl_gen.ret_type);
    ast_node_free(node->decl_gen.stmt);
    break;
  case AST_DECL_STRUCT:
    if (node->decl_struct.generic_params != NULL)
    {
      list_for_each(node->decl_struct.generic_params, ast_node_free);
      list_free(node->decl_struct.generic_params);
    }
    ast_node_free(node->decl_struct.id);
    list_for_each(node->decl_struct.members, ast_node_free);
    list_free(node->decl_struct.members);
    break;
  case AST_DECL_UNION:
    ast_node_free(node->decl_union.id);
    list_for_each(node->decl_union.members, ast_node_free);
    list_free(node->decl_union.members);
    break;
  case AST_DECL_ENUM:
    ast_node_free(node->decl_enum.id);
    list_for_each(node->decl_enum.members, ast_node_free);
    list_free(node->decl_enum.members);
    break;
  case AST_DECL_MOD:
    ast_node_free(node->decl_mod.id);
    list_for_each(node->decl_mod.members, ast_node_free);
    list_free(node->decl_mod.members);
    break;
  case AST_DECL_MEMBER:
    ast_node_free(node->decl_member.decl);
    break;
  case AST_PARAM:
    ast_node_free(node->param.id);
    ast_node_free(node->param.type);
    ast_node_free(node->param.init);
    break;
  case AST_VARIADIC_PARAM:
    ast_node_free(node->variadic_param.id);
    ast_node_free(node->variadic_param.type);
    break;
  case AST_GENERIC_PARAM:
    ast_node_free(node->generic_param.id);
    ast_node_free(node->generic_param.type);
    break;
  case AST_LOOP_VAR:
    ast_node_free(node->loop_var.id);
    ast_node_free(node->loop_var.type);
    break;
  case AST_ENUMERATOR:
    ast_node_free(node->enumerator.id);
    break;
  case AST_PROG:
    list_for_each(node->prog.decls, ast_node_free);
    list_free(node->prog.decls);
  }

  free(node);
}

const char* ast_kind_to_string(ast_kind_t kind)
{
  switch (kind)
  {
  case AST_UNKNOWN:            return "AST_UNKNOWN";
  case AST_ID:                 return "AST_ID";
  case AST_TYPE_MUT:           return "AST_TYPE_MUT";
  case AST_TYPE_CONST:         return "AST_TYPE_CONST";
  case AST_TYPE_PTR:           return "AST_TYPE_PTR";
  case AST_TYPE_ARRAY:         return "AST_TYPE_ARRAY";
  case AST_TYPE_REF:           return "AST_TYPE_REF";
  case AST_TYPE_OPTIONAL:      return "AST_TYPE_OPTIONAL";
  case AST_TYPE_FUN:           return "AST_TYPE_FUN";
  case AST_TYPE_GEN:           return "AST_TYPE_GEN";
  case AST_TYPE_TYPE:          return "AST_TYPE_TYPE";
  case AST_TYPE_BUILTIN_I8:    return "AST_TYPE_BUILTIN_I8";
  case AST_TYPE_BUILTIN_I16:   return "AST_TYPE_BUILTIN_I16";
  case AST_TYPE_BUILTIN_I32:   return "AST_TYPE_BUILTIN_I32";
  case AST_TYPE_BUILTIN_I64:   return "AST_TYPE_BUILTIN_I64";
  case AST_TYPE_BUILTIN_ISIZE: return "AST_TYPE_BUILTIN_ISIZE";
  case AST_TYPE_BUILTIN_U8:    return "AST_TYPE_BUILTIN_U8";
  case AST_TYPE_BUILTIN_U16:   return "AST_TYPE_BUILTIN_U16";
  case AST_TYPE_BUILTIN_U32:   return "AST_TYPE_BUILTIN_U32";
  case AST_TYPE_BUILTIN_U64:   return "AST_TYPE_BUILTIN_U64";
  case AST_TYPE_BUILTIN_USIZE: return "AST_TYPE_BUILTIN_USIZE";
  case AST_TYPE_BUILTIN_F32:   return "AST_TYPE_BUILTIN_F32";
  case AST_TYPE_BUILTIN_F64:   return "AST_TYPE_BUILTIN_F64";
  case AST_TYPE_BUILTIN_BOOL:  return "AST_TYPE_BUILTIN_BOOL";
  case AST_TYPE_BUILTIN_UNIT:  return "AST_TYPE_BUILTIN_UNIT";
  case AST_EXPR_LIT_INT:       return "AST_EXPR_LIT_INT";
  case AST_EXPR_LIT_FLT:       return "AST_EXPR_LIT_FLT";
  case AST_EXPR_LIT_STR:       return "AST_EXPR_LIT_STR";
  case AST_EXPR_LIT_CHAR:      return "AST_EXPR_LIT_CHAR";
  case AST_EXPR_LIT_BOOL:      return "AST_EXPR_LIT_BOOL";
  case AST_EXPR_LIT_NULL:      return "AST_EXPR_LIT_NULL";
  case AST_EXPR_OP:            return "AST_EXPR_OP";
  case AST_STMT_IF:            return "AST_STMT_IF";
  case AST_STMT_FOR:           return "AST_STMT_FOR";
  case AST_STMT_WHILE:         return "AST_STMT_WHILE";
  case AST_STMT_BREAK:         return "AST_STMT_BREAK";
  case AST_STMT_CONTINUE:      return "AST_STMT_CONTINUE";
  case AST_STMT_RETURN:        return "AST_STMT_RETURN";
  case AST_STMT_YIELD:         return "AST_STMT_YIELD";
  case AST_STMT_BLOCK:         return "AST_STMT_BLOCK";
  case AST_STMT_EXPR:          return "AST_STMT_EXPR";
  case AST_DECL_VAR:           return "AST_DECL_VAR";
  case AST_DECL_FUN:           return "AST_DECL_FUN";
  case AST_DECL_GEN:           return "AST_DECL_GEN";
  case AST_DECL_STRUCT:        return "AST_DECL_STRUCT";
  case AST_DECL_UNION:         return "AST_DECL_UNION";
  case AST_DECL_ENUM:          return "AST_DECL_ENUM";
  case AST_DECL_MOD:           return "AST_DECL_MOD";
  case AST_DECL_MEMBER:        return "AST_DECL_MEMBER";
  case AST_PARAM:              return "AST_PARAM";
  case AST_VARIADIC_PARAM:     return "AST_VARIADIC_PARAM";
  case AST_GENERIC_PARAM:      return "AST_GENERIC_PARAM";
  case AST_LOOP_VAR:           return "AST_LOOP_VAR";
  case AST_ENUMERATOR:         return "AST_ENUMERATOR";
  case AST_PROG:               return "AST_PROG";
  default: unreachable();
  }

  return NULL;
}

void ast_list_json_dump(FILE* stream, list_t* list)
{
  if (list == NULL)
  {
    fprintf(stream, "null");
    return;
  }

  fputc('[', stream);
  for (list_node_t* elem = list_front_node(list); elem != NULL; elem = list_node_next(elem))
  {
    ast_json_dump(stream, (ast_node_t*)list_node_get(elem));
    if (list_node_next(elem) != NULL)
      fputc(',', stream);
  }
  fputc(']', stream);
}

void ast_json_dump(FILE* stream, ast_node_t* root)
{
  if (root == NULL)
  {
    fprintf(stream, "null");
    return;
  }

  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_string(root->kind));

  switch (root->kind)
  {
  case AST_UNKNOWN:
    break;
  case AST_ID:
    fprintf(stream, ",\"id\":\"%s\"", ((token_id_t*)root->tok)->value);
    break;
  case AST_TYPE_MUT:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, root->type_mut.base_type);
    break;
  case AST_TYPE_CONST:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, root->type_const.base_type);
    break;
  case AST_TYPE_PTR:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, root->type_ptr.base_type);
    break;
  case AST_TYPE_ARRAY:
    fprintf(stream, ",\"size\":");
    ast_json_dump(stream, root->type_array.size);
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, root->type_array.base_type);
    break;
  case AST_TYPE_REF:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, root->type_ref.base_type);
    break;
  case AST_TYPE_OPTIONAL:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, root->type_nullable.base_type);
    break;
  case AST_TYPE_FUN:
    fprintf(stream, ",\"params\":");
    ast_list_json_dump(stream, root->type_fun.params);
    fprintf(stream, ",\"ret_type\":");
    ast_json_dump(stream, root->type_fun.ret_type);
    break;
  case AST_TYPE_GEN:
    fprintf(stream, ",\"params\":");
    ast_list_json_dump(stream, root->type_gen.params);
    fprintf(stream, ",\"ret_type\":");
    ast_json_dump(stream, root->type_gen.ret_type);
    break;
  case AST_TYPE_TYPE:
  case AST_TYPE_BUILTIN_I8:
  case AST_TYPE_BUILTIN_I16:
  case AST_TYPE_BUILTIN_I32:
  case AST_TYPE_BUILTIN_I64:
  case AST_TYPE_BUILTIN_ISIZE:
  case AST_TYPE_BUILTIN_U8:
  case AST_TYPE_BUILTIN_U16:
  case AST_TYPE_BUILTIN_U32:
  case AST_TYPE_BUILTIN_U64:
  case AST_TYPE_BUILTIN_USIZE:
  case AST_TYPE_BUILTIN_F32:
  case AST_TYPE_BUILTIN_F64:
  case AST_TYPE_BUILTIN_BOOL:
  case AST_TYPE_BUILTIN_UNIT:
    break;
  case AST_EXPR_LIT_INT:
    fprintf(stream, ",\"value\":%llu", ((token_lit_int_t*)root->tok)->value);
    break;
  case AST_EXPR_LIT_FLT:
    fprintf(stream, ",\"value\":%Lf", ((token_lit_flt_t*)root->tok)->value);
    break;
  case AST_EXPR_LIT_STR:
    fprintf(stream, ",\"value\":\"%s\"", ((token_lit_str_t*)root->tok)->value);
    break;
  case AST_EXPR_LIT_CHAR:
    fprintf(stream, ",\"value\":\"%.*s\"", root->tok->loc->len, root->tok->loc->cur);
    break;
  case AST_EXPR_LIT_BOOL:
    fprintf(stream, ",\"value\":%s", ((token_lit_bool_t*)root->tok)->value ? "true" : "false");
    break;
  case AST_EXPR_LIT_NULL:
    break;
  case AST_EXPR_OP:
    fprintf(stream, ",\"op\":\"%s\"", op_kind_to_string(root->expr_op.kind));
    if (op_is_unary(root->expr_op.kind))
    {
      fprintf(stream, ",\"arg\":");
      ast_json_dump(stream, root->expr_op.op_unary.arg);
    }
    else if (op_is_binary(root->expr_op.kind))
    {
      fprintf(stream, ",\"lhs\":");
      ast_json_dump(stream, root->expr_op.op_binary.lhs);
      fprintf(stream, ",\"rhs\":");
      ast_json_dump(stream, root->expr_op.op_binary.rhs);
    }
    else if (root->expr_op.kind == OP_CALL)
    {
      fprintf(stream, ",\"callee\":");
      ast_json_dump(stream, root->expr_op.op_call.callee);
      fprintf(stream, ",\"args\":");
      ast_list_json_dump(stream, root->expr_op.op_call.args);
    }
    break;
  case AST_STMT_IF:
    fprintf(stream, ",\"cond\":");
    ast_json_dump(stream, root->stmt_if.cond);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, root->stmt_if.stmt);
    fprintf(stream, ",\"stmt_else\":");
    ast_json_dump(stream, root->stmt_if.stmt_else);
    break;
  case AST_STMT_FOR:
    fprintf(stream, ",\"var\":");
    ast_json_dump(stream, root->stmt_for.var);
    fprintf(stream, ",\"range\":");
    ast_json_dump(stream, root->stmt_for.range);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, root->stmt_for.stmt);
    break;
  case AST_STMT_WHILE:
    fprintf(stream, ",\"cond\":");
    ast_json_dump(stream, root->stmt_while.cond);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, root->stmt_while.stmt);
    break;
  case AST_STMT_BREAK:
    break;
  case AST_STMT_CONTINUE:
    break;
  case AST_STMT_RETURN:
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, root->stmt_return.expr);
    break;
  case AST_STMT_YIELD:
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, root->stmt_yield.expr);
    break;
  case AST_STMT_BLOCK:
    fprintf(stream, ",\"stmts\":");
    ast_list_json_dump(stream, root->stmt_block.stmts);
    break;
  case AST_STMT_EXPR:
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, root->stmt_expr.expr);
    break;
  case AST_DECL_VAR:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->decl_var.id);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, root->decl_var.type);
    fprintf(stream, ",\"init\":");
    ast_json_dump(stream, root->decl_var.init);
    break;
  case AST_DECL_FUN:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->decl_fun.id);
    fprintf(stream, ",\"generic_params\":");
    ast_list_json_dump(stream, root->decl_fun.generic_params);
    fprintf(stream, ",\"params\":");
    ast_list_json_dump(stream, root->decl_fun.params);
    fprintf(stream, ",\"ret_type\":");
    ast_json_dump(stream, root->decl_fun.ret_type);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, root->decl_fun.stmt);
    break;
  case AST_DECL_GEN:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->decl_gen.id);
    fprintf(stream, ",\"generic_params\":");
    ast_list_json_dump(stream, root->decl_gen.generic_params);
    fprintf(stream, ",\"params\":");
    ast_list_json_dump(stream, root->decl_gen.params);
    fprintf(stream, ",\"ret_type\":");
    ast_json_dump(stream, root->decl_gen.ret_type);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, root->decl_gen.stmt);
    break;
  case AST_DECL_STRUCT:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->decl_struct.id);
    fprintf(stream, ",\"generic_params\":");
    ast_list_json_dump(stream, root->decl_struct.generic_params);
    fprintf(stream, ",\"members\":");
    ast_list_json_dump(stream, root->decl_struct.members);
    break;
  case AST_DECL_UNION:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->decl_union.id);
    fprintf(stream, ",\"members\":");
    ast_list_json_dump(stream, root->decl_union.members);
    break;
  case AST_DECL_ENUM:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->decl_enum.id);
    fprintf(stream, ",\"members\":");
    ast_list_json_dump(stream, root->decl_enum.members);
    break;
  case AST_DECL_MOD:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->decl_mod.id);
    fprintf(stream, ",\"members\":");
    ast_list_json_dump(stream, root->decl_mod.members);
    break;
  case AST_DECL_MEMBER:
    fprintf(stream, ",\"decl\":");
    ast_json_dump(stream, root->decl_member.decl);
    break;
  case AST_PARAM:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->param.id);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, root->param.type);
    fprintf(stream, ",\"init\":");
    ast_json_dump(stream, root->param.init);
    break;
  case AST_VARIADIC_PARAM:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->variadic_param.id);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, root->variadic_param.type);
    break;
  case AST_GENERIC_PARAM:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->generic_param.id);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, root->generic_param.type);
    break;
  case AST_LOOP_VAR:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->loop_var.id);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, root->loop_var.type);
    break;
  case AST_ENUMERATOR:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->enumerator.id);
    break;
  case AST_PROG:
    fprintf(stream, ",\"decls\":");
    ast_list_json_dump(stream, root->prog.decls);
    break;
  default:
    unreachable();
  }

  fputc('}', stream);
}
