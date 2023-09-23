#include "generator.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>

#include "allocator.h"
#include "location.h"
#include "token.h"
#include "util.h"

struct generator_s
{
  LLVMContextRef ctx;
  LLVMModuleRef module;
};

generator_t* generator_init(void)
{
  generator_t* gen = (generator_t*)allocator_allocate(allocator_global(), sizeof(generator_t));

  gen->ctx = LLVMContextCreate();
  gen->module = NULL;

  return gen;
}

void generator_free(generator_t* gen)
{
  LLVMDisposeModule(gen->module);
  LLVMContextDispose(gen->ctx);
  allocator_deallocate(allocator_global(), gen);
}

void generator_visit_type_ptr(generator_t* gen, ast_type_ptr_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->base_type);

  node->llvm_type = LLVMPointerType(((ast_type_t*)node->base_type)->llvm_type, 0);
}

void generator_visit_type(generator_t* gen, ast_type_t* node)
{
  switch (node->kind)
  {
  case AST_TYPE_PTR:   generator_visit_type_ptr  (gen, (ast_type_ptr_t*  )node); break;
  case AST_TYPE_I8:    node->llvm_type = LLVMInt8TypeInContext  (gen->ctx); break;
  case AST_TYPE_I16:   node->llvm_type = LLVMInt16TypeInContext (gen->ctx); break;
  case AST_TYPE_I32:   node->llvm_type = LLVMInt32TypeInContext (gen->ctx); break;
  case AST_TYPE_I64:   node->llvm_type = LLVMInt64TypeInContext (gen->ctx); break;
  case AST_TYPE_ISIZE: node->llvm_type = LLVMInt64TypeInContext (gen->ctx); break;
  case AST_TYPE_U8:    node->llvm_type = LLVMInt8TypeInContext  (gen->ctx); break;
  case AST_TYPE_U16:   node->llvm_type = LLVMInt16TypeInContext (gen->ctx); break;
  case AST_TYPE_U32:   node->llvm_type = LLVMInt32TypeInContext (gen->ctx); break;
  case AST_TYPE_U64:   node->llvm_type = LLVMInt64TypeInContext (gen->ctx); break;
  case AST_TYPE_USIZE: node->llvm_type = LLVMInt64TypeInContext (gen->ctx); break;
  case AST_TYPE_F32:   node->llvm_type = LLVMFloatTypeInContext (gen->ctx); break;
  case AST_TYPE_F64:   node->llvm_type = LLVMDoubleTypeInContext(gen->ctx); break;
  case AST_TYPE_BOOL:  node->llvm_type = LLVMInt1TypeInContext  (gen->ctx); break;
  case AST_TYPE_UNIT:  node->llvm_type = LLVMVoidTypeInContext  (gen->ctx); break;
  default: unreachable();
  }
}

void generator_visit_expr_lit(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_expr_lit_t* node)
{
  size_t lit_len = node->tok->loc->len;

  char* lit_str = allocator_allocate(allocator_global(), lit_len + 1);
  memcpy(lit_str, node->tok->loc->ptr, lit_len);
  lit_str[lit_len] = '\0';

  switch (node->kind)
  {
  case AST_EXPR_LIT_INT:
  {
    uint64_t lit_value = strtoull(lit_str, NULL, 10);

    node->llvm_type = LLVMInt32TypeInContext(gen->ctx);
    node->llvm_value = LLVMConstInt(node->llvm_type, lit_value, false);
    break;
  }
  default:
    unreachable();
  }

  allocator_deallocate(allocator_global(), lit_str);
}

void generator_visit_expr_op_unary(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node)
{
  generator_visit_expr(gen, builder, fun_node, (ast_expr_t*)node->expr);

  switch (node->op_kind)
  {
  case OP_ARIT_POS:
    node->llvm_type = ((ast_expr_t*)node->expr)->llvm_type;
    node->llvm_value = ((ast_expr_t*)node->expr)->llvm_value;
    break;
  case OP_ARIT_NEG:
    node->llvm_value = LLVMBuildNeg(builder, ((ast_expr_t*)node->expr)->llvm_value, "neg_tmp");
    node->llvm_type = LLVMTypeOf(node->llvm_value);
    break;
  case OP_LOGIC_NOT:
  case OP_BIT_NOT:
    node->llvm_value = LLVMBuildNot(builder, ((ast_expr_t*)node->expr)->llvm_value, "not_tmp");
    node->llvm_type = LLVMTypeOf(node->llvm_value);
    break;
  default:
    unreachable();
  }
}

void generator_visit_expr_op_binary(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, builder, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, builder, fun_node, (ast_expr_t*)node->rhs);

  switch (node->op_kind)
  {
  case OP_ARIT_ADD:  node->llvm_value = LLVMBuildAdd (builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "add_tmp" ); break;
  case OP_ARIT_SUB:  node->llvm_value = LLVMBuildSub (builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "sub_tmp" ); break;
  case OP_ARIT_MUL:  node->llvm_value = LLVMBuildMul (builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "mul_tmp" ); break;
  case OP_ARIT_DIV:  node->llvm_value = LLVMBuildSDiv(builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "sdiv_tmp"); break;
  case OP_ARIT_MOD:  node->llvm_value = LLVMBuildSRem(builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "srem_tmp"); break;
  case OP_BIT_AND:
  case OP_LOGIC_AND: node->llvm_value = LLVMBuildAnd (builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "and_tmp" ); break;
  case OP_BIT_OR:
  case OP_LOGIC_OR:  node->llvm_value = LLVMBuildOr  (builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "or_tmp"  ); break;
  case OP_BIT_XOR:   node->llvm_value = LLVMBuildXor (builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "xor_tmp" ); break;
  case OP_BIT_LSH:   node->llvm_value = LLVMBuildShl (builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "shl_tmp" ); break;
  case OP_BIT_RSH:   node->llvm_value = LLVMBuildLShr(builder,             ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "lshr_tmp"); break;
  case OP_COMP_EQ:   node->llvm_value = LLVMBuildICmp(builder, LLVMIntEQ,  ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "icmp_tmp"); break;
  case OP_COMP_NE:   node->llvm_value = LLVMBuildICmp(builder, LLVMIntNE,  ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "icmp_tmp"); break;
  case OP_COMP_LT:   node->llvm_value = LLVMBuildICmp(builder, LLVMIntSLT, ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "icmp_tmp"); break;
  case OP_COMP_LE:   node->llvm_value = LLVMBuildICmp(builder, LLVMIntSLE, ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "icmp_tmp"); break;
  case OP_COMP_GT:   node->llvm_value = LLVMBuildICmp(builder, LLVMIntSGT, ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "icmp_tmp"); break;
  case OP_COMP_GE:   node->llvm_value = LLVMBuildICmp(builder, LLVMIntSGE, ((ast_expr_t*)node->lhs)->llvm_value, ((ast_expr_t*)node->rhs)->llvm_value, "icmp_tmp"); break;
  default: unreachable();
  }

  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_call(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_expr_op_call_t* node)
{
  generator_visit_expr(gen, builder, fun_node, (ast_expr_t*)node->callee);
  
  LLVMValueRef* param_values = allocator_allocate(allocator_global(), list_size(node->params) * sizeof(LLVMValueRef));

  size_t i = 0;
  LIST_FOR_LOOP(it, node->params)
  {
    ast_expr_t* param = (ast_expr_t*)list_node_get(it);
    generator_visit_expr(gen, builder, fun_node, param);

    param_values[i++] = param->llvm_value;
  }

  node->llvm_value = LLVMBuildCall2(builder,
    ((ast_expr_t*)node->callee)->llvm_type,
    ((ast_expr_t*)node->callee)->llvm_value,
    param_values,
    (uint32_t)list_size(node->params),
    "call2_tmp");

  allocator_deallocate(allocator_global(), param_values);

  node->llvm_type = LLVMGetReturnType(((ast_expr_t*)node->callee)->llvm_type);
}

void generator_visit_expr_decl(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_expr_decl_t* node)
{
  switch (node->decl->kind)
  {
  case AST_DECL_VAR:
    node->llvm_type = ((ast_decl_var_t*)node->decl)->llvm_type;
    node->llvm_value = ((ast_decl_var_t*)node->decl)->llvm_value;
    break;
  case AST_DECL_PARAM:
    node->llvm_type = ((ast_decl_param_t*)node->decl)->llvm_type;
    node->llvm_value = ((ast_decl_param_t*)node->decl)->llvm_value;
    break;
  case AST_DECL_FUN:
    node->llvm_type = ((ast_decl_fun_t*)node->decl)->llvm_type;
    node->llvm_value = ((ast_decl_fun_t*)node->decl)->llvm_value;
    break;
  default:
    unreachable();
  }
}

void generator_visit_expr(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_expr_t* node)
{
  switch (node->kind)
  {
  case AST_EXPR_LIT_INT:
  case AST_EXPR_LIT_FLT:
  case AST_EXPR_LIT_STR:
  case AST_EXPR_LIT_CHAR:
  case AST_EXPR_LIT_BOOL:
  case AST_EXPR_LIT_NULL:  generator_visit_expr_lit      (gen, builder, fun_node, (ast_expr_lit_t*    )node); break;
  case AST_EXPR_OP_UNARY:  generator_visit_expr_op_unary (gen, builder, fun_node, (ast_expr_op_un_t*  )node); break;
  case AST_EXPR_OP_BINARY: generator_visit_expr_op_binary(gen, builder, fun_node, (ast_expr_op_bin_t* )node); break;
  case AST_EXPR_OP_CALL:   generator_visit_expr_op_call  (gen, builder, fun_node, (ast_expr_op_call_t*)node); break;
  case AST_EXPR_DECL:      generator_visit_expr_decl     (gen, builder, fun_node, (ast_expr_decl_t*   )node); break;
  default: unreachable();
  }
}

void generator_visit_stmt_if(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_stmt_if_t* node)
{
  generator_visit_expr(gen, builder, fun_node, (ast_expr_t*)node->cond);

  node->llvm_then = LLVMAppendBasicBlockInContext(gen->ctx, fun_node->llvm_value, "if_then");

  if (node->stmt_else != NULL)
    node->llvm_else = LLVMAppendBasicBlockInContext(gen->ctx, fun_node->llvm_value, "if_else");
  
  node->llvm_end = LLVMAppendBasicBlockInContext(gen->ctx, fun_node->llvm_value, "if_end");

  node->llvm_value = LLVMBuildCondBr(builder, ((ast_expr_t*)node->cond)->llvm_value, node->llvm_then, node->stmt_else != NULL ? node->llvm_else : node->llvm_end);

  LLVMPositionBuilderAtEnd(builder, node->llvm_then);

  generator_visit_stmt(gen, builder, fun_node, (ast_stmt_t*)node->stmt);

  if (LLVMGetBasicBlockTerminator(node->llvm_then) == NULL)
    LLVMBuildBr(builder, node->llvm_end);

  if (node->stmt_else != NULL)
  {
    LLVMPositionBuilderAtEnd(builder, node->llvm_else);

    generator_visit_stmt(gen, builder, fun_node, (ast_stmt_t*)node->stmt_else);

    if (LLVMGetBasicBlockTerminator(node->llvm_else) == NULL)
      LLVMBuildBr(builder, node->llvm_end);
  }

  LLVMPositionBuilderAtEnd(builder, node->llvm_end);
}

void generator_visit_stmt_return(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_stmt_return_t* node)
{
  if (node->expr == NULL)
    LLVMBuildRetVoid(builder);
  else
  {
    generator_visit_expr(gen, builder, fun_node, (ast_expr_t*)node->expr);
    LLVMBuildRet(builder, ((ast_expr_t*)node->expr)->llvm_value);
  }
}

void generator_visit_stmt_block(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_stmt_block_t* node)
{
  LIST_FOR_LOOP(it, node->stmts)
    generator_visit_stmt(gen, builder, fun_node, (ast_stmt_t*)list_node_get(it));
}

void generator_visit_stmt(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_stmt_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:    generator_visit_decl_var   (gen, builder, fun_node, (ast_decl_var_t*   )node); break;
  case AST_STMT_IF:     generator_visit_stmt_if    (gen, builder, fun_node, (ast_stmt_if_t*    )node); break;
  case AST_STMT_RETURN: generator_visit_stmt_return(gen, builder, fun_node, (ast_stmt_return_t*)node); break;
  case AST_STMT_BLOCK:  generator_visit_stmt_block (gen, builder, fun_node, (ast_stmt_block_t* )node); break;
  default: unreachable();
  }
}

void generator_visit_decl_var(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_decl_var_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->type);
  generator_visit_expr(gen, builder, fun_node, (ast_expr_t*)node->expr);

  node->llvm_type = ((ast_type_t*)node->type)->llvm_type;
  node->llvm_value = ((ast_expr_t*)node->expr)->llvm_value;

  assert(node->llvm_type == ((ast_expr_t*)node->expr)->llvm_type);
}

void generator_visit_decl_fun(generator_t* gen, ast_decl_fun_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->return_type);

  size_t param_count = 0;
  LLVMTypeRef* param_types = NULL;

  if (node->params != NULL)
  {
    param_count = list_size(node->params);
    param_types = allocator_allocate(allocator_global(), param_count * sizeof(LLVMTypeRef));

    size_t i = 0;
    LIST_FOR_LOOP(it, node->params)
    {
      ast_decl_param_t* param = (ast_decl_param_t*)list_node_get(it);

      generator_visit_type(gen, (ast_type_t*)param->type);

      param_types[i++] = ((ast_type_t*)param->type)->llvm_type;
    }
  }

  node->llvm_type = LLVMFunctionType(
    ((ast_type_t*)node->return_type)->llvm_type,
    param_types,
    (uint32_t)param_count,
    false
  );

  if (param_types != NULL)
    allocator_deallocate(allocator_global(), param_types);

  size_t fun_id_len = node->id->tok->loc->len;

  char* fun_id_str = allocator_allocate(allocator_global(), fun_id_len + 1);
  memcpy(fun_id_str, node->id->tok->loc->ptr, fun_id_len);
  fun_id_str[fun_id_len] = '\0';

  node->llvm_value = LLVMAddFunction(gen->module, fun_id_str, node->llvm_type);

  allocator_deallocate(allocator_global(), fun_id_str);

  node->llvm_entry = LLVMAppendBasicBlockInContext(gen->ctx, node->llvm_value, "entry");

  LLVMBuilderRef builder = LLVMCreateBuilderInContext(gen->ctx);
  LLVMPositionBuilderAtEnd(builder, node->llvm_entry);

  size_t i = 0;
  LIST_FOR_LOOP(it, node->params)
    generator_visit_decl_param(gen, builder, node, (ast_decl_param_t*)list_node_get(it), i++);

  generator_visit_stmt(gen, builder, node, (ast_stmt_t*)node->stmt);

  LLVMDisposeBuilder(builder);
}

void generator_visit_decl_param(generator_t* gen, LLVMBuilderRef builder, ast_decl_fun_t* fun_node, ast_decl_param_t* node, size_t idx)
{
  unused(gen);
  unused(builder);

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
  gen->module = LLVMModuleCreateWithNameInContext("PROG", gen->ctx);

  LIST_FOR_LOOP(it, node->decls)
  {
    ast_decl_t* decl_node = (ast_decl_t*)list_node_get(it);
    generator_visit_decl(gen, decl_node);
  }
}

LLVMModuleRef generator_generate(generator_t* gen, ast_node_t* node)
{
  assert(node->kind == AST_PROG);
  generator_visit_prog(gen, (ast_prog_t*)node);

  char* error = NULL;
  if (LLVMVerifyModule(gen->module, LLVMReturnStatusAction, &error))
    puts(error);

  LLVMDisposeMessage(error);

  return gen->module;
}
