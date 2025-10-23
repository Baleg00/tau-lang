/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/access/direct.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_expr_op_bin_access_direct_t* tau_ast_expr_op_bin_access_direct_init(void)
{
  tau_ast_expr_op_bin_access_direct_t* node = (tau_ast_expr_op_bin_access_direct_t*)malloc(sizeof(tau_ast_expr_op_bin_access_direct_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_BINARY;
  node->op_kind = OP_ACCESS_DIRECT;

  return node;
}

void tau_ast_expr_op_bin_access_direct_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_bin_access_direct_t* node)
{
  tau_ast_node_nameres(ctx, node->lhs);
}

void tau_ast_expr_op_bin_access_direct_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_access_direct_t* node)
{
  tau_ast_node_typecheck(ctx, node->lhs);

  TAU_ASSERT(node->rhs->kind == TAU_AST_EXPR_ID); // RHS must be an identifier.

  tau_typedesc_t* lhs_desc = tau_typetable_lookup(ctx->typetable, node->lhs);
  TAU_ASSERT(lhs_desc != NULL);

  if (node->lhs->kind == TAU_AST_EXPR_ID && ((tau_ast_expr_id_t*)node->lhs)->decl->kind == TAU_AST_DECL_ENUM)
  {
    tau_typedesc_enum_t* enum_desc = (tau_typedesc_enum_t*)lhs_desc;

    tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, (tau_typedesc_t*)enum_desc);

    tau_ast_decl_enum_t* enum_node = (tau_ast_decl_enum_t*)enum_desc->node;

    node->decl = (tau_ast_node_t*)enum_node;

    tau_string_view_t id_view = tau_token_to_string_view(node->rhs->tok);
    tau_symbol_t* mbr_sym = tau_symtable_get_with_str_view(enum_node->scope, id_view);

    if (mbr_sym == NULL)
    {
      tau_error_bag_put_typecheck_no_member(ctx->errors, tau_token_location(node->rhs->tok));
      return;
    }

    node->idx = tau_vector_find(enum_node->members, mbr_sym->node);
  }
  else
  {
    TAU_ASSERT(tau_typedesc_is_ref(lhs_desc));

    bool is_mut = tau_typedesc_is_mut(tau_typedesc_remove_ref(lhs_desc));

    tau_typedesc_t* base_desc = tau_typedesc_remove_ref_mut(lhs_desc);

    tau_symtable_t* decl_scope = NULL;
    tau_vector_t* members = NULL;

    switch (base_desc->kind)
    {
    case TAU_TYPEDESC_STRUCT:
    {
      tau_typedesc_struct_t* struct_desc = (tau_typedesc_struct_t*)base_desc;
      tau_ast_decl_struct_t* struct_node = (tau_ast_decl_struct_t*)struct_desc->node;

      node->decl = (tau_ast_node_t*)struct_node;
      decl_scope = struct_node->scope;
      members = struct_node->members;
      break;
    }
    case TAU_TYPEDESC_UNION:
    {
      tau_typedesc_union_t* union_desc = (tau_typedesc_union_t*)base_desc;
      tau_ast_decl_union_t* union_node = (tau_ast_decl_union_t*)union_desc->node;

      node->decl = (tau_ast_node_t*)union_node;
      decl_scope = union_node->scope;
      members = union_node->members;
      break;
    }
    default:
      TAU_UNREACHABLE();
    }

    tau_string_view_t id_view = tau_token_to_string_view(node->rhs->tok);
    tau_symbol_t* mbr_sym = tau_symtable_get_with_str_view(decl_scope, id_view);

    if (mbr_sym == NULL)
    {
      tau_error_bag_put_typecheck_no_member(ctx->errors, tau_token_location(node->rhs->tok));
      tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
      return;
    }

    if (!((tau_ast_decl_t*)mbr_sym->node)->is_pub)
      tau_error_bag_put_typecheck_private_member(ctx->errors, tau_token_location(node->rhs->tok));

    node->idx = tau_vector_find(members, mbr_sym->node);

    tau_typedesc_t* mbr_desc = tau_typetable_lookup(ctx->typetable, mbr_sym->node);
    TAU_ASSERT(lhs_desc != NULL);

    if (is_mut && tau_typedesc_can_add_mut(mbr_desc))
      mbr_desc = tau_typebuilder_build_mut(ctx->typebuilder, mbr_desc);

    if (tau_typedesc_can_add_ref(mbr_desc))
      mbr_desc = tau_typebuilder_build_ref(ctx->typebuilder, mbr_desc);

    tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, mbr_desc);
  }
}

void tau_ast_expr_op_bin_access_direct_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_access_direct_t* node)
{
  tau_ast_node_codegen(ctx, node->lhs);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  TAU_ASSERT(desc != NULL);

  node->llvm_type = desc->llvm_type;

  tau_typedesc_t* decl_desc = tau_typetable_lookup(ctx->typetable, node->decl);
  TAU_ASSERT(decl_desc != NULL);

  LLVMValueRef llvm_lhs_value = ((tau_ast_expr_t*)node->lhs)->llvm_value;

  switch (node->decl->kind)
  {
  case TAU_AST_DECL_STRUCT:
  {
    node->llvm_value = LLVMBuildStructGEP2(ctx->llvm_builder, decl_desc->llvm_type, llvm_lhs_value, (uint32_t)node->idx, "");
    break;
  }
  case TAU_AST_DECL_UNION:
  {
    node->llvm_value = llvm_lhs_value;
    break;
  }
  case TAU_AST_DECL_ENUM:
  {
    node->llvm_value = LLVMConstInt(decl_desc->llvm_type, node->idx, false);
    break;
  }
  default:
    TAU_UNREACHABLE();
  }
}
