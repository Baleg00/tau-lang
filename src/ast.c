/**
 * \file ast.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast.h"

#include <string.h>

#include "memtrace.h"
#include "stack.h"
#include "util.h"
#include "vector.h"

static vector_t* g_ast_nodes = NULL;

ast_node_t* ast_node_init(ast_kind_t kind)
{
  size_t node_size = sizeof(ast_node_t);

  switch (kind)
  {
  case AST_ID:                  node_size = sizeof(ast_id_t                ); break;
  case AST_TYPE_MUT:            node_size = sizeof(ast_type_mut_t          ); break;
  case AST_TYPE_CONST:          node_size = sizeof(ast_type_const_t        ); break;
  case AST_TYPE_PTR:            node_size = sizeof(ast_type_ptr_t          ); break;
  case AST_TYPE_ARRAY:          node_size = sizeof(ast_type_array_t        ); break;
  case AST_TYPE_REF:            node_size = sizeof(ast_type_ref_t          ); break;
  case AST_TYPE_OPT:            node_size = sizeof(ast_type_opt_t          ); break;
  case AST_TYPE_FUN:            node_size = sizeof(ast_type_fun_t          ); break;
  case AST_TYPE_GEN:            node_size = sizeof(ast_type_gen_t          ); break;
  case AST_TYPE_TYPE:           node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_SELF:           node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_I8:             node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_I16:            node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_I32:            node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_I64:            node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_ISIZE:          node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_U8:             node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_U16:            node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_U32:            node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_U64:            node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_USIZE:          node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_F32:            node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_F64:            node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_BOOL:           node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_UNIT:           node_size = sizeof(ast_type_t              ); break;
  case AST_TYPE_MEMBER:         node_size = sizeof(ast_type_member_t       ); break;
  case AST_TYPE_DECL:           node_size = sizeof(ast_type_decl_t         ); break;
  case AST_EXPR_LIT_INT:        node_size = sizeof(ast_expr_lit_t          ); break;
  case AST_EXPR_LIT_FLT:        node_size = sizeof(ast_expr_lit_t          ); break;
  case AST_EXPR_LIT_STR:        node_size = sizeof(ast_expr_lit_t          ); break;
  case AST_EXPR_LIT_CHAR:       node_size = sizeof(ast_expr_lit_t          ); break;
  case AST_EXPR_LIT_BOOL:       node_size = sizeof(ast_expr_lit_t          ); break;
  case AST_EXPR_LIT_NULL:       node_size = sizeof(ast_expr_lit_t          ); break;
  case AST_EXPR_OP_UNARY:       node_size = sizeof(ast_expr_op_un_t        ); break;
  case AST_EXPR_OP_BINARY:      node_size = sizeof(ast_expr_op_bin_t       ); break;
  case AST_EXPR_OP_CALL:        node_size = sizeof(ast_expr_op_call_t      ); break;
  case AST_EXPR_DECL:           node_size = sizeof(ast_expr_decl_t         ); break;
  case AST_STMT_IF:             node_size = sizeof(ast_stmt_if_t           ); break;
  case AST_STMT_FOR:            node_size = sizeof(ast_stmt_for_t          ); break;
  case AST_STMT_WHILE:          node_size = sizeof(ast_stmt_while_t        ); break;
  case AST_STMT_BREAK:          node_size = sizeof(ast_stmt_break_t        ); break;
  case AST_STMT_CONTINUE:       node_size = sizeof(ast_stmt_continue_t     ); break;
  case AST_STMT_RETURN:         node_size = sizeof(ast_stmt_return_t       ); break;
  case AST_STMT_YIELD:          node_size = sizeof(ast_stmt_yield_t        ); break;
  case AST_STMT_DEFER:          node_size = sizeof(ast_stmt_defer_t        ); break;
  case AST_STMT_BLOCK:          node_size = sizeof(ast_stmt_block_t        ); break;
  case AST_STMT_EXPR:           node_size = sizeof(ast_stmt_expr_t         ); break;
  case AST_DECL_VAR:            node_size = sizeof(ast_decl_var_t          ); break;
  case AST_DECL_FUN:            node_size = sizeof(ast_decl_fun_t          ); break;
  case AST_DECL_GEN:            node_size = sizeof(ast_decl_gen_t          ); break;
  case AST_DECL_STRUCT:         node_size = sizeof(ast_decl_struct_t       ); break;
  case AST_DECL_UNION:          node_size = sizeof(ast_decl_union_t        ); break;
  case AST_DECL_ENUM:           node_size = sizeof(ast_decl_enum_t         ); break;
  case AST_DECL_MOD:            node_size = sizeof(ast_decl_mod_t          ); break;
  case AST_DECL_PARAM:          node_size = sizeof(ast_decl_param_t        ); break;
  case AST_DECL_ENUM_CONSTANT:  node_size = sizeof(ast_decl_enum_constant_t); break;
  case AST_ATTR:                node_size = sizeof(ast_attr_t              ); break;
  case AST_PROG:                node_size = sizeof(ast_prog_t              ); break;
  default: unreachable();
  }

  ast_node_t* node = (ast_node_t*)malloc(node_size);
  assert(node != NULL);

  memset(node, 0, node_size);

  node->kind = kind;
  node->tok = NULL;

  if (g_ast_nodes == NULL)
    g_ast_nodes = vector_init();

  vector_push(g_ast_nodes, node);

  return node;
}

void ast_cleanup(void)
{
  if (g_ast_nodes == NULL)
    return;

  for (size_t i = 0; i < vector_size(g_ast_nodes); i++)
  {
    ast_node_t* node = (ast_node_t*)vector_get(g_ast_nodes, i);

    switch (node->kind)
    {
    case AST_TYPE_FUN:
      list_free(((ast_type_fun_t*)node)->params);
      break;
    case AST_TYPE_GEN:
      list_free(((ast_type_gen_t*)node)->params);
      break;
    case AST_EXPR_OP_CALL:
      list_free(((ast_expr_op_call_t*)node)->params);
      break;
    case AST_STMT_BLOCK:
      list_free(((ast_stmt_block_t*)node)->stmts);
      break;
    case AST_DECL_VAR:
      list_free(((ast_decl_var_t*)node)->attrs);
      break;
    case AST_DECL_PARAM:
      list_free(((ast_decl_param_t*)node)->attrs);
      break;
    case AST_DECL_FUN:
      list_free(((ast_decl_fun_t*)node)->attrs);
      list_free(((ast_decl_fun_t*)node)->params);
      break;
    case AST_DECL_GEN:
      list_free(((ast_decl_gen_t*)node)->attrs);
      list_free(((ast_decl_gen_t*)node)->params);
      break;
    case AST_DECL_STRUCT:
      list_free(((ast_decl_struct_t*)node)->attrs);
      list_free(((ast_decl_struct_t*)node)->members);
      break;
    case AST_DECL_UNION:
      list_free(((ast_decl_union_t*)node)->attrs);
      list_free(((ast_decl_union_t*)node)->members);
      break;
    case AST_DECL_ENUM:
      list_free(((ast_decl_enum_t*)node)->attrs);
      list_free(((ast_decl_enum_t*)node)->members);
      break;
    case AST_DECL_ENUM_CONSTANT:
      list_free(((ast_decl_enum_constant_t*)node)->attrs);
      break;
    case AST_DECL_MOD:
      list_free(((ast_decl_mod_t*)node)->attrs);
      list_free(((ast_decl_mod_t*)node)->members);
      break;
    case AST_ATTR:
      list_free(((ast_attr_t*)node)->params);
      break;
    case AST_PROG:
      list_free(((ast_prog_t*)node)->decls);
      break;
    default:
      noop();
    }

    free(node);
  }

  vector_free(g_ast_nodes);
  g_ast_nodes = NULL;
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

void ast_json_dump(FILE* stream, ast_node_t* node)
{
  if (node == NULL)
  {
    fprintf(stream, "null");
    return;
  }

  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));

  switch (node->kind)
  {
  case AST_ID:
    fprintf(stream, ",\"id\":\"%.*s\"", (int)node->tok->loc->len, node->tok->loc->ptr);
    break;
  case AST_TYPE_MUT:
  case AST_TYPE_CONST:
  case AST_TYPE_PTR:
  case AST_TYPE_REF:
  case AST_TYPE_OPT:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, ((ast_type_modifier_t*)node)->base_type);
    break;
  case AST_TYPE_ARRAY:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, ((ast_type_array_t*)node)->base_type);
    fprintf(stream, ",\"size\":");
    ast_json_dump(stream, ((ast_type_array_t*)node)->size);
    break;
  case AST_TYPE_FUN:
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ((ast_type_fun_t*)node)->params);
    fprintf(stream, ",\"return_type\":");
    ast_json_dump(stream, ((ast_type_fun_t*)node)->return_type);
    fprintf(stream, ",\"is_async\":%s", ((ast_type_fun_t*)node)->is_async ? "true" : "false");
    fprintf(stream, ",\"is_vararg\":%s", ((ast_type_fun_t*)node)->is_vararg ? "true" : "false");
    fprintf(stream, ",\"abi\":\"%s\"", abi_kind_to_cstr(((ast_type_fun_t*)node)->abi));
    break;
  case AST_TYPE_GEN:
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ((ast_type_gen_t*)node)->params);
    fprintf(stream, ",\"yield_type\":");
    ast_json_dump(stream, ((ast_type_gen_t*)node)->yield_type);
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
    ast_json_dump(stream, ((ast_type_member_t*)node)->owner);
    fprintf(stream, ",\"member\":");
    ast_json_dump(stream, ((ast_type_member_t*)node)->member);
    break;
  case AST_TYPE_DECL:
    fprintf(stream, ",\"decl\":");
    ast_json_dump(stream, ((ast_type_decl_t*)node)->decl);
    break;
  case AST_EXPR_LIT_INT:
  case AST_EXPR_LIT_FLT:
  case AST_EXPR_LIT_STR:
  case AST_EXPR_LIT_CHAR:
  case AST_EXPR_LIT_BOOL:
  case AST_EXPR_LIT_NULL:
    fprintf(stream, ",\"value\":\"%.*s\"", (int)node->tok->loc->len, node->tok->loc->ptr);
    break;
  case AST_EXPR_OP_UNARY:
    fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_string(((ast_expr_op_un_t*)node)->op_kind));
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ((ast_expr_op_un_t*)node)->expr);
    break;
  case AST_EXPR_OP_BINARY:
    fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_string(((ast_expr_op_bin_t*)node)->op_kind));
    fprintf(stream, ",\"lhs\":");
    ast_json_dump(stream, ((ast_expr_op_bin_t*)node)->lhs);
    fprintf(stream, ",\"rhs\":");
    ast_json_dump(stream, ((ast_expr_op_bin_t*)node)->rhs);
    break;
  case AST_EXPR_OP_CALL:
    fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_string(AST_EXPR_OP_CALL));
    fprintf(stream, ",\"callee\":");
    ast_json_dump(stream, ((ast_expr_op_call_t*)node)->callee);
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ((ast_expr_op_call_t*)node)->params);
    break;
  case AST_EXPR_DECL:
    fprintf(stream, ",\"decl\":");
    ast_json_dump(stream, ((ast_expr_decl_t*)node)->decl);
    break;
  case AST_STMT_IF:
    fprintf(stream, ",\"cond\":");
    ast_json_dump(stream, ((ast_stmt_if_t*)node)->cond);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ((ast_stmt_if_t*)node)->stmt);
    fprintf(stream, ",\"stmt_else\":");
    ast_json_dump(stream, ((ast_stmt_if_t*)node)->stmt_else);
    break;
  case AST_STMT_FOR:
    fprintf(stream, ",\"var\":");
    ast_json_dump(stream, ((ast_stmt_for_t*)node)->var);
    fprintf(stream, ",\"range\":");
    ast_json_dump(stream, ((ast_stmt_for_t*)node)->range);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ((ast_stmt_for_t*)node)->stmt);
    break;
  case AST_STMT_WHILE:
    fprintf(stream, ",\"cond\":");
    ast_json_dump(stream, ((ast_stmt_while_t*)node)->cond);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ((ast_stmt_while_t*)node)->stmt);
    break;
  case AST_STMT_BREAK:
    fprintf(stream, ",\"loop\":");
    ast_json_dump(stream, ((ast_stmt_break_t*)node)->loop);
    break;
  case AST_STMT_CONTINUE:
    fprintf(stream, ",\"loop\":");
    ast_json_dump(stream, ((ast_stmt_continue_t*)node)->loop);
    break;
  case AST_STMT_RETURN:
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ((ast_stmt_return_t*)node)->expr);
    break;
  case AST_STMT_YIELD:
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ((ast_stmt_yield_t*)node)->expr);
    break;
  case AST_STMT_DEFER:
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ((ast_stmt_defer_t*)node)->stmt);
    break;
  case AST_STMT_BLOCK:
    fprintf(stream, ",\"stmts\":");
    ast_json_dump_list(stream, ((ast_stmt_block_t*)node)->stmts);
    break;
  case AST_STMT_EXPR:
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ((ast_stmt_expr_t*)node)->expr);
    break;
  case AST_DECL_VAR:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_var_t*)node)->id);
    fprintf(stream, ",\"is_pub\":%s", ((ast_decl_var_t*)node)->is_pub ? "true" : "false");
    fprintf(stream, ",\"attrs\":");
    ast_json_dump_list(stream, ((ast_decl_var_t*)node)->attrs);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, ((ast_decl_var_t*)node)->type);
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ((ast_decl_var_t*)node)->expr);
    break;
  case AST_DECL_PARAM:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_param_t*)node)->id);
    fprintf(stream, ",\"attrs\":");
    ast_json_dump_list(stream, ((ast_decl_param_t*)node)->attrs);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, ((ast_decl_param_t*)node)->type);
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ((ast_decl_param_t*)node)->expr);
    fprintf(stream, ",\"is_variadic\":%s", ((ast_decl_param_t*)node)->is_variadic ? "true" : "false");
    break;
  case AST_DECL_FUN:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_fun_t*)node)->id);
    fprintf(stream, ",\"is_pub\":%s", ((ast_decl_fun_t*)node)->is_pub ? "true" : "false");
    fprintf(stream, ",\"is_async\":%s", ((ast_decl_fun_t*)node)->is_async ? "true" : "false");
    fprintf(stream, ",\"is_extern\":%s", ((ast_decl_fun_t*)node)->is_extern ? "true" : "false");
    fprintf(stream, ",\"is_vararg\":%s", ((ast_decl_fun_t*)node)->is_vararg ? "true" : "false");
    fprintf(stream, ",\"abi\":\"%s\"", abi_kind_to_cstr(((ast_decl_fun_t*)node)->abi));
    fprintf(stream, ",\"attrs\":");
    ast_json_dump_list(stream, ((ast_decl_fun_t*)node)->attrs);
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ((ast_decl_fun_t*)node)->params);
    fprintf(stream, ",\"return_type\":");
    ast_json_dump(stream, ((ast_decl_fun_t*)node)->return_type);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ((ast_decl_fun_t*)node)->stmt);
    break;
  case AST_DECL_GEN:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_gen_t*)node)->id);
    fprintf(stream, ",\"is_pub\":%s", ((ast_decl_gen_t*)node)->is_pub ? "true" : "false");
    fprintf(stream, ",\"attrs\":");
    ast_json_dump_list(stream, ((ast_decl_gen_t*)node)->attrs);
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ((ast_decl_gen_t*)node)->params);
    fprintf(stream, ",\"yield_type\":");
    ast_json_dump(stream, ((ast_decl_gen_t*)node)->yield_type);
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ((ast_decl_gen_t*)node)->stmt);
    break;
  case AST_DECL_STRUCT:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_struct_t*)node)->id);
    fprintf(stream, ",\"is_pub\":%s", ((ast_decl_struct_t*)node)->is_pub ? "true" : "false");
    fprintf(stream, ",\"attrs\":");
    ast_json_dump_list(stream, ((ast_decl_struct_t*)node)->attrs);
    fprintf(stream, ",\"members\":");
    ast_json_dump_list(stream, ((ast_decl_struct_t*)node)->members);
    break;
  case AST_DECL_UNION:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_union_t*)node)->id);
    fprintf(stream, ",\"is_pub\":%s", ((ast_decl_union_t*)node)->is_pub ? "true" : "false");
    fprintf(stream, ",\"attrs\":");
    ast_json_dump_list(stream, ((ast_decl_union_t*)node)->attrs);
    fprintf(stream, ",\"members\":");
    ast_json_dump_list(stream, ((ast_decl_union_t*)node)->members);
    break;
  case AST_DECL_ENUM:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_enum_t*)node)->id);
    fprintf(stream, ",\"is_pub\":%s", ((ast_decl_enum_t*)node)->is_pub ? "true" : "false");
    fprintf(stream, ",\"attrs\":");
    ast_json_dump_list(stream, ((ast_decl_enum_t*)node)->attrs);
    fprintf(stream, ",\"members\":");
    ast_json_dump_list(stream, ((ast_decl_enum_t*)node)->members);
    break;
  case AST_DECL_ENUM_CONSTANT:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_enum_constant_t*)node)->id);
    fprintf(stream, ",\"attrs\":");
    ast_json_dump_list(stream, ((ast_decl_enum_constant_t*)node)->attrs);
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, ((ast_decl_enum_constant_t*)node)->type);
    break;
  case AST_DECL_MOD:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_decl_mod_t*)node)->id);
    fprintf(stream, ",\"is_pub\":%s", ((ast_decl_mod_t*)node)->is_pub ? "true" : "false");
    fprintf(stream, ",\"attrs\":");
    ast_json_dump_list(stream, ((ast_decl_mod_t*)node)->attrs);
    fprintf(stream, ",\"members\":");
    ast_json_dump_list(stream, ((ast_decl_mod_t*)node)->members);
    break;
  case AST_ATTR:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ((ast_attr_t*)node)->id);
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ((ast_attr_t*)node)->params);
    break;
  case AST_PROG:
    fprintf(stream, ",\"decls\":");
    ast_json_dump_list(stream, ((ast_prog_t*)node)->decls);
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

    fprintf(stream, "\"%p\"", (void*)node);
    
    if (list_node_next(it) != NULL)
      fputc(',', stream);
  }
  fputc(']', stream);
}

void ast_json_dump_flat(FILE* stream, ast_node_t* node)
{
  stack_t* nodes = stack_init();
  list_t* discovered = list_init();

  stack_push(nodes, node);

  fprintf(stream, "{");

  bool is_first = true;

  while (!stack_empty(nodes))
  {
    node = (ast_node_t*)stack_pop(nodes);

    if (node == NULL)
      continue;

    bool is_discovered = false;

    LIST_FOR_LOOP(it, discovered)
      if (node == (ast_node_t*)list_node_get(it))
      {
        is_discovered = true;
        break;
      }

    if (!is_discovered)
    {
      list_push_back(discovered, node);

      if (!is_first)
        fputc(',', stream);
      else
        is_first = false;

      fprintf(stream, "\"%p\":{\"kind\":\"%s\"", (void*)node, ast_kind_to_cstr(node->kind));

      switch (node->kind)
      {
      case AST_ID:
        fprintf(stream, ",\"id\":\"%.*s\"", (int)node->tok->loc->len, node->tok->loc->ptr);
        break;
      case AST_TYPE_MUT:
      case AST_TYPE_CONST:
      case AST_TYPE_PTR:
      case AST_TYPE_REF:
      case AST_TYPE_OPT:
        fprintf(stream, ",\"base_type\":\"%p\"", ((ast_type_modifier_t*)node)->base_type);
        stack_push(nodes, ((ast_type_modifier_t*)node)->base_type);
        break;
      case AST_TYPE_ARRAY:
        fprintf(stream, ",\"base_type\":\"%p\"", ((ast_type_array_t*)node)->base_type);
        stack_push(nodes, ((ast_type_array_t*)node)->base_type);
        fprintf(stream, ",\"size\":\"%p\"", ((ast_type_array_t*)node)->size);
        stack_push(nodes, ((ast_type_array_t*)node)->size);
        break;
      case AST_TYPE_FUN:
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_type_fun_t*)node)->params);
        fprintf(stream, ",\"return_type\":\"%p\"", ((ast_type_fun_t*)node)->return_type);
        fprintf(stream, ",\"is_async\":%s", ((ast_type_fun_t*)node)->is_async ? "true" : "false");
        fprintf(stream, ",\"is_vararg\":%s", ((ast_type_fun_t*)node)->is_vararg ? "true" : "false");
        fprintf(stream, ",\"abi\":\"%s\"", abi_kind_to_cstr(((ast_type_fun_t*)node)->abi));
        stack_push(nodes, ((ast_type_fun_t*)node)->return_type);
        break;
      case AST_TYPE_GEN:
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_type_gen_t*)node)->params);
        fprintf(stream, ",\"yield_type\":\"%p\"", ((ast_type_gen_t*)node)->yield_type);
        stack_push(nodes, ((ast_type_gen_t*)node)->yield_type);
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
        fprintf(stream, ",\"owner\":\"%p\"", ((ast_type_member_t*)node)->owner);
        stack_push(nodes, ((ast_type_member_t*)node)->owner);
        fprintf(stream, ",\"member\":\"%p\"", ((ast_type_member_t*)node)->member);
        stack_push(nodes, ((ast_type_member_t*)node)->member);
        break;
      case AST_TYPE_DECL:
        fprintf(stream, ",\"decl\":\"%p\"", ((ast_type_decl_t*)node)->decl);
        stack_push(nodes, ((ast_type_decl_t*)node)->decl);
        break;
      case AST_EXPR_LIT_INT:
      case AST_EXPR_LIT_FLT:
      case AST_EXPR_LIT_STR:
      case AST_EXPR_LIT_CHAR:
      case AST_EXPR_LIT_BOOL:
      case AST_EXPR_LIT_NULL:
        fprintf(stream, ",\"value\":\"%.*s\"", (int)node->tok->loc->len, node->tok->loc->ptr);
        break;
      case AST_EXPR_OP_UNARY:
        fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_string(((ast_expr_op_un_t*)node)->op_kind));
        fprintf(stream, ",\"expr\":\"%p\"", ((ast_expr_op_un_t*)node)->expr);
        stack_push(nodes, ((ast_expr_op_un_t*)node)->expr);
        break;
      case AST_EXPR_OP_BINARY:
        fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_string(((ast_expr_op_bin_t*)node)->op_kind));
        fprintf(stream, ",\"lhs\":\"%p\"", ((ast_expr_op_bin_t*)node)->lhs);
        stack_push(nodes, ((ast_expr_op_bin_t*)node)->lhs);
        fprintf(stream, ",\"rhs\":\"%p\"", ((ast_expr_op_bin_t*)node)->rhs);
        stack_push(nodes, ((ast_expr_op_bin_t*)node)->rhs);
        break;
      case AST_EXPR_OP_CALL:
        fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_string(AST_EXPR_OP_CALL));
        fprintf(stream, ",\"callee\":\"%p\"", ((ast_expr_op_call_t*)node)->callee);
        stack_push(nodes, ((ast_expr_op_call_t*)node)->callee);
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_expr_op_call_t*)node)->params);
        break;
      case AST_EXPR_DECL:
        fprintf(stream, ",\"decl\":\"%p\"", ((ast_expr_decl_t*)node)->decl);
        stack_push(nodes, ((ast_expr_decl_t*)node)->decl);
        break;
      case AST_STMT_IF:
        fprintf(stream, ",\"cond\":\"%p\"", ((ast_stmt_if_t*)node)->cond);
        stack_push(nodes, ((ast_stmt_if_t*)node)->cond);
        fprintf(stream, ",\"stmt\":\"%p\"", ((ast_stmt_if_t*)node)->stmt);
        stack_push(nodes, ((ast_stmt_if_t*)node)->stmt);
        fprintf(stream, ",\"stmt_else\":\"%p\"", ((ast_stmt_if_t*)node)->stmt_else);
        stack_push(nodes, ((ast_stmt_if_t*)node)->stmt_else);
        break;
      case AST_STMT_FOR:
        fprintf(stream, ",\"var\":\"%p\"", ((ast_stmt_for_t*)node)->var);
        stack_push(nodes, ((ast_stmt_for_t*)node)->var);
        fprintf(stream, ",\"range\":\"%p\"", ((ast_stmt_for_t*)node)->range);
        stack_push(nodes, ((ast_stmt_for_t*)node)->range);
        fprintf(stream, ",\"stmt\":\"%p\"", ((ast_stmt_for_t*)node)->stmt);
        stack_push(nodes, ((ast_stmt_for_t*)node)->stmt);
        break;
      case AST_STMT_WHILE:
        fprintf(stream, ",\"cond\":\"%p\"", ((ast_stmt_while_t*)node)->cond);
        stack_push(nodes, ((ast_stmt_while_t*)node)->cond);
        fprintf(stream, ",\"stmt\":\"%p\"", ((ast_stmt_while_t*)node)->stmt);
        stack_push(nodes, ((ast_stmt_while_t*)node)->stmt);
        break;
      case AST_STMT_BREAK:
        fprintf(stream, ",\"loop\":\"%p\"", ((ast_stmt_break_t*)node)->loop);
        stack_push(nodes, ((ast_stmt_break_t*)node)->loop);
        break;
      case AST_STMT_CONTINUE:
        fprintf(stream, ",\"loop\":\"%p\"", ((ast_stmt_continue_t*)node)->loop);
        stack_push(nodes, ((ast_stmt_continue_t*)node)->loop);
        break;
      case AST_STMT_RETURN:
        fprintf(stream, ",\"expr\":\"%p\"", ((ast_stmt_return_t*)node)->expr);
        stack_push(nodes, ((ast_stmt_return_t*)node)->expr);
        break;
      case AST_STMT_YIELD:
        fprintf(stream, ",\"expr\":\"%p\"", ((ast_stmt_yield_t*)node)->expr);
        stack_push(nodes, ((ast_stmt_yield_t*)node)->expr);
        break;
      case AST_STMT_DEFER:
        fprintf(stream, ",\"stmt\":\"%p\"", ((ast_stmt_defer_t*)node)->stmt);
        stack_push(nodes, ((ast_stmt_defer_t*)node)->stmt);
        break;
      case AST_STMT_BLOCK:
        fprintf(stream, ",\"stmts\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_stmt_block_t*)node)->stmts);
        break;
      case AST_STMT_EXPR:
        fprintf(stream, ",\"expr\":\"%p\"", ((ast_stmt_expr_t*)node)->expr);
        stack_push(nodes, ((ast_stmt_expr_t*)node)->expr);
        break;
      case AST_DECL_VAR:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_var_t*)node)->id);
        stack_push(nodes, ((ast_decl_var_t*)node)->id);
        fprintf(stream, ",\"is_pub\":%s", ((ast_decl_var_t*)node)->is_pub ? "true" : "false");
        fprintf(stream, ",\"attrs\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_var_t*)node)->attrs);
        fprintf(stream, ",\"type\":\"%p\"", ((ast_decl_var_t*)node)->type);
        stack_push(nodes, ((ast_decl_var_t*)node)->type);
        fprintf(stream, ",\"expr\":\"%p\"", ((ast_decl_var_t*)node)->expr);
        stack_push(nodes, ((ast_decl_var_t*)node)->expr);
        break;
      case AST_DECL_PARAM:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_param_t*)node)->id);
        stack_push(nodes, ((ast_decl_param_t*)node)->id);
        fprintf(stream, ",\"attrs\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_param_t*)node)->attrs);
        fprintf(stream, ",\"type\":\"%p\"", ((ast_decl_param_t*)node)->type);
        stack_push(nodes, ((ast_decl_param_t*)node)->type);
        fprintf(stream, ",\"expr\":\"%p\"", ((ast_decl_param_t*)node)->expr);
        stack_push(nodes, ((ast_decl_param_t*)node)->expr);
        fprintf(stream, ",\"is_variadic\":%s", ((ast_decl_param_t*)node)->is_variadic ? "true" : "false");
        break;
      case AST_DECL_FUN:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_fun_t*)node)->id);
        stack_push(nodes, ((ast_decl_fun_t*)node)->id);
        fprintf(stream, ",\"is_pub\":%s", ((ast_decl_fun_t*)node)->is_pub ? "true" : "false");
        fprintf(stream, ",\"is_async\":%s", ((ast_decl_fun_t*)node)->is_async ? "true" : "false");
        fprintf(stream, ",\"is_extern\":%s", ((ast_decl_fun_t*)node)->is_extern ? "true" : "false");
        fprintf(stream, ",\"is_vararg\":%s", ((ast_decl_fun_t*)node)->is_vararg ? "true" : "false");
        fprintf(stream, ",\"abi\":\"%s\"", abi_kind_to_cstr(((ast_decl_fun_t*)node)->abi));
        fprintf(stream, ",\"attrs\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_fun_t*)node)->attrs);
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_fun_t*)node)->params);
        fprintf(stream, ",\"return_type\":\"%p\"", ((ast_decl_fun_t*)node)->return_type);
        stack_push(nodes, ((ast_decl_fun_t*)node)->return_type);
        fprintf(stream, ",\"stmt\":\"%p\"", ((ast_decl_fun_t*)node)->stmt);
        stack_push(nodes, ((ast_decl_fun_t*)node)->stmt);
        break;
      case AST_DECL_GEN:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_gen_t*)node)->id);
        stack_push(nodes, ((ast_decl_gen_t*)node)->id);
        fprintf(stream, ",\"is_pub\":%s", ((ast_decl_gen_t*)node)->is_pub ? "true" : "false");
        fprintf(stream, ",\"attrs\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_gen_t*)node)->attrs);
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_gen_t*)node)->params);
        fprintf(stream, ",\"yield_type\":\"%p\"", ((ast_decl_gen_t*)node)->yield_type);
        stack_push(nodes, ((ast_decl_gen_t*)node)->yield_type);
        fprintf(stream, ",\"stmt\":\"%p\"", ((ast_decl_gen_t*)node)->stmt);
        stack_push(nodes, ((ast_decl_gen_t*)node)->stmt);
        break;
      case AST_DECL_STRUCT:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_struct_t*)node)->id);
        stack_push(nodes, ((ast_decl_struct_t*)node)->id);
        fprintf(stream, ",\"is_pub\":%s", ((ast_decl_struct_t*)node)->is_pub ? "true" : "false");
        fprintf(stream, ",\"attrs\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_struct_t*)node)->attrs);
        fprintf(stream, ",\"members\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_struct_t*)node)->members);
        break;
      case AST_DECL_UNION:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_union_t*)node)->id);
        stack_push(nodes, ((ast_decl_union_t*)node)->id);
        fprintf(stream, ",\"is_pub\":%s", ((ast_decl_union_t*)node)->is_pub ? "true" : "false");
        fprintf(stream, ",\"attrs\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_union_t*)node)->attrs);
        fprintf(stream, ",\"members\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_union_t*)node)->members);
        break;
      case AST_DECL_ENUM:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_enum_t*)node)->id);
        stack_push(nodes, ((ast_decl_enum_t*)node)->id);
        fprintf(stream, ",\"is_pub\":%s", ((ast_decl_enum_t*)node)->is_pub ? "true" : "false");
        fprintf(stream, ",\"attrs\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_enum_t*)node)->attrs);
        fprintf(stream, ",\"members\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_enum_t*)node)->members);
        break;
      case AST_DECL_ENUM_CONSTANT:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_enum_constant_t*)node)->id);
        stack_push(nodes, ((ast_decl_enum_constant_t*)node)->id);
        fprintf(stream, ",\"attrs\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_enum_constant_t*)node)->attrs);
        fprintf(stream, ",\"type\":\"%p\"", ((ast_decl_enum_constant_t*)node)->type);
        stack_push(nodes, ((ast_decl_enum_constant_t*)node)->type);
        break;
      case AST_DECL_MOD:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_decl_mod_t*)node)->id);
        stack_push(nodes, ((ast_decl_mod_t*)node)->id);
        fprintf(stream, ",\"is_pub\":%s", ((ast_decl_mod_t*)node)->is_pub ? "true" : "false");
        fprintf(stream, ",\"attrs\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_mod_t*)node)->attrs);
        fprintf(stream, ",\"members\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_decl_mod_t*)node)->members);
        break;
      case AST_ATTR:
        fprintf(stream, ",\"id\":\"%p\"", ((ast_attr_t*)node)->id);
        stack_push(nodes, ((ast_attr_t*)node)->id);
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_attr_t*)node)->params);
        break;
      case AST_PROG:
        fprintf(stream, ",\"decls\":");
        ast_json_dump_flat_list(stream, nodes, ((ast_prog_t*)node)->decls);
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

const char* ast_kind_to_cstr(ast_kind_t kind)
{
  switch (kind)
  {
  case AST_ID:                 return "AST_ID";
  case AST_TYPE_MUT:           return "AST_TYPE_MUT";
  case AST_TYPE_CONST:         return "AST_TYPE_CONST";
  case AST_TYPE_PTR:           return "AST_TYPE_PTR";
  case AST_TYPE_ARRAY:         return "AST_TYPE_ARRAY";
  case AST_TYPE_REF:           return "AST_TYPE_REF";
  case AST_TYPE_OPT:           return "AST_TYPE_OPT";
  case AST_TYPE_FUN:           return "AST_TYPE_FUN";
  case AST_TYPE_GEN:           return "AST_TYPE_GEN";
  case AST_TYPE_TYPE:          return "AST_TYPE_TYPE";
  case AST_TYPE_I8:            return "AST_TYPE_I8";
  case AST_TYPE_I16:           return "AST_TYPE_I16";
  case AST_TYPE_I32:           return "AST_TYPE_I32";
  case AST_TYPE_I64:           return "AST_TYPE_I64";
  case AST_TYPE_ISIZE:         return "AST_TYPE_ISIZE";
  case AST_TYPE_U8:            return "AST_TYPE_U8";
  case AST_TYPE_U16:           return "AST_TYPE_U16";
  case AST_TYPE_U32:           return "AST_TYPE_U32";
  case AST_TYPE_U64:           return "AST_TYPE_U64";
  case AST_TYPE_USIZE:         return "AST_TYPE_USIZE";
  case AST_TYPE_F32:           return "AST_TYPE_F32";
  case AST_TYPE_F64:           return "AST_TYPE_F64";
  case AST_TYPE_BOOL:          return "AST_TYPE_BOOL";
  case AST_TYPE_UNIT:          return "AST_TYPE_UNIT";
  case AST_TYPE_MEMBER:        return "AST_TYPE_MEMBER";
  case AST_TYPE_DECL:          return "AST_TYPE_DECL";
  case AST_EXPR_LIT_INT:       return "AST_EXPR_LIT_INT";
  case AST_EXPR_LIT_FLT:       return "AST_EXPR_LIT_FLT";
  case AST_EXPR_LIT_STR:       return "AST_EXPR_LIT_STR";
  case AST_EXPR_LIT_CHAR:      return "AST_EXPR_LIT_CHAR";
  case AST_EXPR_LIT_BOOL:      return "AST_EXPR_LIT_BOOL";
  case AST_EXPR_LIT_NULL:      return "AST_EXPR_LIT_NULL";
  case AST_EXPR_OP_UNARY:      return "AST_EXPR_OP_UNARY";
  case AST_EXPR_OP_BINARY:     return "AST_EXPR_OP_BINARY";
  case AST_EXPR_OP_CALL:       return "AST_EXPR_OP_CALL";
  case AST_EXPR_DECL:          return "AST_EXPR_DECL";
  case AST_STMT_IF:            return "AST_STMT_IF";
  case AST_STMT_FOR:           return "AST_STMT_FOR";
  case AST_STMT_WHILE:         return "AST_STMT_WHILE";
  case AST_STMT_BREAK:         return "AST_STMT_BREAK";
  case AST_STMT_CONTINUE:      return "AST_STMT_CONTINUE";
  case AST_STMT_RETURN:        return "AST_STMT_RETURN";
  case AST_STMT_YIELD:         return "AST_STMT_YIELD";
  case AST_STMT_DEFER:         return "AST_STMT_DEFER";
  case AST_STMT_BLOCK:         return "AST_STMT_BLOCK";
  case AST_STMT_EXPR:          return "AST_STMT_EXPR";
  case AST_DECL_VAR:           return "AST_DECL_VAR";
  case AST_DECL_FUN:           return "AST_DECL_FUN";
  case AST_DECL_GEN:           return "AST_DECL_GEN";
  case AST_DECL_STRUCT:        return "AST_DECL_STRUCT";
  case AST_DECL_UNION:         return "AST_DECL_UNION";
  case AST_DECL_ENUM:          return "AST_DECL_ENUM";
  case AST_DECL_MOD:           return "AST_DECL_MOD";
  case AST_DECL_PARAM:         return "AST_DECL_PARAM";
  case AST_DECL_ENUM_CONSTANT: return "AST_DECL_ENUM_CONSTANT";
  case AST_ATTR:               return "AST_ATTR";
  case AST_PROG:               return "AST_PROG";
  default: unreachable();
  }

  return NULL;
}

const char* abi_kind_to_cstr(abi_kind_t kind)
{
  switch (kind)
  {
  case ABI_TAU:        return "Tau";
  case ABI_CDECL:      return "cdecl";
  case ABI_STDCALL:    return "stdcall";
  case ABI_WIN64:      return "win64";
  case ABI_SYSV64:     return "sysv64";
  case ABI_AAPCS:      return "aapcs";
  case ABI_FASTCALL:   return "fastcall";
  case ABI_VECTORCALL: return "vectorcall";
  case ABI_THISCALL:   return "thiscall";
  default: unreachable();
  }

  return NULL;
}

bool ast_is_type(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_TYPE_MUT:
  case AST_TYPE_CONST:
  case AST_TYPE_PTR:
  case AST_TYPE_ARRAY:
  case AST_TYPE_REF:
  case AST_TYPE_OPT:
  case AST_TYPE_FUN:
  case AST_TYPE_GEN:
  case AST_TYPE_TYPE:
  case AST_TYPE_SELF:
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
  case AST_TYPE_MEMBER:
  case AST_TYPE_DECL:
    return true;
  default:
    return false;
  }
}

bool ast_is_expr(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_EXPR_LIT_INT:
  case AST_EXPR_LIT_FLT:
  case AST_EXPR_LIT_STR:
  case AST_EXPR_LIT_CHAR:
  case AST_EXPR_LIT_BOOL:
  case AST_EXPR_LIT_NULL:
  case AST_EXPR_OP_UNARY:
  case AST_EXPR_OP_BINARY:
  case AST_EXPR_OP_CALL:
  case AST_EXPR_DECL:
    return true;
  default:
    return false;
  }
}

bool ast_is_stmt(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_STMT_IF:
  case AST_STMT_FOR:
  case AST_STMT_WHILE:
  case AST_STMT_BREAK:
  case AST_STMT_CONTINUE:
  case AST_STMT_RETURN:
  case AST_STMT_YIELD:
  case AST_STMT_DEFER:
  case AST_STMT_BLOCK:
  case AST_STMT_EXPR:
    return true;
  default:
    return false;
  }
}

bool ast_is_decl(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:
  case AST_DECL_PARAM:
  case AST_DECL_FUN:
  case AST_DECL_GEN:
  case AST_DECL_STRUCT:
  case AST_DECL_UNION:
  case AST_DECL_ENUM:
  case AST_DECL_ENUM_CONSTANT:
  case AST_DECL_MOD:
    return true;
  default:
    return false;
  }
}
