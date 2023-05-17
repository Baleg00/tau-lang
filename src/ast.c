#include "ast.h"

#include "util.h"

#include "list.h"
#include "stack.h"

#include "location.h"
#include "token.h"
#include "op.h"
#include "typedesc.h"

#include "memtrace.h"

ast_node_t* ast_node_init(ast_kind_t kind, token_t* tok, size_t size)
{
  ast_node_t* node = (ast_node_t*)calloc(1, size);
  assert(node != NULL);

  node->kind = kind;
  node->tok = tok;
  
  return node;
}

LIST_FOR_EACH_FUNC_DECL(ast_node_free, ast_node_t);

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
    ast_node_free(((ast_type_mut_t*)node)->base_type);
    typedesc_free(((ast_type_mut_t*)node)->desc);
    break;
  case AST_TYPE_CONST:
    ast_node_free(((ast_type_const_t*)node)->base_type);
    typedesc_free(((ast_type_const_t*)node)->desc);
    break;
  case AST_TYPE_PTR:
    ast_node_free(((ast_type_ptr_t*)node)->base_type);
    typedesc_free(((ast_type_ptr_t*)node)->desc);
    break;
  case AST_TYPE_ARRAY:
    ast_node_free(((ast_type_array_t*)node)->base_type);
    ast_node_free(((ast_type_array_t*)node)->size);
    typedesc_free(((ast_type_array_t*)node)->desc);
    break;
  case AST_TYPE_REF:
    ast_node_free(((ast_type_ref_t*)node)->base_type);
    typedesc_free(((ast_type_ref_t*)node)->desc);
    break;
  case AST_TYPE_OPT:
    ast_node_free(((ast_type_opt_t*)node)->base_type);
    typedesc_free(((ast_type_opt_t*)node)->desc);
    break;
  case AST_TYPE_FUN:
    list_for_each(((ast_type_fun_t*)node)->params, LIST_FOR_EACH_FUNC_NAME(ast_node_free));
    list_free(((ast_type_fun_t*)node)->params);
    ast_node_free(((ast_type_fun_t*)node)->return_type);
    typedesc_free(((ast_type_fun_t*)node)->desc);
    break;
  case AST_TYPE_GEN:
    list_for_each(((ast_type_gen_t*)node)->params, LIST_FOR_EACH_FUNC_NAME(ast_node_free));
    list_free(((ast_type_gen_t*)node)->params);
    ast_node_free(((ast_type_gen_t*)node)->yield_type);
    typedesc_free(((ast_type_gen_t*)node)->desc);
    break;
  case AST_TYPE_TYPE:
  case AST_TYPE_I8:
  case AST_TYPE_I16:
  case AST_TYPE_I32:
  case AST_TYPE_I64:
  case AST_TYPE_ISIZE:
  case AST_TYPE_U8:
  case AST_TYPE_U16:
  case AST_TYPE_U32:
  case AST_TYPE_U64:
  case AST_TYPE_USIZE:
  case AST_TYPE_F32:
  case AST_TYPE_F64:
  case AST_TYPE_BOOL:
  case AST_TYPE_UNIT:
    break;
  case AST_TYPE_MEMBER:
    ast_node_free(((ast_type_member_t*)node)->owner);
    ast_node_free(((ast_type_member_t*)node)->member);
    typedesc_free(((ast_type_member_t*)node)->desc);
    break;
  case AST_EXPR_LIT_INT:
  case AST_EXPR_LIT_FLT:
  case AST_EXPR_LIT_STR:
  case AST_EXPR_LIT_CHAR:
  case AST_EXPR_LIT_BOOL:
  case AST_EXPR_LIT_NULL:
    typedesc_free(((ast_expr_lit_t*)node)->desc);
    break;
  case AST_EXPR_OP:
    if (op_is_unary(((ast_expr_op_t*)node)->op_kind))
      ast_node_free(((ast_expr_op_un_t*)node)->param);
    else if (op_is_binary(((ast_expr_op_t*)node)->op_kind))
    {
      ast_node_free(((ast_expr_op_bin_t*)node)->lhs);
      ast_node_free(((ast_expr_op_bin_t*)node)->rhs);
    }
    else if (((ast_expr_op_t*)node)->op_kind == OP_CALL)
    {
      ast_node_free(((ast_expr_op_call_t*)node)->callee);
      list_for_each(((ast_expr_op_call_t*)node)->params, LIST_FOR_EACH_FUNC_NAME(ast_node_free));
      list_free(((ast_expr_op_call_t*)node)->params);
    }
    typedesc_free(((ast_expr_op_t*)node)->desc);
    break;
  case AST_STMT_IF:
    ast_node_free(((ast_stmt_if_t*)node)->cond);
    ast_node_free(((ast_stmt_if_t*)node)->stmt);
    ast_node_free(((ast_stmt_if_t*)node)->stmt_else);
    break;
  case AST_STMT_FOR:
    ast_node_free(((ast_stmt_for_t*)node)->var);
    ast_node_free(((ast_stmt_for_t*)node)->range);
    ast_node_free(((ast_stmt_for_t*)node)->stmt);
    break;
  case AST_STMT_WHILE:
    ast_node_free(((ast_stmt_while_t*)node)->cond);
    ast_node_free(((ast_stmt_while_t*)node)->stmt);
    break;
  case AST_STMT_RETURN:
    ast_node_free(((ast_stmt_return_t*)node)->expr);
    break;
  case AST_STMT_YIELD:
    ast_node_free(((ast_stmt_yield_t*)node)->expr);
    break;
  case AST_STMT_BLOCK:
    list_for_each(((ast_stmt_block_t*)node)->stmts, LIST_FOR_EACH_FUNC_NAME(ast_node_free));
    list_free(((ast_stmt_block_t*)node)->stmts);
    break;
  case AST_STMT_EXPR:
    ast_node_free(((ast_stmt_expr_t*)node)->expr);
    break;
  case AST_DECL_VAR:
    ast_node_free(((ast_decl_var_t*)node)->id);
    ast_node_free(((ast_decl_var_t*)node)->type);
    ast_node_free(((ast_decl_var_t*)node)->init);
    typedesc_free(((ast_decl_var_t*)node)->desc);
    break;
  case AST_DECL_LOOP_VAR:
    ast_node_free(((ast_decl_loop_var_t*)node)->id);
    ast_node_free(((ast_decl_loop_var_t*)node)->type);
    typedesc_free(((ast_decl_loop_var_t*)node)->desc);
    break;
  case AST_DECL_FUN:
    ast_node_free(((ast_decl_fun_t*)node)->id);
    if (((ast_decl_fun_t*)node)->params != NULL)
    {
      list_for_each(((ast_decl_fun_t*)node)->params, LIST_FOR_EACH_FUNC_NAME(ast_node_free));
      list_free(((ast_decl_fun_t*)node)->params);
    }
    ast_node_free(((ast_decl_fun_t*)node)->return_type);
    ast_node_free(((ast_decl_fun_t*)node)->stmt);
    typedesc_free(((ast_decl_fun_t*)node)->desc);
    break;
  case AST_DECL_GEN:
    ast_node_free(((ast_decl_gen_t*)node)->id);
    if (((ast_decl_gen_t*)node)->params != NULL)
    {
      list_for_each(((ast_decl_gen_t*)node)->params, LIST_FOR_EACH_FUNC_NAME(ast_node_free));
      list_free(((ast_decl_gen_t*)node)->params);
    }
    ast_node_free(((ast_decl_gen_t*)node)->yield_type);
    ast_node_free(((ast_decl_gen_t*)node)->stmt);
    typedesc_free(((ast_decl_gen_t*)node)->desc);
    break;
  case AST_DECL_STRUCT:
    ast_node_free(((ast_decl_struct_t*)node)->id);
    list_for_each(((ast_decl_struct_t*)node)->members, LIST_FOR_EACH_FUNC_NAME(ast_node_free));
    list_free(((ast_decl_struct_t*)node)->members);
    typedesc_free(((ast_decl_struct_t*)node)->desc);
    break;
  case AST_DECL_UNION:
    ast_node_free(((ast_decl_union_t*)node)->id);
    list_for_each(((ast_decl_union_t*)node)->members, LIST_FOR_EACH_FUNC_NAME(ast_node_free));
    list_free(((ast_decl_union_t*)node)->members);
    typedesc_free(((ast_decl_union_t*)node)->desc);
    break;
  case AST_DECL_ENUM:
    ast_node_free(((ast_decl_enum_t*)node)->id);
    list_for_each(((ast_decl_enum_t*)node)->values, LIST_FOR_EACH_FUNC_NAME(ast_node_free));
    list_free(((ast_decl_enum_t*)node)->values);
    typedesc_free(((ast_decl_enum_t*)node)->desc);
    break;
  case AST_DECL_MOD:
    ast_node_free(((ast_decl_mod_t*)node)->id);
    list_for_each(((ast_decl_mod_t*)node)->decls, LIST_FOR_EACH_FUNC_NAME(ast_node_free));
    list_free(((ast_decl_mod_t*)node)->decls);
    typedesc_free(((ast_decl_mod_t*)node)->desc);
    break;
  case AST_DECL_GENERIC:
    list_for_each(((ast_decl_generic_t*)node)->params, LIST_FOR_EACH_FUNC_NAME(ast_node_free));
    list_free(((ast_decl_generic_t*)node)->params);
    ast_node_free(((ast_decl_generic_t*)node)->decl);
    break;
  case AST_PARAM:
    ast_node_free(((ast_param_t*)node)->id);
    ast_node_free(((ast_param_t*)node)->type);
    typedesc_free(((ast_param_t*)node)->desc);
    break;
  case AST_PARAM_DEFAULT:
    ast_node_free(((ast_param_default_t*)node)->id);
    ast_node_free(((ast_param_default_t*)node)->type);
    ast_node_free(((ast_param_default_t*)node)->init);
    typedesc_free(((ast_param_default_t*)node)->desc);
    break;
  case AST_PARAM_VARIADIC:
    ast_node_free(((ast_param_variadic_t*)node)->id);
    ast_node_free(((ast_param_variadic_t*)node)->type);
    typedesc_free(((ast_param_variadic_t*)node)->desc);
    break;
  case AST_PARAM_GENERIC:
    ast_node_free(((ast_param_generic_t*)node)->id);
    ast_node_free(((ast_param_generic_t*)node)->type);
    typedesc_free(((ast_param_generic_t*)node)->desc);
    break;
  case AST_ENUMERATOR:
    ast_node_free(((ast_enumerator_t*)node)->id);
    typedesc_free(((ast_enumerator_t*)node)->desc);
    break;
  case AST_PROG:
    list_for_each(((ast_prog_t*)node)->decls, LIST_FOR_EACH_FUNC_NAME(ast_node_free));
    list_free(((ast_prog_t*)node)->decls);
    break;
  }

  free(node);
}

void ast_json_dump_list(FILE* stream, list_t* list)
{
  if (list == NULL)
  {
    fprintf(stream, "null");
    return;
  }

  fputc('[', stream);
  for (list_node_t* it = list_front_node(list); it != NULL; it = list_node_next(it))
  {
    ast_json_dump(stream, (ast_node_t*)list_node_get(it));
    if (list_node_next(it) != NULL)
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
    fprintf(stream, ",\"id\":\"%.*s\"", root->tok->loc->len, root->tok->loc->cur);
    break;
  case AST_TYPE_MUT:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, ((ast_type_mut_t*)root)->base_type);
    break;
  case AST_TYPE_CONST:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, ((ast_type_const_t*)root)->base_type);
    break;
  case AST_TYPE_PTR:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, ((ast_type_ptr_t*)root)->base_type);
    break;
  case AST_TYPE_ARRAY:
    fprintf(stream, ",\"size\":");
    ast_json_dump(stream, ((ast_type_array_t*)root)->size);
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, ((ast_type_array_t*)root)->base_type);
    break;
  case AST_TYPE_REF:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, ((ast_type_ref_t*)root)->base_type);
    break;
  case AST_TYPE_OPT:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, ((ast_type_opt_t*)root)->base_type);
    break;
  case AST_TYPE_FUN:
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ((ast_type_fun_t*)root)->params);
    fprintf(stream, ",\"return_type\":");
    ast_json_dump(stream, ((ast_type_fun_t*)root)->return_type);
    break;
  case AST_TYPE_GEN:
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ((ast_type_gen_t*)root)->params);
    fprintf(stream, ",\"yield_type\":");
    ast_json_dump(stream, ((ast_type_gen_t*)root)->yield_type);
    break;
  case AST_TYPE_TYPE:
  case AST_TYPE_I8:
  case AST_TYPE_I16:
  case AST_TYPE_I32:
  case AST_TYPE_I64:
  case AST_TYPE_ISIZE:
  case AST_TYPE_U8:
  case AST_TYPE_U16:
  case AST_TYPE_U32:
  case AST_TYPE_U64:
  case AST_TYPE_USIZE:
  case AST_TYPE_F32:
  case AST_TYPE_F64:
  case AST_TYPE_BOOL:
  case AST_TYPE_UNIT:
    break;
  case AST_TYPE_MEMBER:
    fprintf(stream, ",\"owner\":");
    ast_json_dump(stream, ((ast_type_member_t*)root)->owner);
    fprintf(stream, ",\"member\":");
    ast_json_dump(stream, ((ast_type_member_t*)root)->member);
    break;
  case AST_EXPR_LIT_INT:
  case AST_EXPR_LIT_FLT:
  case AST_EXPR_LIT_STR:
  case AST_EXPR_LIT_CHAR:
  case AST_EXPR_LIT_BOOL:
  case AST_EXPR_LIT_NULL:
    fprintf(stream, ",\"value\":\"%.*s\"", root->tok->loc->len, root->tok->loc->cur);
    break;
  case AST_EXPR_OP:
    fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_string(((ast_expr_op_t*)root)->op_kind));
    if (op_is_unary(((ast_expr_op_t*)root)->op_kind))
    {
      fprintf(stream, ",\"param\":");
      ast_json_dump(stream, ((ast_expr_op_un_t*)root)->param);
    }
    else if (op_is_binary(((ast_expr_op_t*)root)->op_kind))
    {
      fprintf(stream, ",\"lhs\":");
      ast_json_dump(stream, ((ast_expr_op_bin_t*)root)->lhs);
      fprintf(stream, ",\"rhs\":");
      ast_json_dump(stream, ((ast_expr_op_bin_t*)root)->rhs);
    }
    else if (((ast_expr_op_t*)root)->op_kind == OP_CALL)
    {
      fprintf(stream, ",\"callee\":");
      ast_json_dump(stream, ((ast_expr_op_call_t*)root)->callee);
      fprintf(stream, ",\"args\":");
      ast_json_dump_list(stream, ((ast_expr_op_call_t*)root)->params);
    }
    break;
  case AST_STMT_IF:
    fprintf(stream, ",\"cond\":");
    ast_json_dump(stream, ((ast_stmt_if_t*)root)->cond);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ((ast_stmt_if_t*)root)->stmt);
    fprintf(stream, ",\"stmt_else\":");
    ast_json_dump(stream, ((ast_stmt_if_t*)root)->stmt_else);
    break;
  case AST_STMT_FOR:
    fprintf(stream, ",\"var\":");
    ast_json_dump(stream, ((ast_stmt_for_t*)root)->var);
    fprintf(stream, ",\"range\":");
    ast_json_dump(stream, ((ast_stmt_for_t*)root)->range);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ((ast_stmt_for_t*)root)->stmt);
    break;
  case AST_STMT_WHILE:
    fprintf(stream, ",\"cond\":");
    ast_json_dump(stream, ((ast_stmt_while_t*)root)->cond);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ((ast_stmt_while_t*)root)->stmt);
    break;
  case AST_STMT_BREAK:
    break;
  case AST_STMT_CONTINUE:
    break;
  case AST_STMT_RETURN:
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ((ast_stmt_return_t*)root)->expr);
    break;
  case AST_STMT_YIELD:
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ((ast_stmt_yield_t*)root)->expr);
    break;
  case AST_STMT_BLOCK:
    fprintf(stream, ",\"stmts\":");
    ast_json_dump_list(stream, ((ast_stmt_block_t*)root)->stmts);
    break;
  case AST_STMT_EXPR:
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ((ast_stmt_expr_t*)root)->expr);
    break;
  case AST_DECL_VAR:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_var_t*)root)->id);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, ((ast_decl_var_t*)root)->type);
    fprintf(stream, ",\"init\":");
    ast_json_dump(stream, ((ast_decl_var_t*)root)->init);
    break;
  case AST_DECL_LOOP_VAR:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_loop_var_t*)root)->id);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, ((ast_decl_loop_var_t*)root)->type);
    break;
  case AST_DECL_FUN:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_fun_t*)root)->id);
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ((ast_decl_fun_t*)root)->params);
    fprintf(stream, ",\"return_type\":");
    ast_json_dump(stream, ((ast_decl_fun_t*)root)->return_type);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ((ast_decl_fun_t*)root)->stmt);
    break;
  case AST_DECL_GEN:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_gen_t*)root)->id);
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ((ast_decl_gen_t*)root)->params);
    fprintf(stream, ",\"yield_type\":");
    ast_json_dump(stream, ((ast_decl_gen_t*)root)->yield_type);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ((ast_decl_gen_t*)root)->stmt);
    break;
  case AST_DECL_STRUCT:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_struct_t*)root)->id);
    fprintf(stream, ",\"members\":");
    ast_json_dump_list(stream, ((ast_decl_struct_t*)root)->members);
    break;
  case AST_DECL_UNION:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_union_t*)root)->id);
    fprintf(stream, ",\"members\":");
    ast_json_dump_list(stream, ((ast_decl_union_t*)root)->members);
    break;
  case AST_DECL_ENUM:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_enum_t*)root)->id);
    fprintf(stream, ",\"values\":");
    ast_json_dump_list(stream, ((ast_decl_enum_t*)root)->values);
    break;
  case AST_DECL_MOD:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_mod_t*)root)->id);
    fprintf(stream, ",\"decls\":");
    ast_json_dump_list(stream, ((ast_decl_mod_t*)root)->decls);
    break;
  case AST_PARAM:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_param_t*)root)->id);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, ((ast_param_t*)root)->type);
    break;
  case AST_PARAM_DEFAULT:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_param_default_t*)root)->id);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, ((ast_param_default_t*)root)->type);
    fprintf(stream, ",\"init\":");
    ast_json_dump(stream, ((ast_param_default_t*)root)->init);
    break;
  case AST_PARAM_VARIADIC:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_param_variadic_t*)root)->id);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, ((ast_param_variadic_t*)root)->type);
    break;
  case AST_PARAM_GENERIC:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_param_generic_t*)root)->id);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, ((ast_param_generic_t*)root)->type);
    break;
  case AST_ENUMERATOR:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_enumerator_t*)root)->id);
    break;
  case AST_PROG:
    fprintf(stream, ",\"decls\":");
    ast_json_dump_list(stream, ((ast_prog_t*)root)->decls);
    break;
  default:
    unreachable();
  }

  fputc('}', stream);
}

void ast_json_dump_flat_list(FILE* stream, stack_t* nodes, list_t* list)
{
  if (list == NULL)
  {
    fprintf(stream, "null");
    return;
  }

  fputc('[', stream);
  for (list_node_t* it = list_front_node(list); it != NULL; it = list_node_next(it))
  {
    ast_node_t* node = (ast_node_t*)list_node_get(it);
    stack_push(nodes, node);

    fprintf(stream, "\"%p\"", node);
    
    if (list_node_next(it) != NULL)
      fputc(',', stream);
  }
  fputc(']', stream);
}

void ast_json_dump_flat(FILE* stream, ast_node_t* root)
{
  stack_t* nodes = stack_init();
  list_t* discovered = list_init();

  stack_push(nodes, root);

  fprintf(stream, "{");

  bool is_first = true;

  while (!stack_empty(nodes))
  {
    root = (ast_node_t*)stack_pop(nodes);

    if (root == NULL)
      continue;

    bool is_discovered = false;

    LIST_FOR_LOOP(it, discovered)
      if (root == (ast_node_t*)list_node_get(it))
      {
        is_discovered = true;
        break;
      }

    if (!is_discovered)
    {
      list_push_back(discovered, root);

      if (!is_first)
        fputc(',', stream);
      else
        is_first = false;

      fprintf(stream, "\"%p\":{\"kind\":\"%s\"", root, ast_kind_to_string(root->kind));

      switch (root->kind)
      {
      case AST_UNKNOWN:
        break;
      case AST_ID:
        fprintf(stream, ",\"id\":\"%.*s\"", root->tok->loc->len, root->tok->loc->cur);
        break;
      case AST_TYPE_MUT:
        fprintf(stream, ",\"base_type\":\"%p\"", ((ast_type_mut_t*)root)->base_type);
        stack_push(nodes, ((ast_type_mut_t*)root)->base_type);
        break;
      case AST_TYPE_CONST:
        fprintf(stream, ",\"base_type\":\"%p\"", ((ast_type_const_t*)root)->base_type);
        stack_push(nodes, ((ast_type_const_t*)root)->base_type);
        break;
      case AST_TYPE_PTR:
        fprintf(stream, ",\"base_type\":\"%p\"", ((ast_type_ptr_t*)root)->base_type);
        stack_push(nodes, ((ast_type_ptr_t*)root)->base_type);
        break;
      case AST_TYPE_ARRAY:
        fprintf(stream, ",\"size\":\"%p\"", ((ast_type_array_t*)root)->size);
        stack_push(nodes, ((ast_type_array_t*)root)->size);
        fprintf(stream, ",\"base_type\":\"%p\"", ((ast_type_array_t*)root)->base_type);
        stack_push(nodes, ((ast_type_array_t*)root)->base_type);
        break;
      case AST_TYPE_REF:
        fprintf(stream, ",\"base_type\":\"%p\"", ((ast_type_ref_t*)root)->base_type);
        stack_push(nodes, ((ast_type_ref_t*)root)->base_type);
        break;
      case AST_TYPE_OPT:
        fprintf(stream, ",\"base_type\":\"%p\"", ((ast_type_opt_t*)root)->base_type);
        stack_push(nodes, ((ast_type_opt_t*)root)->base_type);
        break;
      case AST_TYPE_FUN:
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_type_fun_t*)root)->params);
        fprintf(stream, ",\"return_type\":\"%p\"", ((ast_type_fun_t*)root)->return_type);
        stack_push(nodes, ((ast_type_fun_t*)root)->return_type);
        break;
      case AST_TYPE_GEN:
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_type_gen_t*)root)->params);
        fprintf(stream, ",\"yield_type\":\"%p\"", ((ast_type_gen_t*)root)->yield_type);
        stack_push(nodes, ((ast_type_gen_t*)root)->yield_type);
        break;
      case AST_TYPE_TYPE:
      case AST_TYPE_I8:
      case AST_TYPE_I16:
      case AST_TYPE_I32:
      case AST_TYPE_I64:
      case AST_TYPE_ISIZE:
      case AST_TYPE_U8:
      case AST_TYPE_U16:
      case AST_TYPE_U32:
      case AST_TYPE_U64:
      case AST_TYPE_USIZE:
      case AST_TYPE_F32:
      case AST_TYPE_F64:
      case AST_TYPE_BOOL:
      case AST_TYPE_UNIT:
        break;
      case AST_TYPE_MEMBER:
        fprintf(stream, ",\"owner\":\"%p\"", ((ast_type_member_t*)root)->owner);
        stack_push(nodes, ((ast_type_member_t*)root)->owner);
        fprintf(stream, ",\"member\":\"%p\"", ((ast_type_member_t*)root)->member);
        stack_push(nodes, ((ast_type_member_t*)root)->member);
        break;
      case AST_EXPR_LIT_INT:
      case AST_EXPR_LIT_FLT:
      case AST_EXPR_LIT_STR:
      case AST_EXPR_LIT_CHAR:
      case AST_EXPR_LIT_BOOL:
      case AST_EXPR_LIT_NULL:
        fprintf(stream, ",\"value\":\"%.*s\"", root->tok->loc->len, root->tok->loc->cur);
        break;
      case AST_EXPR_OP:
        fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_string(((ast_expr_op_t*)root)->op_kind));
        if (op_is_unary(((ast_expr_op_t*)root)->op_kind))
        {
          fprintf(stream, ",\"param\":\"%p\"", ((ast_expr_op_un_t*)root)->param);
          stack_push(nodes, ((ast_expr_op_un_t*)root)->param);
        }
        else if (op_is_binary(((ast_expr_op_t*)root)->op_kind))
        {
          fprintf(stream, ",\"lhs\":\"%p\"", ((ast_expr_op_bin_t*)root)->lhs);
          stack_push(nodes, ((ast_expr_op_bin_t*)root)->lhs);
          fprintf(stream, ",\"rhs\":\"%p\"", ((ast_expr_op_bin_t*)root)->rhs);
          stack_push(nodes, ((ast_expr_op_bin_t*)root)->rhs);
        }
        else if (((ast_expr_op_t*)root)->op_kind == OP_CALL)
        {
          fprintf(stream, ",\"callee\":\"%p\"", ((ast_expr_op_call_t*)root)->callee);
          stack_push(nodes, ((ast_expr_op_call_t*)root)->callee);
          fprintf(stream, ",\"args\":");
          ast_json_dump_flat_list(stream, nodes, ((ast_expr_op_call_t*)root)->params);
        }
        break;
      case AST_STMT_IF:
        fprintf(stream, ",\"cond\":\"%p\"", ((ast_stmt_if_t*)root)->cond);
        stack_push(nodes, ((ast_stmt_if_t*)root)->cond);
        fprintf(stream, ",\"stmt\":\"%p\"", ((ast_stmt_if_t*)root)->stmt);
        stack_push(nodes, ((ast_stmt_if_t*)root)->stmt);
        fprintf(stream, ",\"stmt_else\":\"%p\"", ((ast_stmt_if_t*)root)->stmt_else);
        stack_push(nodes, ((ast_stmt_if_t*)root)->stmt_else);
        break;
      case AST_STMT_FOR:
        fprintf(stream, ",\"var\":\"%p\"", ((ast_stmt_for_t*)root)->var);
        stack_push(nodes, ((ast_stmt_for_t*)root)->var);
        fprintf(stream, ",\"range\":\"%p\"", ((ast_stmt_for_t*)root)->range);
        stack_push(nodes, ((ast_stmt_for_t*)root)->range);
        fprintf(stream, ",\"stmt\":\"%p\"", ((ast_stmt_for_t*)root)->stmt);
        stack_push(nodes, ((ast_stmt_for_t*)root)->stmt);
        break;
      case AST_STMT_WHILE:
        fprintf(stream, ",\"cond\":\"%p\"", ((ast_stmt_while_t*)root)->cond);
        stack_push(nodes, ((ast_stmt_while_t*)root)->cond);
        fprintf(stream, ",\"stmt\":\"%p\"", ((ast_stmt_while_t*)root)->stmt);
        stack_push(nodes, ((ast_stmt_while_t*)root)->stmt);
        break;
      case AST_STMT_BREAK:
        break;
      case AST_STMT_CONTINUE:
        break;
      case AST_STMT_RETURN:
        fprintf(stream, ",\"expr\":\"%p\"", ((ast_stmt_return_t*)root)->expr);
        stack_push(nodes, ((ast_stmt_return_t*)root)->expr);
        break;
      case AST_STMT_YIELD:
        fprintf(stream, ",\"expr\":\"%p\"", ((ast_stmt_yield_t*)root)->expr);
        stack_push(nodes, ((ast_stmt_yield_t*)root)->expr);
        break;
      case AST_STMT_BLOCK:
        fprintf(stream, ",\"stmts\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_stmt_block_t*)root)->stmts);
        break;
      case AST_STMT_EXPR:
        fprintf(stream, ",\"expr\":\"%p\"", ((ast_stmt_expr_t*)root)->expr);
        stack_push(nodes, ((ast_stmt_expr_t*)root)->expr);
        break;
      case AST_DECL_VAR:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_var_t*)root)->id);
        stack_push(nodes, ((ast_decl_var_t*)root)->id);
        fprintf(stream, ",\"type\":\"%p\"", ((ast_decl_var_t*)root)->type);
        stack_push(nodes, ((ast_decl_var_t*)root)->type);
        fprintf(stream, ",\"init\":\"%p\"", ((ast_decl_var_t*)root)->init);
        stack_push(nodes, ((ast_decl_var_t*)root)->init);
        break;
      case AST_DECL_LOOP_VAR:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_loop_var_t*)root)->id);
        stack_push(nodes, ((ast_decl_loop_var_t*)root)->id);
        fprintf(stream, ",\"type\":\"%p\"", ((ast_decl_loop_var_t*)root)->type);
        stack_push(nodes, ((ast_decl_loop_var_t*)root)->type);
        break;
      case AST_DECL_FUN:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_fun_t*)root)->id);
        stack_push(nodes, ((ast_decl_fun_t*)root)->id);
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_fun_t*)root)->params);
        fprintf(stream, ",\"return_type\":\"%p\"", ((ast_decl_fun_t*)root)->return_type);
        stack_push(nodes, ((ast_decl_fun_t*)root)->return_type);
        fprintf(stream, ",\"stmt\":\"%p\"", ((ast_decl_fun_t*)root)->stmt);
        stack_push(nodes, ((ast_decl_fun_t*)root)->stmt);
        break;
      case AST_DECL_GEN:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_gen_t*)root)->id);
        stack_push(nodes, ((ast_decl_gen_t*)root)->id);
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_gen_t*)root)->params);
        fprintf(stream, ",\"yield_type\":\"%p\"", ((ast_decl_gen_t*)root)->yield_type);
        stack_push(nodes, ((ast_decl_gen_t*)root)->yield_type);
        fprintf(stream, ",\"stmt\":\"%p\"", ((ast_decl_gen_t*)root)->stmt);
        stack_push(nodes, ((ast_decl_gen_t*)root)->stmt);
        break;
      case AST_DECL_STRUCT:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_struct_t*)root)->id);
        stack_push(nodes, ((ast_decl_struct_t*)root)->id);
        fprintf(stream, ",\"members\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_struct_t*)root)->members);
        break;
      case AST_DECL_UNION:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_union_t*)root)->id);
        stack_push(nodes, ((ast_decl_union_t*)root)->id);
        fprintf(stream, ",\"members\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_union_t*)root)->members);
        break;
      case AST_DECL_ENUM:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_enum_t*)root)->id);
        stack_push(nodes, ((ast_decl_enum_t*)root)->id);
        fprintf(stream, ",\"values\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_enum_t*)root)->values);
        break;
      case AST_DECL_MOD:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_mod_t*)root)->id);
        stack_push(nodes, ((ast_decl_mod_t*)root)->id);
        fprintf(stream, ",\"decls\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_mod_t*)root)->decls);
        break;
      case AST_PARAM:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_param_t*)root)->id);
        stack_push(nodes, ((ast_param_t*)root)->id);
        fprintf(stream, ",\"type\":\"%p\"", ((ast_param_t*)root)->type);
        stack_push(nodes, ((ast_param_t*)root)->type);
        break;
      case AST_PARAM_DEFAULT:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_param_default_t*)root)->id);
        stack_push(nodes, ((ast_param_default_t*)root)->id);
        fprintf(stream, ",\"type\":\"%p\"", ((ast_param_default_t*)root)->type);
        stack_push(nodes, ((ast_param_default_t*)root)->type);
        fprintf(stream, ",\"init\":\"%p\"", ((ast_param_default_t*)root)->init);
        stack_push(nodes, ((ast_param_default_t*)root)->init);
        break;
      case AST_PARAM_VARIADIC:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_param_variadic_t*)root)->id);
        stack_push(nodes, ((ast_param_variadic_t*)root)->id);
        fprintf(stream, ",\"type\":\"%p\"", ((ast_param_variadic_t*)root)->type);
        stack_push(nodes, ((ast_param_variadic_t*)root)->type);
        break;
      case AST_PARAM_GENERIC:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_param_generic_t*)root)->id);
        stack_push(nodes, ((ast_param_generic_t*)root)->id);
        fprintf(stream, ",\"type\":\"%p\"", ((ast_param_generic_t*)root)->type);
        stack_push(nodes, ((ast_param_generic_t*)root)->type);
        break;
      case AST_ENUMERATOR:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_enumerator_t*)root)->id);
        stack_push(nodes, ((ast_enumerator_t*)root)->id);
        break;
      case AST_PROG:
        fprintf(stream, ",\"decls\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_prog_t*)root)->decls);
        break;
      default:
        unreachable();
      }

      fputc('}', stream);
    }
  }

  fprintf(stream, "}");

  list_free(discovered);
  stack_free(nodes);
}

const char* ast_kind_to_string(ast_kind_t kind)
{
  switch (kind)
  {
  case AST_UNKNOWN:        return "AST_UNKNOWN";
  case AST_ID:             return "AST_ID";
  case AST_TYPE_MUT:       return "AST_TYPE_MUT";
  case AST_TYPE_CONST:     return "AST_TYPE_CONST";
  case AST_TYPE_PTR:       return "AST_TYPE_PTR";
  case AST_TYPE_ARRAY:     return "AST_TYPE_ARRAY";
  case AST_TYPE_REF:       return "AST_TYPE_REF";
  case AST_TYPE_OPT:       return "AST_TYPE_OPT";
  case AST_TYPE_FUN:       return "AST_TYPE_FUN";
  case AST_TYPE_GEN:       return "AST_TYPE_GEN";
  case AST_TYPE_TYPE:      return "AST_TYPE_TYPE";
  case AST_TYPE_I8:        return "AST_TYPE_I8";
  case AST_TYPE_I16:       return "AST_TYPE_I16";
  case AST_TYPE_I32:       return "AST_TYPE_I32";
  case AST_TYPE_I64:       return "AST_TYPE_I64";
  case AST_TYPE_ISIZE:     return "AST_TYPE_ISIZE";
  case AST_TYPE_U8:        return "AST_TYPE_U8";
  case AST_TYPE_U16:       return "AST_TYPE_U16";
  case AST_TYPE_U32:       return "AST_TYPE_U32";
  case AST_TYPE_U64:       return "AST_TYPE_U64";
  case AST_TYPE_USIZE:     return "AST_TYPE_USIZE";
  case AST_TYPE_F32:       return "AST_TYPE_F32";
  case AST_TYPE_F64:       return "AST_TYPE_F64";
  case AST_TYPE_BOOL:      return "AST_TYPE_BOOL";
  case AST_TYPE_UNIT:      return "AST_TYPE_UNIT";
  case AST_TYPE_MEMBER:    return "AST_TYPE_MEMBER";
  case AST_EXPR_LIT_INT:   return "AST_EXPR_LIT_INT";
  case AST_EXPR_LIT_FLT:   return "AST_EXPR_LIT_FLT";
  case AST_EXPR_LIT_STR:   return "AST_EXPR_LIT_STR";
  case AST_EXPR_LIT_CHAR:  return "AST_EXPR_LIT_CHAR";
  case AST_EXPR_LIT_BOOL:  return "AST_EXPR_LIT_BOOL";
  case AST_EXPR_LIT_NULL:  return "AST_EXPR_LIT_NULL";
  case AST_EXPR_OP:        return "AST_EXPR_OP";
  case AST_STMT_IF:        return "AST_STMT_IF";
  case AST_STMT_FOR:       return "AST_STMT_FOR";
  case AST_STMT_WHILE:     return "AST_STMT_WHILE";
  case AST_STMT_BREAK:     return "AST_STMT_BREAK";
  case AST_STMT_CONTINUE:  return "AST_STMT_CONTINUE";
  case AST_STMT_RETURN:    return "AST_STMT_RETURN";
  case AST_STMT_YIELD:     return "AST_STMT_YIELD";
  case AST_STMT_BLOCK:     return "AST_STMT_BLOCK";
  case AST_STMT_EXPR:      return "AST_STMT_EXPR";
  case AST_DECL_VAR:       return "AST_DECL_VAR";
  case AST_DECL_LOOP_VAR:  return "AST_DECL_LOOP_VAR";
  case AST_DECL_FUN:       return "AST_DECL_FUN";
  case AST_DECL_GEN:       return "AST_DECL_GEN";
  case AST_DECL_STRUCT:    return "AST_DECL_STRUCT";
  case AST_DECL_UNION:     return "AST_DECL_UNION";
  case AST_DECL_ENUM:      return "AST_DECL_ENUM";
  case AST_DECL_MOD:       return "AST_DECL_MOD";
  case AST_DECL_GENERIC:   return "AST_DECL_GENERIC";
  case AST_PARAM:          return "AST_PARAM";
  case AST_PARAM_DEFAULT:  return "AST_PARAM_DEFAULT";
  case AST_PARAM_VARIADIC: return "AST_PARAM_VARIADIC";
  case AST_PARAM_GENERIC:  return "AST_PARAM_GENERIC";
  case AST_ENUMERATOR:     return "AST_ENUMERATOR";
  case AST_PROG:           return "AST_PROG";
  default: unreachable();
  }

  return NULL;
}

bool ast_is_type(ast_node_t* node)
{
  return node->kind & AST_FLAG_TYPE;
}

bool ast_is_expr(ast_node_t* node)
{
  return node->kind & AST_FLAG_EXPR;
}

bool ast_is_stmt(ast_node_t* node)
{
  return node->kind & AST_FLAG_STMT;
}

bool ast_is_decl(ast_node_t* node)
{
  return node->kind & AST_FLAG_DECL;
}

bool ast_is_param(ast_node_t* node)
{
  return node->kind & AST_FLAG_PARAM;
}

typedesc_t* ast_desc_of(ast_node_t* node)
{
  if (ast_is_type(node))
    return ((ast_type_t*)node)->desc;

  if (ast_is_expr(node))
    return ((ast_expr_t*)node)->desc;
  
  if (ast_is_decl(node))
    return ((ast_decl_t*)node)->desc;

  if (ast_is_param(node))
    return ((ast_param_t*)node)->desc;

  if (node->kind == AST_ENUMERATOR)
    return ((ast_enumerator_t*)node)->desc;

  unreachable();

  return NULL;
}
