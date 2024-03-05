/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/access.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_expr_op_bin_access_t* ast_expr_op_bin_access_init(void)
{
  ast_expr_op_bin_access_t* node = (ast_expr_op_bin_access_t*)malloc(sizeof(ast_expr_op_bin_access_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_BINARY;

  return node;
}

void ast_expr_op_bin_access_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_access_t* node)
{
  ast_node_nameres(ctx, node->lhs);
}

void ast_expr_op_bin_access_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_access_t* node)
{
  ast_node_typecheck(ctx, node->lhs);

  assert(node->rhs->kind == AST_EXPR_ID); // RHS must be an identifier.

  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  assert(lhs_desc != NULL);

  assert(typedesc_remove_const(lhs_desc)->kind == TYPEDESC_REF);

  bool is_mut = typedesc_remove_const_ref(lhs_desc)->kind == TYPEDESC_MUT;

  typedesc_t* base_desc = typedesc_remove_const_ref_mut(lhs_desc);

  assert(base_desc->kind == TYPEDESC_STRUCT); // Only deal with structs for now.

  typedesc_struct_t* struct_desc = (typedesc_struct_t*)base_desc;
  ast_decl_struct_t* struct_node = (ast_decl_struct_t*)struct_desc->node;

  node->decl = (ast_node_t*)struct_node;

  string_view_t id_view = token_to_string_view(node->rhs->tok);
  symbol_t* mbr_sym = symtable_get_with_str_view(struct_node->scope, id_view);

  if (mbr_sym == NULL)
    report_error_no_member_with_name(node->rhs->tok->loc);

  if (!((ast_decl_t*)mbr_sym->node)->is_pub)
    report_error_private_member(node->rhs->tok->loc);

  node->idx = vector_find(struct_node->members, mbr_sym->node);

  typedesc_t* mbr_desc = typetable_lookup(ctx->typetable, mbr_sym->node);
  assert(lhs_desc != NULL);

  if (is_mut && typedesc_can_add_mut(mbr_desc))
    mbr_desc = typebuilder_build_mut(ctx->typebuilder, mbr_desc);

  if (typedesc_can_add_ref(mbr_desc))
    mbr_desc = typebuilder_build_ref(ctx->typebuilder, mbr_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, mbr_desc);
}

void ast_expr_op_bin_access_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_access_t* node)
{
  ast_node_codegen(ctx, node->lhs);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  assert(desc != NULL);

  node->llvm_type = desc->llvm_type;

  typedesc_t* decl_desc = typetable_lookup(ctx->typetable, node->decl);
  assert(decl_desc != NULL);

  node->llvm_value = LLVMBuildStructGEP2(ctx->llvm_builder, decl_desc->llvm_type, ((ast_expr_t*)node->lhs)->llvm_value, (uint32_t)node->idx, "struct_gep2_tmp");
}
