/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/prim.h"

#include "ast/registry.h"

static ast_type_prim_t* ast_type_prim_init(ast_kind_t kind)
{
  ast_type_prim_t* node = (ast_type_prim_t*)malloc(sizeof(ast_type_prim_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = kind;

  return node;
}

ast_type_prim_t* ast_type_prim_i8_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_I8);
}

ast_type_prim_t* ast_type_prim_i16_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_I16);
}

ast_type_prim_t* ast_type_prim_i32_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_I32);
}

ast_type_prim_t* ast_type_prim_i64_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_I64);
}

ast_type_prim_t* ast_type_prim_isize_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_ISIZE);
}

ast_type_prim_t* ast_type_prim_u8_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_U8);
}

ast_type_prim_t* ast_type_prim_u16_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_U16);
}

ast_type_prim_t* ast_type_prim_u32_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_U32);
}

ast_type_prim_t* ast_type_prim_u64_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_U64);
}

ast_type_prim_t* ast_type_prim_usize_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_USIZE);
}

ast_type_prim_t* ast_type_prim_f32_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_F32);
}

ast_type_prim_t* ast_type_prim_f64_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_F64);
}

ast_type_prim_t* ast_type_prim_c64_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_C64);
}

ast_type_prim_t* ast_type_prim_c128_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_C128);
}

ast_type_prim_t* ast_type_prim_char_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_CHAR);
}

ast_type_prim_t* ast_type_prim_bool_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_BOOL);
}

ast_type_prim_t* ast_type_prim_unit_init(void)
{
  return ast_type_prim_init(AST_TYPE_PRIM_UNIT);
}

void ast_type_prim_free(ast_type_prim_t* node)
{
  free(node);
}

void ast_type_prim_nameres(nameres_ctx_t* UNUSED(ctx), ast_type_prim_t* UNUSED(node))
{
}

void ast_type_prim_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = NULL;

  switch (node->kind)
  {
  case AST_TYPE_PRIM_I8:    desc = typebuilder_build_i8   (ctx->typebuilder); break;
  case AST_TYPE_PRIM_I16:   desc = typebuilder_build_i16  (ctx->typebuilder); break;
  case AST_TYPE_PRIM_I32:   desc = typebuilder_build_i32  (ctx->typebuilder); break;
  case AST_TYPE_PRIM_I64:   desc = typebuilder_build_i64  (ctx->typebuilder); break;
  case AST_TYPE_PRIM_ISIZE: desc = typebuilder_build_isize(ctx->typebuilder); break;
  case AST_TYPE_PRIM_U8:    desc = typebuilder_build_u8   (ctx->typebuilder); break;
  case AST_TYPE_PRIM_U16:   desc = typebuilder_build_u16  (ctx->typebuilder); break;
  case AST_TYPE_PRIM_U32:   desc = typebuilder_build_u32  (ctx->typebuilder); break;
  case AST_TYPE_PRIM_U64:   desc = typebuilder_build_u64  (ctx->typebuilder); break;
  case AST_TYPE_PRIM_USIZE: desc = typebuilder_build_usize(ctx->typebuilder); break;
  case AST_TYPE_PRIM_F32:   desc = typebuilder_build_f32  (ctx->typebuilder); break;
  case AST_TYPE_PRIM_F64:   desc = typebuilder_build_f64  (ctx->typebuilder); break;
  case AST_TYPE_PRIM_C64:   desc = typebuilder_build_c64  (ctx->typebuilder); break;
  case AST_TYPE_PRIM_C128:  desc = typebuilder_build_c128 (ctx->typebuilder); break;
  case AST_TYPE_PRIM_CHAR:  desc = typebuilder_build_char (ctx->typebuilder); break;
  case AST_TYPE_PRIM_BOOL:  desc = typebuilder_build_bool (ctx->typebuilder); break;
  case AST_TYPE_PRIM_UNIT:  desc = typebuilder_build_unit (ctx->typebuilder); break;
  default: UNREACHABLE();
  }

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_codegen(codegen_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  ASSERT(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

void ast_type_prim_dump_json(FILE* stream, ast_type_prim_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
