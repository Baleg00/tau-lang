/**
 * \file ast.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast.h"

#include <string.h>

#include "allocator.h"
#include "memtrace.h"
#include "stack.h"
#include "util.h"
#include "vector.h"

/**
 * \brief Utility macro which expands to fields that all AST nodes must have.
 */
#define AST_NODE_HEADER\
  struct\
  {\
    ast_kind_t kind; /** AST node kind. */\
    token_t* tok; /** Pointer to the token associated with this AST node. */\
  }

/**
 * \brief Utility macro which expands to fields that all typed AST nodes must
 * have.
 */
#define AST_TYPED_HEADER\
  struct\
  {\
    ast_node_t* type; /** Type. */\
  }

/**
 * \brief Utility macro which expands to fields that all operator expression AST
 * nodes must have.
 */
#define AST_EXPR_OP_HEADER\
  struct\
  {\
    op_kind_t op_kind; /** Operation kind. */\
  }

/**
 * \brief Utility macro which expands to fields that all declaration AST nodes
 * must have.
 */
#define AST_DECL_HEADER\
  struct\
  {\
    ast_node_t* id; /** Identifier. */\
  }

/**
 * \brief Utility macro which expands to fields that all composite AST nodes
 * must have.
 */
#define AST_COMPOSITE_HEADER\
  struct\
  {\
    list_t* members; /** Members. */\
  }

/**
 * \brief AST identifier node.
 */
typedef struct ast_id_s ast_id_t;

/**
 * \brief AST type node.
 */
typedef struct ast_type_s ast_type_t;

/**
 * \brief AST mutable type node.
 */
typedef struct ast_type_mut_s ast_type_mut_t;

/**
 * \brief AST constant type node.
 */
typedef struct ast_type_const_s ast_type_const_t;

/**
 * \brief AST pointer type node.
 */
typedef struct ast_type_ptr_s ast_type_ptr_t;

/**
 * \brief AST array type node.
 */
typedef struct ast_type_array_s ast_type_array_t;

/**
 * \brief AST reference type node.
 */
typedef struct ast_type_ref_s ast_type_ref_t;

/**
 * \brief AST optional type node.
 */
typedef struct ast_type_opt_s ast_type_opt_t;

/**
 * \brief AST function type node.
 */
typedef struct ast_type_fun_s ast_type_fun_t;

/**
 * \brief AST generator type node.
 */
typedef struct ast_type_gen_s ast_type_gen_t;

/**
 * \brief AST member type node.
 */
typedef struct ast_type_member_s ast_type_member_t;

/**
 * \brief AST expression node.
 */
typedef struct ast_expr_s ast_expr_t;

/**
 * \brief AST literal expression node.
 */
typedef struct ast_expr_lit_s ast_expr_lit_t;

/**
 * \brief AST operation expression node.
 */
typedef struct ast_expr_op_s ast_expr_op_t;

/**
 * \brief AST unary operation expression node.
 */
typedef struct ast_expr_op_un_s ast_expr_op_un_t;

/**
 * \brief AST binary operation expression node.
 */
typedef struct ast_expr_op_bin_s ast_expr_op_bin_t;

/**
 * \brief AST call expression node.
 */
typedef struct ast_expr_op_call_s ast_expr_op_call_t;

/**
 * \brief AST statement node.
 */
typedef struct ast_stmt_s ast_stmt_t;

/**
 * \brief AST if-else statement node.
 */
typedef struct ast_stmt_if_s ast_stmt_if_t;

/**
 * \brief AST for-loop statement node.
 */
typedef struct ast_stmt_for_s ast_stmt_for_t;

/**
 * \brief AST while-loop statement node.
 */
typedef struct ast_stmt_while_s ast_stmt_while_t;

/**
 * \brief AST break statement node.
 */
typedef struct ast_stmt_break_s ast_stmt_break_t;

/**
 * \brief AST continue statement node.
 */
typedef struct ast_stmt_continue_s ast_stmt_continue_t;

/**
 * \brief AST return statement node.
 */
typedef struct ast_stmt_return_s ast_stmt_return_t;

/**
 * \brief AST yield statement node.
 */
typedef struct ast_stmt_yield_s ast_stmt_yield_t;

/**
 * \brief AST expression statement node.
 */
typedef struct ast_stmt_expr_s ast_stmt_expr_t;

/**
 * \brief AST block statement node.
 */
typedef struct ast_stmt_block_s ast_stmt_block_t;

/**
 * \brief AST declaration node.
 */
typedef struct ast_decl_s ast_decl_t;

/**
 * \brief AST variable declaration node.
 */
typedef struct ast_decl_var_s ast_decl_var_t;

/**
 * \brief AST function declaration node.
 */
typedef struct ast_decl_fun_s ast_decl_fun_t;

/**
 * \brief AST generator declaration node.
 */
typedef struct ast_decl_gen_s ast_decl_gen_t;

/**
 * \brief AST structure declaration node.
 */
typedef struct ast_decl_struct_s ast_decl_struct_t;

/**
 * \brief AST union declaration node.
 */
typedef struct ast_decl_union_s ast_decl_union_t;

/**
 * \brief AST enumeration declaration node.
 */
typedef struct ast_decl_enum_s ast_decl_enum_t;

/**
 * \brief AST module declaration node.
 */
typedef struct ast_decl_mod_s ast_decl_mod_t;

/**
 * \brief AST parameter declaration node.
 */
typedef struct ast_decl_param_s ast_decl_param_t;

/**
 * \brief AST enumeration constant declaration node.
 */
typedef struct ast_decl_enum_constant_s ast_decl_enum_constant_t;

/**
 * \brief AST program node.
 */
typedef struct ast_prog_s ast_prog_t;

struct ast_node_s
{
  AST_NODE_HEADER;
};

struct ast_id_s
{
  AST_NODE_HEADER;
};

struct ast_type_s
{
  AST_NODE_HEADER;
};

struct ast_type_mut_s
{
  AST_NODE_HEADER;
  ast_node_t* base_type;
};

struct ast_type_const_s
{
  AST_NODE_HEADER;
  ast_node_t* base_type;
};

struct ast_type_ptr_s
{
  AST_NODE_HEADER;
  ast_node_t* base_type;
};

struct ast_type_ref_s
{
  AST_NODE_HEADER;
  ast_node_t* base_type;
};

struct ast_type_opt_s
{
  AST_NODE_HEADER;
  ast_node_t* base_type;
};

struct ast_type_array_s
{
  AST_NODE_HEADER;
  ast_node_t* base_type;
  ast_node_t* size;
};

struct ast_type_fun_s
{
  AST_NODE_HEADER;
  list_t* params;
  ast_node_t* return_type;
};

struct ast_type_gen_s
{
  AST_NODE_HEADER;
  list_t* params;
  ast_node_t* yield_type;
};

struct ast_type_member_s
{
  AST_NODE_HEADER;
  ast_node_t* owner;
  ast_node_t* member;
};

struct ast_expr_s
{
  AST_NODE_HEADER;
};

struct ast_expr_lit_s
{
  AST_NODE_HEADER;
};

struct ast_expr_op_s
{
  AST_NODE_HEADER;
  AST_EXPR_OP_HEADER;
};

struct ast_expr_op_bin_s
{
  AST_NODE_HEADER;
  AST_EXPR_OP_HEADER;
  ast_node_t* lhs;
  ast_node_t* rhs;
};

struct ast_expr_op_un_s
{
  AST_NODE_HEADER;
  AST_EXPR_OP_HEADER;
  ast_node_t* expr;
};

struct ast_expr_op_call_s
{
  AST_NODE_HEADER;
  AST_EXPR_OP_HEADER;
  ast_node_t* callee;
  list_t* params;
};

struct ast_stmt_s
{
  AST_NODE_HEADER;
};

struct ast_stmt_if_s
{
  AST_NODE_HEADER;
  ast_node_t* cond;
  ast_node_t* stmt;
  ast_node_t* stmt_else;
};

struct ast_stmt_for_s
{
  AST_NODE_HEADER;
  ast_node_t* var;
  ast_node_t* range;
  ast_node_t* stmt;
};

struct ast_stmt_while_s
{
  AST_NODE_HEADER;
  ast_node_t* cond;
  ast_node_t* stmt;
};

struct ast_stmt_break_s
{
  AST_NODE_HEADER;
};

struct ast_stmt_continue_s
{
  AST_NODE_HEADER;
};

struct ast_stmt_return_s
{
  AST_NODE_HEADER;
  ast_node_t* expr;
};

struct ast_stmt_yield_s
{
  AST_NODE_HEADER;
  ast_node_t* expr;
};

struct ast_stmt_expr_s
{
  AST_NODE_HEADER;
  ast_node_t* expr;
};

struct ast_stmt_block_s
{
  AST_NODE_HEADER;
  list_t* stmts;
};

struct ast_decl_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
};

struct ast_decl_var_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  AST_TYPED_HEADER;
  ast_node_t* expr;
};

struct ast_decl_fun_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  list_t* params;
  ast_node_t* return_type;
  ast_node_t* stmt;
};

struct ast_decl_gen_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  list_t* params;
  ast_node_t* yield_type;
  ast_node_t* stmt;
};

struct ast_decl_struct_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  AST_COMPOSITE_HEADER;
};

struct ast_decl_union_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  AST_COMPOSITE_HEADER;
};

struct ast_decl_enum_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  AST_COMPOSITE_HEADER;
};

struct ast_decl_mod_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  list_t* decls;
};

struct ast_decl_param_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  AST_TYPED_HEADER;
  ast_node_t* expr;
  bool is_variadic;
};

struct ast_decl_enum_constant_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  AST_TYPED_HEADER;
};

struct ast_prog_s
{
  AST_NODE_HEADER;
  list_t* decls;
};

#undef AST_COMPOSITE_HEADER
#undef AST_DECL_HEADER
#undef AST_EXPR_OP_HEADER
#undef AST_TYPED_HEADER
#undef AST_NODE_HEADER

static vector_t* g_ast_nodes = NULL;

ast_node_t* ast_node_init(ast_kind_t kind)
{
  size_t node_size = sizeof(ast_node_t);

  switch (kind)
  {
  case AST_ID:                  node_size = sizeof(ast_id_t                 ); break;
  case AST_PROG:                node_size = sizeof(ast_prog_t               ); break;
  case AST_TYPE_MUT:            node_size = sizeof(ast_type_mut_t           ); break;
  case AST_TYPE_CONST:          node_size = sizeof(ast_type_const_t         ); break;
  case AST_TYPE_PTR:            node_size = sizeof(ast_type_ptr_t           ); break;
  case AST_TYPE_ARRAY:          node_size = sizeof(ast_type_array_t         ); break;
  case AST_TYPE_REF:            node_size = sizeof(ast_type_ref_t           ); break;
  case AST_TYPE_OPT:            node_size = sizeof(ast_type_opt_t           ); break;
  case AST_TYPE_FUN:            node_size = sizeof(ast_type_fun_t           ); break;
  case AST_TYPE_GEN:            node_size = sizeof(ast_type_gen_t           ); break;
  case AST_TYPE_TYPE:           node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_SELF:           node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_I8:             node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_I16:            node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_I32:            node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_I64:            node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_ISIZE:          node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_U8:             node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_U16:            node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_U32:            node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_U64:            node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_USIZE:          node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_F32:            node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_F64:            node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_BOOL:           node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_UNIT:           node_size = sizeof(ast_node_t               ); break;
  case AST_TYPE_MEMBER:         node_size = sizeof(ast_type_member_t        ); break;
  case AST_EXPR_LIT_INT:        node_size = sizeof(ast_expr_lit_t           ); break;
  case AST_EXPR_LIT_FLT:        node_size = sizeof(ast_expr_lit_t           ); break;
  case AST_EXPR_LIT_STR:        node_size = sizeof(ast_expr_lit_t           ); break;
  case AST_EXPR_LIT_CHAR:       node_size = sizeof(ast_expr_lit_t           ); break;
  case AST_EXPR_LIT_BOOL:       node_size = sizeof(ast_expr_lit_t           ); break;
  case AST_EXPR_LIT_NULL:       node_size = sizeof(ast_expr_lit_t           ); break;
  case AST_EXPR_OP:             node_size = sizeof(ast_expr_op_t            ); break;
  case AST_STMT_IF:             node_size = sizeof(ast_stmt_if_t            ); break;
  case AST_STMT_FOR:            node_size = sizeof(ast_stmt_for_t           ); break;
  case AST_STMT_WHILE:          node_size = sizeof(ast_stmt_while_t         ); break;
  case AST_STMT_BREAK:          node_size = sizeof(ast_stmt_break_t         ); break;
  case AST_STMT_CONTINUE:       node_size = sizeof(ast_stmt_continue_t      ); break;
  case AST_STMT_RETURN:         node_size = sizeof(ast_stmt_return_t        ); break;
  case AST_STMT_YIELD:          node_size = sizeof(ast_stmt_yield_t         ); break;
  case AST_STMT_BLOCK:          node_size = sizeof(ast_stmt_block_t         ); break;
  case AST_STMT_EXPR:           node_size = sizeof(ast_stmt_expr_t          ); break;
  case AST_DECL_VAR:            node_size = sizeof(ast_decl_var_t           ); break;
  case AST_DECL_FUN:            node_size = sizeof(ast_decl_fun_t           ); break;
  case AST_DECL_GEN:            node_size = sizeof(ast_decl_gen_t           ); break;
  case AST_DECL_STRUCT:         node_size = sizeof(ast_decl_struct_t        ); break;
  case AST_DECL_UNION:          node_size = sizeof(ast_decl_union_t         ); break;
  case AST_DECL_ENUM:           node_size = sizeof(ast_decl_enum_t          ); break;
  case AST_DECL_MOD:            node_size = sizeof(ast_decl_mod_t           ); break;
  case AST_DECL_PARAM:          node_size = sizeof(ast_decl_param_t         ); break;
  case AST_DECL_ENUM_CONSTANT:  node_size = sizeof(ast_decl_enum_constant_t ); break;
  default: unreachable();
  }

  ast_node_t* node = (ast_node_t*)allocator_allocate(allocator_global(), node_size);
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
    case AST_PROG:
    case AST_DECL_MOD:
      list_free(ast_get_decls(node));
      break;
    case AST_EXPR_OP:
      if (ast_get_op(node) == OP_CALL)
        list_free(ast_get_params(node));
      break;
    case AST_TYPE_FUN:
    case AST_TYPE_GEN:
    case AST_DECL_FUN:
    case AST_DECL_GEN:
      list_free(ast_get_params(node));
      break;
    case AST_STMT_BLOCK:
      list_free(ast_get_stmts(node));
      break;
    case AST_DECL_STRUCT:
    case AST_DECL_UNION:
    case AST_DECL_ENUM:
      list_free(ast_get_members(node));
      break;
    default: fallthrough();
    }

    allocator_deallocate(allocator_global(), node);
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
  case AST_ID:
    fprintf(stream, ",\"id\":\"%.*s\"", (int)location_get_len(token_get_loc(root->tok)), location_get_ptr(token_get_loc(root->tok)));
    break;
  case AST_TYPE_MUT:
  case AST_TYPE_CONST:
  case AST_TYPE_PTR:
  case AST_TYPE_REF:
  case AST_TYPE_OPT:
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, ast_get_base_type(root));
    break;
  case AST_TYPE_ARRAY:
    fprintf(stream, ",\"size\":");
    ast_json_dump(stream, ast_get_size(root));
    fprintf(stream, ",\"base_type\":");
    ast_json_dump(stream, ast_get_base_type(root));
    break;
  case AST_TYPE_FUN:
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ast_get_params(root));
    fprintf(stream, ",\"return_type\":");
    ast_json_dump(stream, ast_get_return_type(root));
    break;
  case AST_TYPE_GEN:
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ast_get_params(root));
    fprintf(stream, ",\"yield_type\":");
    ast_json_dump(stream, ast_get_yield_type(root));
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
    ast_json_dump(stream, ast_get_owner(root));
    fprintf(stream, ",\"member\":");
    ast_json_dump(stream, ast_get_member(root));
    break;
  case AST_EXPR_LIT_INT:
  case AST_EXPR_LIT_FLT:
  case AST_EXPR_LIT_STR:
  case AST_EXPR_LIT_CHAR:
  case AST_EXPR_LIT_BOOL:
  case AST_EXPR_LIT_NULL:
    fprintf(stream, ",\"value\":\"%.*s\"", (int)location_get_len(token_get_loc(root->tok)), location_get_ptr(token_get_loc(root->tok)));
    break;
  case AST_EXPR_OP:
    fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_string(ast_get_op(root)));
    if (op_is_unary(ast_get_op(root)))
    {
      fprintf(stream, ",\"expr\":");
      ast_json_dump(stream, ast_get_expr(root));
    }
    else if (op_is_binary(ast_get_op(root)))
    {
      fprintf(stream, ",\"lhs\":");
      ast_json_dump(stream, ast_get_lhs(root));
      fprintf(stream, ",\"rhs\":");
      ast_json_dump(stream, ast_get_rhs(root));
    }
    else if (ast_get_op(root) == OP_CALL)
    {
      fprintf(stream, ",\"callee\":");
      ast_json_dump(stream, ast_get_callee(root));
      fprintf(stream, ",\"params\":");
      ast_json_dump_list(stream, ast_get_params(root));
    }
    else
      unreachable();
    break;
  case AST_STMT_IF:
    fprintf(stream, ",\"cond\":");
    ast_json_dump(stream, ast_get_cond(root));
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ast_get_stmt(root));
    fprintf(stream, ",\"stmt_else\":");
    ast_json_dump(stream, ast_get_stmt_else(root));
    break;
  case AST_STMT_FOR:
    fprintf(stream, ",\"var\":");
    ast_json_dump(stream, ast_get_var(root));
    fprintf(stream, ",\"range\":");
    ast_json_dump(stream, ast_get_range(root));
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ast_get_stmt(root));
    break;
  case AST_STMT_WHILE:
    fprintf(stream, ",\"cond\":");
    ast_json_dump(stream, ast_get_cond(root));
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ast_get_stmt(root));
    break;
  case AST_STMT_BREAK:
  case AST_STMT_CONTINUE:
    break;
  case AST_STMT_RETURN:
  case AST_STMT_YIELD:
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ast_get_expr(root));
    break;
  case AST_STMT_BLOCK:
    fprintf(stream, ",\"stmts\":");
    ast_json_dump_list(stream, ast_get_stmts(root));
    break;
  case AST_STMT_EXPR:
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ast_get_expr(root));
    break;
  case AST_DECL_VAR:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ast_get_id(root));
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, ast_get_type(root));
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ast_get_expr(root));
    break;
  case AST_DECL_FUN:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ast_get_id(root));
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ast_get_params(root));
    fprintf(stream, ",\"return_type\":");
    ast_json_dump(stream, ast_get_return_type(root));
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ast_get_stmt(root));
    break;
  case AST_DECL_GEN:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ast_get_id(root));
    fprintf(stream, ",\"params\":");
    ast_json_dump_list(stream, ast_get_params(root));
    fprintf(stream, ",\"yield_type\":");
    ast_json_dump(stream, ast_get_yield_type(root));
    fprintf(stream, ",\"stmt\":");
    ast_json_dump(stream, ast_get_stmt(root));
    break;
  case AST_DECL_STRUCT:
  case AST_DECL_UNION:
  case AST_DECL_ENUM:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ast_get_id(root));
    fprintf(stream, ",\"members\":");
    ast_json_dump_list(stream, ast_get_members(root));
    break;
  case AST_DECL_MOD:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ast_get_id(root));
    fprintf(stream, ",\"decls\":");
    ast_json_dump_list(stream, ast_get_decls(root));
    break;
  case AST_DECL_PARAM:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ast_get_id(root));
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, ast_get_type(root));
    fprintf(stream, ",\"expr\":");
    ast_json_dump(stream, ast_get_expr(root));
    fprintf(stream, ",\"is_variadic\":%s", ast_is_variadic(root) ? "true" : "false");
    break;
  case AST_DECL_ENUM_CONSTANT:
    fprintf(stream, ",\"id\":");
    ast_json_dump(stream, ast_get_id(root));
    fprintf(stream, ",\"type\":");
    ast_json_dump(stream, ast_get_type(root));
    break;
  case AST_PROG:
    fprintf(stream, ",\"decls\":");
    ast_json_dump_list(stream, ast_get_decls(root));
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

      fprintf(stream, "\"%p\":{\"kind\":\"%s\"", (void*)root, ast_kind_to_string(root->kind));

      switch (root->kind)
      {
      case AST_ID:
        fprintf(stream, ",\"id\":\"%.*s\"", (int)location_get_len(token_get_loc(root->tok)), location_get_ptr(token_get_loc(root->tok)));
        break;
      case AST_TYPE_MUT:
      case AST_TYPE_CONST:
      case AST_TYPE_PTR:
      case AST_TYPE_REF:
      case AST_TYPE_OPT:
        fprintf(stream, ",\"base_type\":\"%p\"", (void*)ast_get_base_type(root));
        stack_push(nodes, ast_get_base_type(root));
        break;
      case AST_TYPE_ARRAY:
        fprintf(stream, ",\"size\":\"%p\"", (void*)ast_get_size(root));
        stack_push(nodes, ast_get_size(root));
        fprintf(stream, ",\"base_type\":\"%p\"", (void*)ast_get_base_type(root));
        stack_push(nodes, ast_get_base_type(root));
        break;
      case AST_TYPE_FUN:
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ast_get_params(root));
        fprintf(stream, ",\"return_type\":\"%p\"", (void*)ast_get_return_type(root));
        stack_push(nodes, ast_get_return_type(root));
        break;
      case AST_TYPE_GEN:
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ast_get_params(root));
        fprintf(stream, ",\"yield_type\":\"%p\"", (void*)ast_get_yield_type(root));
        stack_push(nodes, ast_get_yield_type(root));
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
        fprintf(stream, ",\"owner\":\"%p\"", (void*)ast_get_owner(root));
        stack_push(nodes, ast_get_owner(root));
        fprintf(stream, ",\"member\":\"%p\"", (void*)ast_get_member(root));
        stack_push(nodes, ast_get_member(root));
        break;
      case AST_EXPR_LIT_INT:
      case AST_EXPR_LIT_FLT:
      case AST_EXPR_LIT_STR:
      case AST_EXPR_LIT_CHAR:
      case AST_EXPR_LIT_BOOL:
      case AST_EXPR_LIT_NULL:
        fprintf(stream, ",\"value\":\"%.*s\"", (int)location_get_len(token_get_loc(root->tok)), location_get_ptr(token_get_loc(root->tok)));
        break;
      case AST_EXPR_OP:
        fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_string(ast_get_op(root)));
        if (op_is_unary(ast_get_op(root)))
        {
          fprintf(stream, ",\"param\":\"%p\"", (void*)ast_get_expr(root));
          stack_push(nodes, ast_get_expr(root));
        }
        else if (op_is_binary(ast_get_op(root)))
        {
          fprintf(stream, ",\"lhs\":\"%p\"", (void*)ast_get_lhs(root));
          stack_push(nodes, ast_get_lhs(root));
          fprintf(stream, ",\"rhs\":\"%p\"", (void*)ast_get_rhs(root));
          stack_push(nodes, ast_get_rhs(root));
        }
        else if (ast_get_op(root) == OP_CALL)
        {
          fprintf(stream, ",\"callee\":\"%p\"", (void*)ast_get_callee(root));
          stack_push(nodes, ast_get_callee(root));
          fprintf(stream, ",\"params\":");
          ast_json_dump_flat_list(stream, nodes, ast_get_params(root));
        }
        break;
      case AST_STMT_IF:
        fprintf(stream, ",\"cond\":\"%p\"", (void*)ast_get_cond(root));
        stack_push(nodes, ast_get_cond(root));
        fprintf(stream, ",\"stmt\":\"%p\"", (void*)ast_get_stmt(root));
        stack_push(nodes, ast_get_stmt(root));
        fprintf(stream, ",\"stmt_else\":\"%p\"", (void*)ast_get_stmt_else(root));
        stack_push(nodes, ast_get_stmt_else(root));
        break;
      case AST_STMT_FOR:
        fprintf(stream, ",\"var\":\"%p\"", (void*)ast_get_var(root));
        stack_push(nodes, ast_get_var(root));
        fprintf(stream, ",\"range\":\"%p\"", (void*)ast_get_range(root));
        stack_push(nodes, ast_get_range(root));
        fprintf(stream, ",\"stmt\":\"%p\"", (void*)ast_get_stmt(root));
        stack_push(nodes, ast_get_stmt(root));
        break;
      case AST_STMT_WHILE:
        fprintf(stream, ",\"cond\":\"%p\"", (void*)ast_get_cond(root));
        stack_push(nodes, ast_get_cond(root));
        fprintf(stream, ",\"stmt\":\"%p\"", (void*)ast_get_stmt(root));
        stack_push(nodes, ast_get_stmt(root));
        break;
      case AST_STMT_BREAK:
      case AST_STMT_CONTINUE:
        break;
      case AST_STMT_RETURN:
      case AST_STMT_YIELD:
        fprintf(stream, ",\"expr\":\"%p\"", (void*)ast_get_expr(root));
        stack_push(nodes, ast_get_expr(root));
        break;
      case AST_STMT_BLOCK:
        fprintf(stream, ",\"stmts\":");
        ast_json_dump_flat_list(stream, nodes, ast_get_stmts(root));
        break;
      case AST_STMT_EXPR:
        fprintf(stream, ",\"expr\":\"%p\"", (void*)ast_get_expr(root));
        stack_push(nodes, ast_get_expr(root));
        break;
      case AST_DECL_VAR:
        fprintf(stream, ",\"id\":\"%p\"", (void*)ast_get_id(root));
        stack_push(nodes, ast_get_id(root));
        fprintf(stream, ",\"type\":\"%p\"", (void*)ast_get_type(root));
        stack_push(nodes, ast_get_type(root));
        fprintf(stream, ",\"expr\":\"%p\"", (void*)ast_get_expr(root));
        stack_push(nodes, ast_get_expr(root));
        break;
      case AST_DECL_FUN:
        fprintf(stream, ",\"id\":\"%p\"", (void*)ast_get_id(root));
        stack_push(nodes, ast_get_id(root));
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ast_get_params(root));
        fprintf(stream, ",\"return_type\":\"%p\"", (void*)ast_get_return_type(root));
        stack_push(nodes, ast_get_return_type(root));
        fprintf(stream, ",\"stmt\":\"%p\"", (void*)ast_get_stmt(root));
        stack_push(nodes, ast_get_stmt(root));
        break;
      case AST_DECL_GEN:
        fprintf(stream, ",\"id\":\"%p\"", (void*)ast_get_id(root));
        stack_push(nodes, ast_get_id(root));
        fprintf(stream, ",\"params\":");
        ast_json_dump_flat_list(stream, nodes, ast_get_params(root));
        fprintf(stream, ",\"yield_type\":\"%p\"", (void*)ast_get_yield_type(root));
        stack_push(nodes, ast_get_yield_type(root));
        fprintf(stream, ",\"stmt\":\"%p\"", (void*)ast_get_stmt(root));
        stack_push(nodes, ast_get_stmt(root));
        break;
      case AST_DECL_STRUCT:
      case AST_DECL_UNION:
      case AST_DECL_ENUM:
        fprintf(stream, ",\"id\":\"%p\"", (void*)ast_get_id(root));
        stack_push(nodes, ast_get_id(root));
        fprintf(stream, ",\"members\":");
        ast_json_dump_flat_list(stream, nodes, ast_get_members(root));
        break;
      case AST_DECL_MOD:
        fprintf(stream, ",\"id\":\"%p\"", (void*)ast_get_id(root));
        stack_push(nodes, ast_get_id(root));
        fprintf(stream, ",\"decls\":");
        ast_json_dump_flat_list(stream, nodes, ast_get_decls(root));
        break;
      case AST_DECL_PARAM:
        fprintf(stream, ",\"id\":\"%p\"", (void*)ast_get_id(root));
        stack_push(nodes, ast_get_id(root));
        fprintf(stream, ",\"type\":\"%p\"", (void*)ast_get_type(root));
        stack_push(nodes, ast_get_type(root));
        fprintf(stream, ",\"expr\":\"%p\"", (void*)ast_get_expr(root));
        stack_push(nodes, ast_get_expr(root));
        fprintf(stream, ",\"is_variadic\":%s", ast_is_variadic(root) ? "true" : "false");
        break;
      case AST_DECL_ENUM_CONSTANT:
        fprintf(stream, ",\"id\":\"%p\"", (void*)ast_get_id(root));
        stack_push(nodes, ast_get_id(root));
        fprintf(stream, ",\"type\":\"%p\"", (void*)ast_get_type(root));
        stack_push(nodes, ast_get_type(root));
        break;
      case AST_PROG:
        fprintf(stream, ",\"decls\":");
        ast_json_dump_flat_list(stream, nodes, ast_get_decls(root));
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
  case AST_ID:                  return "AST_ID";
  case AST_TYPE_MUT:            return "AST_TYPE_MUT";
  case AST_TYPE_CONST:          return "AST_TYPE_CONST";
  case AST_TYPE_PTR:            return "AST_TYPE_PTR";
  case AST_TYPE_ARRAY:          return "AST_TYPE_ARRAY";
  case AST_TYPE_REF:            return "AST_TYPE_REF";
  case AST_TYPE_OPT:            return "AST_TYPE_OPT";
  case AST_TYPE_FUN:            return "AST_TYPE_FUN";
  case AST_TYPE_GEN:            return "AST_TYPE_GEN";
  case AST_TYPE_TYPE:           return "AST_TYPE_TYPE";
  case AST_TYPE_I8:             return "AST_TYPE_I8";
  case AST_TYPE_I16:            return "AST_TYPE_I16";
  case AST_TYPE_I32:            return "AST_TYPE_I32";
  case AST_TYPE_I64:            return "AST_TYPE_I64";
  case AST_TYPE_ISIZE:          return "AST_TYPE_ISIZE";
  case AST_TYPE_U8:             return "AST_TYPE_U8";
  case AST_TYPE_U16:            return "AST_TYPE_U16";
  case AST_TYPE_U32:            return "AST_TYPE_U32";
  case AST_TYPE_U64:            return "AST_TYPE_U64";
  case AST_TYPE_USIZE:          return "AST_TYPE_USIZE";
  case AST_TYPE_F32:            return "AST_TYPE_F32";
  case AST_TYPE_F64:            return "AST_TYPE_F64";
  case AST_TYPE_BOOL:           return "AST_TYPE_BOOL";
  case AST_TYPE_UNIT:           return "AST_TYPE_UNIT";
  case AST_TYPE_MEMBER:         return "AST_TYPE_MEMBER";
  case AST_EXPR_LIT_INT:        return "AST_EXPR_LIT_INT";
  case AST_EXPR_LIT_FLT:        return "AST_EXPR_LIT_FLT";
  case AST_EXPR_LIT_STR:        return "AST_EXPR_LIT_STR";
  case AST_EXPR_LIT_CHAR:       return "AST_EXPR_LIT_CHAR";
  case AST_EXPR_LIT_BOOL:       return "AST_EXPR_LIT_BOOL";
  case AST_EXPR_LIT_NULL:       return "AST_EXPR_LIT_NULL";
  case AST_EXPR_OP:             return "AST_EXPR_OP";
  case AST_STMT_IF:             return "AST_STMT_IF";
  case AST_STMT_FOR:            return "AST_STMT_FOR";
  case AST_STMT_WHILE:          return "AST_STMT_WHILE";
  case AST_STMT_BREAK:          return "AST_STMT_BREAK";
  case AST_STMT_CONTINUE:       return "AST_STMT_CONTINUE";
  case AST_STMT_RETURN:         return "AST_STMT_RETURN";
  case AST_STMT_YIELD:          return "AST_STMT_YIELD";
  case AST_STMT_BLOCK:          return "AST_STMT_BLOCK";
  case AST_STMT_EXPR:           return "AST_STMT_EXPR";
  case AST_DECL_VAR:            return "AST_DECL_VAR";
  case AST_DECL_FUN:            return "AST_DECL_FUN";
  case AST_DECL_GEN:            return "AST_DECL_GEN";
  case AST_DECL_STRUCT:         return "AST_DECL_STRUCT";
  case AST_DECL_UNION:          return "AST_DECL_UNION";
  case AST_DECL_ENUM:           return "AST_DECL_ENUM";
  case AST_DECL_MOD:            return "AST_DECL_MOD";
  case AST_DECL_PARAM:          return "AST_DECL_PARAM";
  case AST_DECL_ENUM_CONSTANT:  return "AST_DECL_ENUM_CONSTANT";
  case AST_PROG:                return "AST_PROG";
  default: unreachable();
  }

  return NULL;
}

ast_kind_t ast_get_kind(ast_node_t* node)
{
  return node->kind;
}

void ast_set_kind(ast_node_t* node, ast_kind_t kind)
{
  node->kind = kind;
}

token_t* ast_get_token(ast_node_t* node)
{
  return node->tok;
}

void ast_set_token(ast_node_t* node, token_t* tok)
{
  node->tok = tok;
}

op_kind_t ast_get_op(ast_node_t* node)
{
  assert(node->kind == AST_EXPR_OP);
  return ((ast_expr_op_t*)node)->op_kind;
}

void ast_set_op(ast_node_t* node, op_kind_t op)
{
  assert(node->kind == AST_EXPR_OP);
  ((ast_expr_op_t*)node)->op_kind = op;
}

ast_node_t* ast_get_id(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:           return ((ast_decl_var_t*)          node)->id;
  case AST_DECL_FUN:           return ((ast_decl_fun_t*)          node)->id;
  case AST_DECL_GEN:           return ((ast_decl_gen_t*)          node)->id;
  case AST_DECL_STRUCT:        return ((ast_decl_struct_t*)       node)->id;
  case AST_DECL_UNION:         return ((ast_decl_union_t*)        node)->id;
  case AST_DECL_ENUM:          return ((ast_decl_enum_t*)         node)->id;
  case AST_DECL_MOD:           return ((ast_decl_mod_t*)          node)->id;
  case AST_DECL_PARAM:         return ((ast_decl_param_t*)        node)->id;
  case AST_DECL_ENUM_CONSTANT: return ((ast_decl_enum_constant_t*)node)->id;
  default: unreachable();
  }

  return NULL;
}

void ast_set_id(ast_node_t* node, ast_node_t* id)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:           ((ast_decl_var_t*)          node)->id = id; break;
  case AST_DECL_FUN:           ((ast_decl_fun_t*)          node)->id = id; break;
  case AST_DECL_GEN:           ((ast_decl_gen_t*)          node)->id = id; break;
  case AST_DECL_STRUCT:        ((ast_decl_struct_t*)       node)->id = id; break;
  case AST_DECL_UNION:         ((ast_decl_union_t*)        node)->id = id; break;
  case AST_DECL_ENUM:          ((ast_decl_enum_t*)         node)->id = id; break;
  case AST_DECL_MOD:           ((ast_decl_mod_t*)          node)->id = id; break;
  case AST_DECL_PARAM:         ((ast_decl_param_t*)        node)->id = id; break;
  case AST_DECL_ENUM_CONSTANT: ((ast_decl_enum_constant_t*)node)->id = id; break;
  default: unreachable();
  }
}

ast_node_t* ast_get_type(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:            return ((ast_decl_var_t*           )node)->type;
  case AST_DECL_PARAM:          return ((ast_decl_param_t*         )node)->type;
  case AST_DECL_ENUM_CONSTANT:  return ((ast_decl_enum_constant_t* )node)->type;
  default: unreachable();
  }

  return NULL;
}

void ast_set_type(ast_node_t* node, ast_node_t* type)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:            ((ast_decl_var_t*           )node)->type = type; break;
  case AST_DECL_PARAM:          ((ast_decl_param_t*         )node)->type = type; break;
  case AST_DECL_ENUM_CONSTANT:  ((ast_decl_enum_constant_t* )node)->type = type; break;
  default: unreachable();
  }
}

ast_node_t* ast_get_base_type(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_TYPE_MUT:   return ((ast_type_mut_t*  )node)->base_type;
  case AST_TYPE_CONST: return ((ast_type_const_t*)node)->base_type;
  case AST_TYPE_PTR:   return ((ast_type_ptr_t*  )node)->base_type;
  case AST_TYPE_ARRAY: return ((ast_type_array_t*)node)->base_type;
  case AST_TYPE_REF:   return ((ast_type_ref_t*  )node)->base_type;
  case AST_TYPE_OPT:   return ((ast_type_opt_t*  )node)->base_type;
  default: unreachable();
  }

  return NULL;
}

void ast_set_base_type(ast_node_t* node, ast_node_t* base_type)
{
  switch (node->kind)
  {
  case AST_TYPE_MUT:   ((ast_type_mut_t*  )node)->base_type = base_type; break;
  case AST_TYPE_CONST: ((ast_type_const_t*)node)->base_type = base_type; break;
  case AST_TYPE_PTR:   ((ast_type_ptr_t*  )node)->base_type = base_type; break;
  case AST_TYPE_ARRAY: ((ast_type_array_t*)node)->base_type = base_type; break;
  case AST_TYPE_REF:   ((ast_type_ref_t*  )node)->base_type = base_type; break;
  case AST_TYPE_OPT:   ((ast_type_opt_t*  )node)->base_type = base_type; break;
  default: unreachable();
  }
}

ast_node_t* ast_get_size(ast_node_t* node)
{
  assert(node->kind == AST_TYPE_ARRAY);
  return ((ast_type_array_t*)node)->size;
}

void ast_set_size(ast_node_t* node, ast_node_t* size)
{
  assert(node->kind == AST_TYPE_ARRAY);
  ((ast_type_array_t*)node)->size = size;
}

list_t* ast_get_params(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_TYPE_FUN: return ((ast_type_fun_t*)node)->params;
  case AST_TYPE_GEN: return ((ast_type_gen_t*)node)->params;
  case AST_DECL_FUN: return ((ast_decl_fun_t*)node)->params;
  case AST_DECL_GEN: return ((ast_decl_gen_t*)node)->params;
  default: unreachable();
  }

  return NULL;
}

void ast_set_params(ast_node_t* node, list_t* params)
{
  switch (node->kind)
  {
  case AST_TYPE_FUN: ((ast_type_fun_t*)node)->params = params; break;
  case AST_TYPE_GEN: ((ast_type_gen_t*)node)->params = params; break;
  case AST_DECL_FUN: ((ast_decl_fun_t*)node)->params = params; break;
  case AST_DECL_GEN: ((ast_decl_gen_t*)node)->params = params; break;
  default: unreachable();
  }
}

ast_node_t* ast_get_return_type(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_TYPE_FUN: return ((ast_type_fun_t*)node)->return_type;
  case AST_DECL_FUN: return ((ast_decl_fun_t*)node)->return_type;
  default: unreachable();
  }

  return NULL;
}

void ast_set_return_type(ast_node_t* node, ast_node_t* return_type)
{
  switch (node->kind)
  {
  case AST_TYPE_FUN: ((ast_type_fun_t*)node)->return_type = return_type; break;
  case AST_DECL_FUN: ((ast_decl_fun_t*)node)->return_type = return_type; break;
  default: unreachable();
  }
}

ast_node_t* ast_get_yield_type(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_TYPE_GEN: return ((ast_type_gen_t*)node)->yield_type;
  case AST_DECL_GEN: return ((ast_decl_gen_t*)node)->yield_type;
  default: unreachable();
  }

  return NULL;
}

void ast_set_yield_type(ast_node_t* node, ast_node_t* yield_type)
{
  switch (node->kind)
  {
  case AST_TYPE_GEN: ((ast_type_gen_t*)node)->yield_type = yield_type; break;
  case AST_DECL_GEN: ((ast_decl_gen_t*)node)->yield_type = yield_type; break;
  default: unreachable();
  }
}

ast_node_t* ast_get_owner(ast_node_t* node)
{
  assert(node->kind == AST_TYPE_MEMBER);
  return ((ast_type_member_t*)node)->owner;
}

void ast_set_owner(ast_node_t* node, ast_node_t* owner)
{
  assert(node->kind == AST_TYPE_MEMBER);
  ((ast_type_member_t*)node)->owner = owner;
}

ast_node_t* ast_get_member(ast_node_t* node)
{
  assert(node->kind == AST_TYPE_MEMBER);
  return ((ast_type_member_t*)node)->member;
}

void ast_set_member(ast_node_t* node, ast_node_t* member)
{
  assert(node->kind == AST_TYPE_MEMBER);
  ((ast_type_member_t*)node)->member = member;
}

ast_node_t* ast_get_lhs(ast_node_t* node)
{
  assert(node->kind == AST_EXPR_OP);
  assert(op_is_binary(ast_get_op(node)));
  return ((ast_expr_op_bin_t*)node)->lhs;
}

void ast_set_lhs(ast_node_t* node, ast_node_t* lhs)
{
  assert(node->kind == AST_EXPR_OP);
  assert(op_is_binary(ast_get_op(node)));
  ((ast_expr_op_bin_t*)node)->lhs = lhs;
}

ast_node_t* ast_get_rhs(ast_node_t* node)
{
  assert(node->kind == AST_EXPR_OP);
  assert(op_is_binary(ast_get_op(node)));
  return ((ast_expr_op_bin_t*)node)->rhs;
}

void ast_set_rhs(ast_node_t* node, ast_node_t* rhs)
{
  assert(node->kind == AST_EXPR_OP);
  assert(op_is_binary(ast_get_op(node)));
  ((ast_expr_op_bin_t*)node)->rhs = rhs;
}

ast_node_t* ast_get_callee(ast_node_t* node)
{
  assert(node->kind == AST_EXPR_OP);
  assert(ast_get_op(node) == OP_CALL);
  return ((ast_expr_op_call_t*)node)->callee;
}

void ast_set_callee(ast_node_t* node, ast_node_t* callee)
{
  assert(node->kind == AST_EXPR_OP);
  assert(ast_get_op(node) == OP_CALL);
  ((ast_expr_op_call_t*)node)->callee = callee;
}

ast_node_t* ast_get_cond(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_STMT_IF:    return ((ast_stmt_if_t*   )node)->cond;
  case AST_STMT_WHILE: return ((ast_stmt_while_t*)node)->cond;
  default: unreachable();
  }

  return NULL;
}

void ast_set_cond(ast_node_t* node, ast_node_t* cond)
{
  switch (node->kind)
  {
  case AST_STMT_IF:    ((ast_stmt_if_t*   )node)->cond = cond; break;
  case AST_STMT_WHILE: ((ast_stmt_while_t*)node)->cond = cond; break;
  default: unreachable();
  }
}

ast_node_t* ast_get_stmt(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_STMT_IF:    return ((ast_stmt_if_t*   )node)->stmt;
  case AST_STMT_FOR:   return ((ast_stmt_for_t*  )node)->stmt;
  case AST_STMT_WHILE: return ((ast_stmt_while_t*)node)->stmt;
  case AST_DECL_FUN:   return ((ast_decl_fun_t*  )node)->stmt;
  case AST_DECL_GEN:   return ((ast_decl_gen_t*  )node)->stmt;
  default: unreachable();
  }

  return NULL;
}

void ast_set_stmt(ast_node_t* node, ast_node_t* stmt)
{
  switch (node->kind)
  {
  case AST_STMT_IF:    ((ast_stmt_if_t*   )node)->stmt = stmt; break;
  case AST_STMT_FOR:   ((ast_stmt_for_t*  )node)->stmt = stmt; break;
  case AST_STMT_WHILE: ((ast_stmt_while_t*)node)->stmt = stmt; break;
  case AST_DECL_FUN:   ((ast_decl_fun_t*  )node)->stmt = stmt; break;
  case AST_DECL_GEN:   ((ast_decl_gen_t*  )node)->stmt = stmt; break;
  default: unreachable();
  }
}

ast_node_t* ast_get_stmt_else(ast_node_t* node)
{
  assert(node->kind == AST_STMT_IF);
  return ((ast_stmt_if_t*)node)->stmt_else;
}

void ast_set_stmt_else(ast_node_t* node, ast_node_t* stmt_else)
{
  assert(node->kind == AST_STMT_IF);
  ((ast_stmt_if_t*)node)->stmt_else = stmt_else;
}

ast_node_t* ast_get_var(ast_node_t* node)
{
  assert(node->kind == AST_STMT_FOR);
  return ((ast_stmt_for_t*)node)->var;
}

void ast_set_var(ast_node_t* node, ast_node_t* var)
{
  assert(node->kind == AST_STMT_FOR);
  ((ast_stmt_for_t*)node)->var = var;
}

ast_node_t* ast_get_range(ast_node_t* node)
{
  assert(node->kind == AST_STMT_FOR);
  return ((ast_stmt_for_t*)node)->range;
}

void ast_set_range(ast_node_t* node, ast_node_t* range)
{
  assert(node->kind == AST_STMT_FOR);
  ((ast_stmt_for_t*)node)->range = range;
}

ast_node_t* ast_get_expr(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_EXPR_OP:
    assert(op_is_unary(ast_get_op(node)));
    return ((ast_expr_op_un_t*)node)->expr;
  case AST_STMT_RETURN: return ((ast_stmt_return_t*)node)->expr;
  case AST_STMT_YIELD:  return ((ast_stmt_yield_t* )node)->expr;
  case AST_STMT_EXPR:   return ((ast_stmt_expr_t*  )node)->expr;
  case AST_DECL_VAR:    return ((ast_decl_var_t*   )node)->expr;
  case AST_DECL_PARAM:  return ((ast_decl_param_t* )node)->expr;
  default: unreachable();
  }

  return NULL;
}

void ast_set_expr(ast_node_t* node, ast_node_t* expr)
{
  switch (node->kind)
  {
  case AST_EXPR_OP:
    assert(op_is_unary(ast_get_op(node)));
    ((ast_expr_op_un_t*)node)->expr = expr;
    break;
  case AST_STMT_RETURN: ((ast_stmt_return_t*)node)->expr = expr; break;
  case AST_STMT_YIELD:  ((ast_stmt_yield_t* )node)->expr = expr; break;
  case AST_STMT_EXPR:   ((ast_stmt_expr_t*  )node)->expr = expr; break;
  case AST_DECL_VAR:    ((ast_decl_var_t*   )node)->expr = expr; break;
  case AST_DECL_PARAM:  ((ast_decl_param_t* )node)->expr = expr; break;
  default: unreachable();
  }
}

list_t* ast_get_stmts(ast_node_t* node)
{
  assert(node->kind == AST_STMT_BLOCK);
  return ((ast_stmt_block_t*)node)->stmts;
}

void ast_set_stmts(ast_node_t* node, list_t* stmts)
{
  assert(node->kind == AST_STMT_BLOCK);
  ((ast_stmt_block_t*)node)->stmts = stmts;
}

list_t* ast_get_members(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_STRUCT: return ((ast_decl_struct_t*)node)->members;
  case AST_DECL_UNION:  return ((ast_decl_union_t* )node)->members;
  case AST_DECL_ENUM:   return ((ast_decl_enum_t*  )node)->members;
  default: unreachable();
  }

  return NULL;
}

void ast_set_members(ast_node_t* node, list_t* members)
{
  switch (node->kind)
  {
  case AST_DECL_STRUCT: ((ast_decl_struct_t*)node)->members = members; break;
  case AST_DECL_UNION:  ((ast_decl_union_t* )node)->members = members; break;
  case AST_DECL_ENUM:   ((ast_decl_enum_t*  )node)->members = members; break;
  default: unreachable();
  }
}

list_t* ast_get_decls(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_MOD: return ((ast_decl_mod_t*)node)->decls;
  case AST_PROG:     return ((ast_prog_t*    )node)->decls;
  default: unreachable();
  }

  return NULL;
}

void ast_set_decls(ast_node_t* node, list_t* decls)
{
  switch (node->kind)
  {
  case AST_DECL_MOD: ((ast_decl_mod_t*)node)->decls = decls; break;
  case AST_PROG:     ((ast_prog_t*    )node)->decls = decls; break;
  default: unreachable();
  }
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
  case AST_EXPR_OP:
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

bool ast_is_variadic(ast_node_t* node)
{
  assert(node->kind == AST_DECL_PARAM);
  return ((ast_decl_param_t*)node)->is_variadic;
}

void ast_set_variadic(ast_node_t* node, bool is_variadic)
{
  assert(node->kind == AST_DECL_PARAM);
  ((ast_decl_param_t*)node)->is_variadic = is_variadic;
}
