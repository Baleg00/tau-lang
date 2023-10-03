/**
 * \file generator.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "generator.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>

#include "location.h"
#include "memtrace.h"
#include "token.h"
#include "util.h"

struct generator_s
{
  typetable_t* typetable; // The type table for resolving types.
  LLVMContextRef context; // The LLVM context for code generation.
  LLVMModuleRef module;   // The LLVM module for the current translation unit.
  LLVMBuilderRef builder; // The LLVM IR builder for generating code.
};

generator_t* generator_init(void)
{
  generator_t* gen = (generator_t*)malloc(sizeof(generator_t));
  memset(gen, 0, sizeof(generator_t));
  return gen;
}

void generator_free(generator_t* gen)
{
  free(gen);
}

void generator_visit_type_ptr(generator_t* gen, ast_type_ptr_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->base_type);

  node->llvm_type = LLVMPointerType(((ast_type_t*)node->base_type)->llvm_type, 0);
}

void generator_visit_type_array(generator_t* gen, ast_type_array_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->base_type);

  node->llvm_type = LLVMArrayType2(((ast_type_t*)node->base_type)->llvm_type, 0);
}

void generator_visit_type(generator_t* gen, ast_type_t* node)
{
  switch (node->kind)
  {
  case AST_TYPE_PTR:   generator_visit_type_ptr  (gen, (ast_type_ptr_t*  )node); break;
  case AST_TYPE_ARRAY: generator_visit_type_array(gen, (ast_type_array_t*)node); break;
  case AST_TYPE_I8:    node->llvm_type = LLVMInt8TypeInContext  (gen->context); break;
  case AST_TYPE_I16:   node->llvm_type = LLVMInt16TypeInContext (gen->context); break;
  case AST_TYPE_I32:   node->llvm_type = LLVMInt32TypeInContext (gen->context); break;
  case AST_TYPE_I64:   node->llvm_type = LLVMInt64TypeInContext (gen->context); break;
  case AST_TYPE_ISIZE: node->llvm_type = LLVMInt64TypeInContext (gen->context); break;
  case AST_TYPE_U8:    node->llvm_type = LLVMInt8TypeInContext  (gen->context); break;
  case AST_TYPE_U16:   node->llvm_type = LLVMInt16TypeInContext (gen->context); break;
  case AST_TYPE_U32:   node->llvm_type = LLVMInt32TypeInContext (gen->context); break;
  case AST_TYPE_U64:   node->llvm_type = LLVMInt64TypeInContext (gen->context); break;
  case AST_TYPE_USIZE: node->llvm_type = LLVMInt64TypeInContext (gen->context); break;
  case AST_TYPE_F32:   node->llvm_type = LLVMFloatTypeInContext (gen->context); break;
  case AST_TYPE_F64:   node->llvm_type = LLVMDoubleTypeInContext(gen->context); break;
  case AST_TYPE_BOOL:  node->llvm_type = LLVMInt1TypeInContext  (gen->context); break;
  case AST_TYPE_UNIT:  node->llvm_type = LLVMVoidTypeInContext  (gen->context); break;
  default: unreachable();
  }
}

void generator_visit_expr_lit_int(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_lit_t* node)
{
  node->llvm_type = LLVMInt32TypeInContext(gen->context);
  node->llvm_value = LLVMConstIntOfStringAndSize(node->llvm_type, node->tok->loc->ptr, (uint32_t)node->tok->loc->len, 10);
}

void generator_visit_expr_lit_flt(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_lit_t* node)
{
  node->llvm_type = LLVMFloatTypeInContext(gen->context);
  node->llvm_value = LLVMConstRealOfStringAndSize(node->llvm_type, node->tok->loc->ptr, (uint32_t)node->tok->loc->len);
}

void generator_visit_expr_lit_str(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_lit_t* node)
{
  size_t lit_len = 0;

  for (char* ch = (char*)node->tok->loc->ptr + 1; *ch != '"'; ch++, lit_len++)
    if (*ch == '\\')
    {
      ch++;

      switch (*ch)
      {
      case '\\':
      case 'b':
      case 'f':
      case 'n':
      case 'r':
      case 't':
      case '\'':
      case '"':
        break;
      default:
        unreachable();
      }
    }

  char* lit_value = malloc(lit_len + 1);

  for (char *ch = (char*)node->tok->loc->ptr + 1, *ptr = lit_value; *ch != '"'; ch++, ptr++)
    if (*ch == '\\')
    {
      ch++;

      switch (*ch)
      {
      case '\\': *ptr = '\\'; break;
      case 'b':  *ptr = '\b'; break;
      case 'f':  *ptr = '\f'; break;
      case 'n':  *ptr = '\n'; break;
      case 'r':  *ptr = '\r'; break;
      case 't':  *ptr = '\t'; break;
      case '\'': *ptr = '\''; break;
      case '"':  *ptr = '"';  break;
      default: unreachable();
      }
    }
    else
      *ptr = *ch;

  lit_value[lit_len] = '\0';

  node->llvm_value = LLVMBuildGlobalStringPtr(gen->builder, lit_value, "global_str");
  node->llvm_type = LLVMTypeOf(node->llvm_value);

  free(lit_value);
}

void generator_visit_expr_lit_bool(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_lit_t* node)
{
  bool lit_value = strncmp("true", node->tok->loc->ptr, node->tok->loc->len) == 0;

  node->llvm_type = LLVMInt1TypeInContext(gen->context);
  node->llvm_value = LLVMConstInt(node->llvm_type, (uint32_t)lit_value, false);
}

void generator_visit_expr_op_unary(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);

  switch (node->op_kind)
  {
  case OP_ARIT_POS:
    node->llvm_type = ((ast_expr_t*)node->expr)->llvm_type;
    node->llvm_value = ((ast_expr_t*)node->expr)->llvm_value;
    break;
  case OP_ARIT_NEG:
    node->llvm_value = LLVMBuildNeg(gen->builder, ((ast_expr_t*)node->expr)->llvm_value, "neg_tmp");
    node->llvm_type = LLVMTypeOf(node->llvm_value);
    break;
  case OP_LOGIC_NOT:
  case OP_BIT_NOT:
    node->llvm_value = LLVMBuildNot(gen->builder, ((ast_expr_t*)node->expr)->llvm_value, "not_tmp");
    node->llvm_type = LLVMTypeOf(node->llvm_value);
    break;
  default:
    unreachable();
  }
}

void generator_visit_expr_op_binary(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  switch (node->op_kind)
  {
  case OP_ARIT_ADD:  node->llvm_value = LLVMBuildAdd (gen->builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "add_tmp" ); break;
  case OP_ARIT_SUB:  node->llvm_value = LLVMBuildSub (gen->builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "sub_tmp" ); break;
  case OP_ARIT_MUL:  node->llvm_value = LLVMBuildMul (gen->builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "mul_tmp" ); break;
  case OP_ARIT_DIV:  node->llvm_value = LLVMBuildSDiv(gen->builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "sdiv_tmp"); break;
  case OP_ARIT_MOD:  node->llvm_value = LLVMBuildSRem(gen->builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "srem_tmp"); break;
  case OP_BIT_AND:
  case OP_LOGIC_AND: node->llvm_value = LLVMBuildAnd (gen->builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "and_tmp" ); break;
  case OP_BIT_OR:
  case OP_LOGIC_OR:  node->llvm_value = LLVMBuildOr  (gen->builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "or_tmp"  ); break;
  case OP_BIT_XOR:   node->llvm_value = LLVMBuildXor (gen->builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "xor_tmp" ); break;
  case OP_BIT_LSH:   node->llvm_value = LLVMBuildShl (gen->builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "shl_tmp" ); break;
  case OP_BIT_RSH:   node->llvm_value = LLVMBuildLShr(gen->builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "lshr_tmp"); break;
  case OP_COMP_EQ:   node->llvm_value = LLVMBuildICmp(gen->builder, LLVMIntEQ,  ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "icmp_tmp"); break;
  case OP_COMP_NE:   node->llvm_value = LLVMBuildICmp(gen->builder, LLVMIntNE,  ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "icmp_tmp"); break;
  case OP_COMP_LT:   node->llvm_value = LLVMBuildICmp(gen->builder, LLVMIntSLT, ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "icmp_tmp"); break;
  case OP_COMP_LE:   node->llvm_value = LLVMBuildICmp(gen->builder, LLVMIntSLE, ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "icmp_tmp"); break;
  case OP_COMP_GT:   node->llvm_value = LLVMBuildICmp(gen->builder, LLVMIntSGT, ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "icmp_tmp"); break;
  case OP_COMP_GE:   node->llvm_value = LLVMBuildICmp(gen->builder, LLVMIntSGE, ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "icmp_tmp"); break;
  default: unreachable();
  }

  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_call(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_call_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->callee);
  
  LLVMValueRef* param_values = malloc(list_size(node->params) * sizeof(LLVMValueRef));

  size_t i = 0;
  LIST_FOR_LOOP(it, node->params)
  {
    ast_expr_t* param = (ast_expr_t*)list_node_get(it);
    generator_visit_expr(gen, fun_node, param);

    param_values[i++] = param->llvm_value;
  }

  node->llvm_value = LLVMBuildCall2(gen->builder,
    ((ast_expr_t*)node->callee)->llvm_type,
    ((ast_expr_t*)node->callee)->llvm_value,
    param_values,
    (uint32_t)list_size(node->params),
    "call2_tmp");

  free(param_values);

  node->llvm_type = LLVMGetReturnType(((ast_expr_t*)node->callee)->llvm_type);
}

void generator_visit_expr_decl_var(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_decl_t* node)
{
  node->llvm_type = ((ast_decl_var_t*)node->decl)->llvm_type;
  node->llvm_value = ((ast_decl_var_t*)node->decl)->llvm_value;
}

void generator_visit_expr_decl_param(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_decl_t* node)
{
  node->llvm_type = ((ast_decl_param_t*)node->decl)->llvm_type;
  node->llvm_value = ((ast_decl_param_t*)node->decl)->llvm_value;
}

void generator_visit_expr_decl_fun(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_decl_t* node)
{
  node->llvm_type = ((ast_decl_fun_t*)node->decl)->llvm_type;
  node->llvm_value = ((ast_decl_fun_t*)node->decl)->llvm_value;
}

void generator_visit_expr_decl(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_decl_t* node)
{
  switch (node->decl->kind)
  {
  case AST_DECL_VAR:   generator_visit_expr_decl_var  (gen, fun_node, node); break;
  case AST_DECL_PARAM: generator_visit_expr_decl_param(gen, fun_node, node); break;
  case AST_DECL_FUN:   generator_visit_expr_decl_fun  (gen, fun_node, node); break;
  default: unreachable();
  }
}

void generator_visit_expr(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_t* node)
{
  switch (node->kind)
  {
  case AST_EXPR_LIT_INT:   generator_visit_expr_lit_int  (gen, fun_node, (ast_expr_lit_t*    )node); break;
  case AST_EXPR_LIT_FLT:   generator_visit_expr_lit_flt  (gen, fun_node, (ast_expr_lit_t*    )node); break;
  case AST_EXPR_LIT_STR:   generator_visit_expr_lit_str  (gen, fun_node, (ast_expr_lit_t*    )node); break;
  case AST_EXPR_LIT_BOOL:  generator_visit_expr_lit_bool (gen, fun_node, (ast_expr_lit_t*    )node); break;
  case AST_EXPR_OP_UNARY:  generator_visit_expr_op_unary (gen, fun_node, (ast_expr_op_un_t*  )node); break;
  case AST_EXPR_OP_BINARY: generator_visit_expr_op_binary(gen, fun_node, (ast_expr_op_bin_t* )node); break;
  case AST_EXPR_OP_CALL:   generator_visit_expr_op_call  (gen, fun_node, (ast_expr_op_call_t*)node); break;
  case AST_EXPR_DECL:      generator_visit_expr_decl     (gen, fun_node, (ast_expr_decl_t*   )node); break;
  default: unreachable();
  }
}

void generator_visit_stmt_if(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_if_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->cond);

  node->llvm_then = LLVMAppendBasicBlockInContext(gen->context, fun_node->llvm_value, "if_then");

  if (node->stmt_else != NULL)
    node->llvm_else = LLVMAppendBasicBlockInContext(gen->context, fun_node->llvm_value, "if_else");
  
  node->llvm_end = LLVMAppendBasicBlockInContext(gen->context, fun_node->llvm_value, "if_end");

  node->llvm_value = LLVMBuildCondBr(gen->builder, ((ast_expr_t*)node->cond)->llvm_value, node->llvm_then, node->stmt_else != NULL ? node->llvm_else : node->llvm_end);

  LLVMPositionBuilderAtEnd(gen->builder, node->llvm_then);

  generator_visit_stmt(gen, fun_node, (ast_stmt_t*)node->stmt);

  if (LLVMGetBasicBlockTerminator(node->llvm_then) == NULL)
    LLVMBuildBr(gen->builder, node->llvm_end);

  if (node->stmt_else != NULL)
  {
    LLVMPositionBuilderAtEnd(gen->builder, node->llvm_else);

    generator_visit_stmt(gen, fun_node, (ast_stmt_t*)node->stmt_else);

    if (LLVMGetBasicBlockTerminator(node->llvm_else) == NULL)
      LLVMBuildBr(gen->builder, node->llvm_end);
  }

  LLVMPositionBuilderAtEnd(gen->builder, node->llvm_end);
}

void generator_visit_stmt_return(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_return_t* node)
{
  if (node->expr == NULL)
    LLVMBuildRetVoid(gen->builder);
  else
  {
    generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);
    LLVMBuildRet(gen->builder, ((ast_expr_t*)node->expr)->llvm_value);
  }
}

void generator_visit_stmt_block(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_block_t* node)
{
  LIST_FOR_LOOP(it, node->stmts)
    generator_visit_stmt(gen, fun_node, (ast_stmt_t*)list_node_get(it));
}

void generator_visit_stmt(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:    generator_visit_decl_var   (gen, fun_node, (ast_decl_var_t*   )node); break;
  case AST_STMT_IF:     generator_visit_stmt_if    (gen, fun_node, (ast_stmt_if_t*    )node); break;
  case AST_STMT_RETURN: generator_visit_stmt_return(gen, fun_node, (ast_stmt_return_t*)node); break;
  case AST_STMT_BLOCK:  generator_visit_stmt_block (gen, fun_node, (ast_stmt_block_t* )node); break;
  case AST_STMT_EXPR:   generator_visit_expr       (gen, fun_node, (ast_expr_t*       )((ast_stmt_expr_t*)node)->expr); break;
  default: unreachable();
  }
}

void generator_visit_decl_var(generator_t* gen, ast_decl_fun_t* fun_node, ast_decl_var_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->type);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);

  node->llvm_type = ((ast_type_t*)node->type)->llvm_type;
  node->llvm_value = ((ast_expr_t*)node->expr)->llvm_value;
}

void generator_visit_decl_fun(generator_t* gen, ast_decl_fun_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->return_type);

  size_t param_count = 0;
  LLVMTypeRef* param_types = NULL;

  if (node->params != NULL)
  {
    param_count = list_size(node->params);
    param_types = malloc(param_count * sizeof(LLVMTypeRef));

    size_t i = 0;
    LIST_FOR_LOOP(it, node->params)
    {
      ast_decl_param_t* param = (ast_decl_param_t*)list_node_get(it);

      generator_visit_type(gen, (ast_type_t*)param->type);

      param_types[i++] = ((ast_type_t*)param->type)->llvm_type;
    }
  }

  bool is_vararg = false;
  LLVMCallConv callconv = LLVMCCallConv;

  if (node->attrs != NULL)
    LIST_FOR_LOOP(it, node->attrs)
    {
      ast_attr_t* attr_node = (ast_attr_t*)list_node_get(it);

      const char* attr_id_ptr = attr_node->id->tok->loc->ptr;
      size_t attr_id_len = attr_node->id->tok->loc->len;

      if (strncmp("cdecl", attr_id_ptr, attr_id_len) == 0)
        callconv = LLVMCCallConv;
      else if (strncmp("vararg", attr_id_ptr, attr_id_len) == 0)
        is_vararg = true;
    }

  node->llvm_type = LLVMFunctionType(
    ((ast_type_t*)node->return_type)->llvm_type,
    param_types,
    (uint32_t)param_count,
    is_vararg
  );

  if (param_types != NULL)
    free(param_types);

  size_t fun_id_len = node->id->tok->loc->len;

  char* fun_id_str = malloc(fun_id_len + 1);
  memcpy(fun_id_str, node->id->tok->loc->ptr, fun_id_len);
  fun_id_str[fun_id_len] = '\0';

  node->llvm_value = LLVMAddFunction(gen->module, fun_id_str, node->llvm_type);

  free(fun_id_str);

  LLVMSetFunctionCallConv(node->llvm_value, callconv);

  if (!node->is_extern)
  {
    node->llvm_entry = LLVMAppendBasicBlockInContext(gen->context, node->llvm_value, "entry");

    LLVMPositionBuilderAtEnd(gen->builder, node->llvm_entry);

    size_t i = 0;
    LIST_FOR_LOOP(it, node->params)
      generator_visit_decl_param(gen, node, (ast_decl_param_t*)list_node_get(it), i++);

    generator_visit_stmt(gen, node, (ast_stmt_t*)node->stmt);
  }
}

void generator_visit_decl_param(generator_t* gen, ast_decl_fun_t* fun_node, ast_decl_param_t* node, size_t idx)
{
  node->llvm_value = LLVMGetParam(fun_node->llvm_value, (uint32_t)idx);
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_decl(generator_t* gen, ast_decl_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_FUN: generator_visit_decl_fun(gen, (ast_decl_fun_t*)node); break;
  default: unreachable();
  }
}

void generator_visit_prog(generator_t* gen, ast_prog_t* node)
{
  LIST_FOR_LOOP(it, node->decls)
  {
    ast_decl_t* decl_node = (ast_decl_t*)list_node_get(it);
    generator_visit_decl(gen, decl_node);
  }
}

void generator_generate(generator_t* gen, LLVMContextRef context, LLVMModuleRef module, typetable_t* typetable, ast_node_t* node)
{
  assert(node->kind == AST_PROG);

  gen->typetable = typetable;
  gen->context = context;
  gen->module = module;
  gen->builder = LLVMCreateBuilderInContext(gen->context);

  generator_visit_prog(gen, (ast_prog_t*)node);

  LLVMDisposeBuilder(gen->builder);

  char* error = NULL;
  if (LLVMVerifyModule(gen->module, LLVMReturnStatusAction, &error))
    puts(error);

  LLVMDisposeMessage(error);
}
