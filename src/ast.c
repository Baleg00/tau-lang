#include "ast.h"

#include <stdlib.h>

#include "util.h"

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
  case AST_TYPE_STATIC:
    ast_node_free(node->type_static.base_type);
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
  case AST_TYPE_NULLABLE:
    ast_node_free(node->type_nullable.base_type);
    break;
  case AST_TYPE_FUN:
    list_for_each(node->type_fun.args, ast_node_free);
    list_free(node->type_fun.args);
    ast_node_free(node->type_fun.ret_type);
    break;
  case AST_TYPE_GEN:
    list_for_each(node->type_gen.args, ast_node_free);
    list_free(node->type_gen.args);
    ast_node_free(node->type_gen.ret_type);
    break;
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
    list_for_each(node->expr_op.args, ast_node_free);
    list_free(node->expr_op.args);
    break;
  case AST_EXPR_LAMBDA_FUN:
    list_for_each(node->expr_lambda_fun.args, ast_node_free);
    list_free(node->expr_lambda_fun.args);
    ast_node_free(node->expr_lambda_fun.ret_type);
    ast_node_free(node->expr_lambda_fun.stmt);
    break;
  case AST_EXPR_LAMBDA_GEN:
    list_for_each(node->expr_lambda_gen.args, ast_node_free);
    list_free(node->expr_lambda_gen.args);
    ast_node_free(node->expr_lambda_gen.ret_type);
    ast_node_free(node->expr_lambda_gen.stmt);
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
  case AST_STMT_WHEN:
    ast_node_free(node->stmt_when.expr);
    list_for_each(node->stmt_when.cases, ast_node_free);
    list_free(node->stmt_when.cases);
    break;
  case AST_STMT_WHEN_CASE:
    ast_node_free(node->stmt_when_case.cond);
    ast_node_free(node->stmt_when_case.stmt);
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
    ast_node_free(node->decl_fun.id);
    list_for_each(node->decl_fun.args, ast_node_free);
    list_free(node->decl_fun.args);
    ast_node_free(node->decl_fun.ret_type);
    ast_node_free(node->decl_fun.stmt);
    break;
  case AST_DECL_GEN:
    ast_node_free(node->decl_gen.id);
    list_for_each(node->decl_gen.args, ast_node_free);
    list_free(node->decl_gen.args);
    ast_node_free(node->decl_gen.ret_type);
    ast_node_free(node->decl_gen.stmt);
    break;
  case AST_DECL_STRUCT:
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
  case AST_ARG:
    ast_node_free(node->arg.id);
    ast_node_free(node->arg.type);
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
  case AST_TYPE_STATIC:        return "AST_TYPE_STATIC";
  case AST_TYPE_PTR:           return "AST_TYPE_PTR";
  case AST_TYPE_ARRAY:         return "AST_TYPE_ARRAY";
  case AST_TYPE_REF:           return "AST_TYPE_REF";
  case AST_TYPE_NULLABLE:      return "AST_TYPE_NULLABLE";
  case AST_TYPE_FUN:           return "AST_TYPE_FUN";
  case AST_TYPE_GEN:           return "AST_TYPE_GEN";
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
  case AST_EXPR_LAMBDA_FUN:    return "AST_EXPR_LAMBDA_FUN";
  case AST_EXPR_LAMBDA_GEN:    return "AST_EXPR_LAMBDA_GEN";
  case AST_STMT_IF:            return "AST_STMT_IF";
  case AST_STMT_FOR:           return "AST_STMT_FOR";
  case AST_STMT_WHILE:         return "AST_STMT_WHILE";
  case AST_STMT_WHEN:          return "AST_STMT_WHEN";
  case AST_STMT_WHEN_CASE:     return "AST_STMT_WHEN_CASE";
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
  case AST_ARG:                return "AST_ARG";
  case AST_PROG:               return "AST_PROG";
  }

  unreachable();

  return NULL;
}

void ast_list_json_dump(FILE* stream, list_t* list)
{
  fputc('[', stream);
  for (list_elem_t* elem = list_front_elem(list); elem != NULL; elem = list_elem_next(elem))
  {
    ast_json_dump(stream, (ast_node_t*)list_elem_data(elem));
    if (list_elem_next(elem) != NULL)
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
    fprintf(stream, ",\"id\":\"%s\"", root->tok->id.value);
    break;
  case AST_TYPE_MUT:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, root->type_mut.base_type);
    break;
  case AST_TYPE_CONST:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, root->type_const.base_type);
    break;
  case AST_TYPE_STATIC:
    fprintf(stream, ",\"base_type\":"); 
    ast_json_dump(stream, root->type_static.base_type);
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
  case AST_TYPE_NULLABLE:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, root->type_nullable.base_type);
    break;
  case AST_TYPE_FUN:
    fprintf(stream, ",\"args\":");
    ast_list_json_dump(stream, root->type_fun.args);
    fprintf(stream, ",\"ret_type\":");
    ast_json_dump(stream, root->type_fun.ret_type);
    break;
  case AST_TYPE_GEN:
    fprintf(stream, ",\"args\":");
    ast_list_json_dump(stream, root->type_gen.args);
    fprintf(stream, ",\"ret_type\":");
    ast_json_dump(stream, root->type_gen.ret_type);
    break;
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
    fprintf(stream, ",\"value\":%llu", root->tok->lit_int.value);
    break;
  case AST_EXPR_LIT_FLT:
    fprintf(stream, ",\"value\":%Lf", root->tok->lit_flt.value);
    break;
  case AST_EXPR_LIT_STR:
    fprintf(stream, ",\"value\":\"%s\"", root->tok->lit_str.value);
    break;
  case AST_EXPR_LIT_CHAR:
    fprintf(stream, ",\"value\":\"%s\"", root->tok->lit_char.value);
    break;
  case AST_EXPR_LIT_BOOL:
    fprintf(stream, ",\"value\":%s", root->tok->kind == TOK_LIT_BOOL_TRUE ? "true" : "false");
    break;
  case AST_EXPR_LIT_NULL:
    break;
  case AST_EXPR_OP:
    fprintf(stream, ",\"op\":\"%s\"", op_kind_to_string(root->expr_op.kind));
    fprintf(stream, ",\"args\":");
    ast_list_json_dump(stream, root->expr_op.args);
    break;
  case AST_EXPR_LAMBDA_FUN:
    fprintf(stream, ",\"args\":");
    ast_list_json_dump(stream, root->expr_lambda_fun.args);
    fprintf(stream, ",\"ret_type\":");
    ast_json_dump(stream, root->expr_lambda_fun.ret_type);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, root->expr_lambda_fun.stmt);
    break;
  case AST_EXPR_LAMBDA_GEN:
    fprintf(stream, ",\"args\":");
    ast_list_json_dump(stream, root->expr_lambda_gen.args);
    fprintf(stream, ",\"ret_type\":");
    ast_json_dump(stream, root->expr_lambda_gen.ret_type);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, root->expr_lambda_gen.stmt);
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
  case AST_STMT_WHEN:
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, root->stmt_when.expr);
    fprintf(stream, ",\"cases\":");
    ast_list_json_dump(stream, root->stmt_when.cases);
    break;
  case AST_STMT_WHEN_CASE:
    fprintf(stream, ",\"cond\":");
    ast_json_dump(stream, root->stmt_when_case.cond);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, root->stmt_when_case.stmt);
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
    fprintf(stream, ",\"args\":");
    ast_list_json_dump(stream, root->decl_fun.args);
    fprintf(stream, ",\"ret_type\":");
    ast_json_dump(stream, root->decl_fun.ret_type);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, root->decl_fun.stmt);
    break;
  case AST_DECL_GEN:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->decl_gen.id);
    fprintf(stream, ",\"args\":");
    ast_list_json_dump(stream, root->decl_gen.args);
    fprintf(stream, ",\"ret_type\":");
    ast_json_dump(stream, root->decl_gen.ret_type);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, root->decl_gen.stmt);
    break;
  case AST_DECL_STRUCT:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->decl_struct.id);
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
    fprintf(stream, ",\"is_pub\":%s", root->decl_member.is_pub ? "true" : "false");
    fprintf(stream, ",\"decl\":");
    ast_json_dump(stream, root->decl_member.decl);
    break;
  case AST_ARG:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, root->arg.id);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, root->arg.type);
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
