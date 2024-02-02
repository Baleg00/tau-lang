/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/prim.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

static ast_type_prim_t* ast_type_prim_init(ast_kind_t kind)
{
  ast_type_prim_t* node = (ast_type_prim_t*)malloc(sizeof(ast_type_prim_t));
  clearobj(node);

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

void ast_type_prim_nameres(nameres_ctx_t* ctx, ast_type_prim_t* node)
{
}

void ast_type_prim_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_unit(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_i8_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_i8(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_i16_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_i16(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_i32_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_i32(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_i64_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_i64(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_isize_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_isize(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_u8_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_u8(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_u16_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_u16(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_u32_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_u32(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_u64_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_u64(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_usize_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_usize(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_f32_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_f32(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_f64_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_f64(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_char_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_char(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_bool_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_bool(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_unit_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typebuilder_build_unit(ctx->typebuilder);
  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_prim_codegen(codegen_ctx_t* ctx, ast_type_prim_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  assert(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

size_t ast_type_prim_mangle(ast_type_prim_t* node, char* buf, size_t len)
{
  switch (node->kind)
  {
  case AST_TYPE_PRIM_I8:    return snprintf(buf, len, "sb");
  case AST_TYPE_PRIM_I16:   return snprintf(buf, len, "ss");
  case AST_TYPE_PRIM_I32:   return snprintf(buf, len, "si");
  case AST_TYPE_PRIM_I64:   return snprintf(buf, len, "sl");
  case AST_TYPE_PRIM_ISIZE: return snprintf(buf, len, "sz");
  case AST_TYPE_PRIM_U8:    return snprintf(buf, len, "ub");
  case AST_TYPE_PRIM_U16:   return snprintf(buf, len, "us");
  case AST_TYPE_PRIM_U32:   return snprintf(buf, len, "ui");
  case AST_TYPE_PRIM_U64:   return snprintf(buf, len, "ul");
  case AST_TYPE_PRIM_USIZE: return snprintf(buf, len, "uz");
  case AST_TYPE_PRIM_F32:   return snprintf(buf, len, "f" );
  case AST_TYPE_PRIM_F64:   return snprintf(buf, len, "d" );
  case AST_TYPE_PRIM_CHAR:  return snprintf(buf, len, "c" );
  case AST_TYPE_PRIM_BOOL:  return snprintf(buf, len, "b" );
  case AST_TYPE_PRIM_UNIT:  return snprintf(buf, len, "v" );
  default: unreachable();
  }

  return 0;
}

void ast_type_prim_dump_json(FILE* stream, ast_type_prim_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
