/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/prim.h"

#include "ast/registry.h"

static tau_ast_type_prim_t* tau_ast_type_prim_init(tau_ast_kind_t kind)
{
  tau_ast_type_prim_t* node = (tau_ast_type_prim_t*)malloc(sizeof(tau_ast_type_prim_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = kind;

  return node;
}

tau_ast_type_prim_t* tau_ast_type_prim_i8_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_I8);
}

tau_ast_type_prim_t* tau_ast_type_prim_i16_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_I16);
}

tau_ast_type_prim_t* tau_ast_type_prim_i32_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_I32);
}

tau_ast_type_prim_t* tau_ast_type_prim_i64_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_I64);
}

tau_ast_type_prim_t* tau_ast_type_prim_isize_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_ISIZE);
}

tau_ast_type_prim_t* tau_ast_type_prim_u8_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_U8);
}

tau_ast_type_prim_t* tau_ast_type_prim_u16_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_U16);
}

tau_ast_type_prim_t* tau_ast_type_prim_u32_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_U32);
}

tau_ast_type_prim_t* tau_ast_type_prim_u64_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_U64);
}

tau_ast_type_prim_t* tau_ast_type_prim_usize_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_USIZE);
}

tau_ast_type_prim_t* tau_ast_type_prim_f32_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_F32);
}

tau_ast_type_prim_t* tau_ast_type_prim_f64_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_F64);
}

tau_ast_type_prim_t* tau_ast_type_prim_c64_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_C64);
}

tau_ast_type_prim_t* tau_ast_type_prim_c128_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_C128);
}

tau_ast_type_prim_t* tau_ast_type_prim_char_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_CHAR);
}

tau_ast_type_prim_t* tau_ast_type_prim_bool_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_BOOL);
}

tau_ast_type_prim_t* tau_ast_type_prim_unit_init(void)
{
  return tau_ast_type_prim_init(TAU_AST_TYPE_PRIM_UNIT);
}

void tau_ast_type_prim_free(tau_ast_type_prim_t* node)
{
  free(node);
}

void tau_ast_type_prim_nameres(tau_nameres_ctx_t* TAU_UNUSED(ctx), tau_ast_type_prim_t* TAU_UNUSED(node))
{
}

void tau_ast_type_prim_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_type_prim_t* node)
{
  tau_typedesc_t* desc = NULL;

  switch (node->kind)
  {
  case TAU_AST_TYPE_PRIM_I8:    desc = tau_typebuilder_build_i8   (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_I16:   desc = tau_typebuilder_build_i16  (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_I32:   desc = tau_typebuilder_build_i32  (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_I64:   desc = tau_typebuilder_build_i64  (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_ISIZE: desc = tau_typebuilder_build_isize(ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_U8:    desc = tau_typebuilder_build_u8   (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_U16:   desc = tau_typebuilder_build_u16  (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_U32:   desc = tau_typebuilder_build_u32  (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_U64:   desc = tau_typebuilder_build_u64  (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_USIZE: desc = tau_typebuilder_build_usize(ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_F32:   desc = tau_typebuilder_build_f32  (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_F64:   desc = tau_typebuilder_build_f64  (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_C64:   desc = tau_typebuilder_build_c64  (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_C128:  desc = tau_typebuilder_build_c128 (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_CHAR:  desc = tau_typebuilder_build_char (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_BOOL:  desc = tau_typebuilder_build_bool (ctx->typebuilder); break;
  case TAU_AST_TYPE_PRIM_UNIT:  desc = tau_typebuilder_build_unit (ctx->typebuilder); break;
  default: TAU_UNREACHABLE();
  }

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_type_prim_codegen(tau_codegen_ctx_t* ctx, tau_ast_type_prim_t* node)
{
  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  TAU_ASSERT(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

void tau_ast_type_prim_dump_json(FILE* stream, tau_ast_type_prim_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", tau_ast_kind_to_cstr(node->kind));
}
