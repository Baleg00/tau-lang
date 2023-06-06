#include "bytecode.h"

#include <string.h>

#include "util.h"

#include "list.h"

#include "location.h"
#include "token.h"
#include "ast.h"
#include "op.h"
#include "opcode.h"
#include "typedesc.h"

#include "arena.h"
#include "memtrace.h"

#define BYTECODE_INITIAL_CAPACITY ((size_t)(4 * (1 << 10)))
#define BYTECODE_EXPAND_AMOUNT    ((size_t)(2 * (1 << 10)))

bytecode_label_t* bytecode_label_init(ast_node_t* node, int64_t offset)
{
  bytecode_label_t* label = (bytecode_label_t*)malloc(sizeof(bytecode_label_t));
  assert(label != NULL);

  label->node = node;
  label->offset = offset;

  return label;
}

void bytecode_label_free(bytecode_label_t* label)
{
  unused(label);
}

bytecode_t* bytecode_init(void)
{
  bytecode_t* bc = (bytecode_t*)malloc(sizeof(bytecode_t));
  assert(bc != NULL);

  bc->capacity = BYTECODE_INITIAL_CAPACITY;
  bc->size = 0;

  bc->data = (uint8_t*)malloc(bc->capacity * sizeof(uint8_t));
  assert(bc->data != NULL);

  bc->labels = list_init();

  bc->sp = 0;

  return bc;
}

LIST_FOR_EACH_FUNC_DECL(bytecode_label_free, bytecode_label_t)

void bytecode_free(bytecode_t* bc)
{
  free(bc->data);

  list_for_each(bc->labels, LIST_FOR_EACH_FUNC_NAME(bytecode_label_free));
  list_free(bc->labels);
}

void bytecode_expand(bytecode_t* bc)
{
  size_t new_capacity = bc->capacity + BYTECODE_EXPAND_AMOUNT;
  void* new_data = realloc(bc->data, new_capacity);
  assert(new_data != NULL);

  bc->capacity = new_capacity;
  bc->data = new_data;
}

void bytecode_emit_opcode(bytecode_t* bc, opcode_t op)
{
  while (bc->size + sizeof(opcode_t) >= bc->capacity)
    bytecode_expand(bc);

  memcpy(bc->data + bc->size, &op, sizeof(opcode_t));
  bc->size += sizeof(opcode_t);
}

void bytecode_emit_bytes(bytecode_t* bc, void* buf, size_t size)
{
  while (bc->size + size >= bc->capacity)
    bytecode_expand(bc);

  memcpy(bc->data + bc->size, buf, size);
  bc->size += size;
}

void bytecode_reg_fun(bytecode_t* bc, ast_decl_fun_t* node)
{
  bytecode_label_t* label = bytecode_label_init((ast_node_t*)node, (int64_t)bc->size);
  list_push_back(bc->labels, label);
}

void bytecode_reg_var(bytecode_t* bc, ast_decl_var_t* node)
{
  bytecode_label_t* label = bytecode_label_init((ast_node_t*)node, (int64_t)bc->sp);
  list_push_back(bc->labels, label);
}

void bytecode_reg_param(bytecode_t* bc, ast_param_t* node, int64_t offset)
{
  bytecode_label_t* label = bytecode_label_init((ast_node_t*)node, offset);
  list_push_back(bc->labels, label);
}

int64_t bytecode_label_offset(bytecode_t* bc, ast_node_t* node)
{
  LIST_FOR_LOOP(it, bc->labels)
  {
    bytecode_label_t* label = (bytecode_label_t*)list_node_get(it);

    if (node == label->node)
      return label->offset;
  }

  unreachable();

  return -1;
}

void bytecode_visit_expr_op_unary(bytecode_t* bc, ast_expr_op_un_t* node)
{
  switch (node->op_kind)
  {
  case OP_SIZEOF:
    {
      bytecode_emit_opcode(bc, OPCODE_PUSH8);
      size_t sizeof_value = ast_desc_of(node->param)->size;
      bytecode_emit_bytes(bc, &sizeof_value, sizeof(size_t));
      break;
    }
  case OP_ARIT_POS:
    break;
  case OP_ALIGNOF:
    {
      bytecode_emit_opcode(bc, OPCODE_PUSH8);
      size_t alignof_value = ast_desc_of(node->param)->align;
      bytecode_emit_bytes(bc, &alignof_value, sizeof(size_t));
      break;
    }
  case OP_ARIT_NEG: // TODO
  case OP_LOGIC_NOT: // TODO
  case OP_BIT_NOT: // TODO
  case OP_IND: // TODO
  case OP_ADDR: // TODO
  case OP_TYPEOF: // TODO
  case OP_ARIT_INC_PRE: // TODO
  case OP_ARIT_INC_POST: // TODO
  case OP_ARIT_DEC_PRE: // TODO
  case OP_ARIT_DEC_POST: // TODO
  default: unreachable();
  }
}

void bytecode_visit_expr_op_binary(bytecode_t* bc, ast_expr_op_bin_t* node)
{
  switch (node->op_kind)
  {
  case OP_ARIT_ADD:
    bytecode_visit_expr(bc, (ast_expr_t*)node->lhs);
    bytecode_visit_expr(bc, (ast_expr_t*)node->rhs);
    bytecode_emit_opcode(bc, OPCODE_ADDI); // TODO
    break;
  case OP_IS: // TODO
  case OP_AS: // TODO
  case OP_IN: // TODO
  case OP_ARIT_SUB: // TODO
  case OP_ARIT_MUL: // TODO
  case OP_ARIT_DIV: // TODO
  case OP_ARIT_MOD: // TODO
  case OP_BIT_AND: // TODO
  case OP_BIT_OR: // TODO
  case OP_BIT_XOR: // TODO
  case OP_BIT_LSH: // TODO
  case OP_BIT_RSH: // TODO
  case OP_LOGIC_AND: // TODO
  case OP_LOGIC_OR: // TODO
  case OP_COMP_EQ: // TODO
  case OP_COMP_NE: // TODO
  case OP_COMP_LT: // TODO
  case OP_COMP_LE: // TODO
  case OP_COMP_GT: // TODO
  case OP_COMP_GE: // TODO
  case OP_ASSIGN: // TODO
  case OP_ARIT_ADD_ASSIGN: // TODO
  case OP_ARIT_SUB_ASSIGN: // TODO
  case OP_ARIT_MUL_ASSIGN: // TODO
  case OP_ARIT_DIV_ASSIGN: // TODO
  case OP_ARIT_MOD_ASSIGN: // TODO
  case OP_BIT_AND_ASSIGN: // TODO
  case OP_BIT_OR_ASSIGN: // TODO
  case OP_BIT_XOR_ASSIGN: // TODO
  case OP_BIT_LSH_ASSIGN: // TODO
  case OP_BIT_RSH_ASSIGN: // TODO
  case OP_SUBS: // TODO
  case OP_MEMBER: // TODO
  case OP_IND_MEMBER: // TODO
  case OP_NULL_SAFE_MEMBER: // TODO
  case OP_RANGE: // TODO
  case OP_SEMICOLON: // TODO
  default: unreachable();
  }
}

void bytecode_visit_expr_op_call(bytecode_t* bc, ast_expr_op_call_t* node)
{
  LIST_FOR_LOOP(it, node->params)
    bytecode_visit_expr(bc, (ast_expr_t*)list_node_get(it));

  bytecode_emit_opcode(bc, OPCODE_INVOKE);
  int64_t fun_addr = bytecode_label_offset(bc, node->callee);
  bytecode_emit_bytes(bc, &fun_addr, sizeof(int64_t));
}

void bytecode_visit_expr_op(bytecode_t* bc, ast_expr_op_t* node)
{
  if (node->op_kind == OP_CALL)
    bytecode_visit_expr_op_call(bc, (ast_expr_op_call_t*)node);
  else if (op_is_unary(node->op_kind))
    bytecode_visit_expr_op_unary(bc, (ast_expr_op_un_t*)node);
  else if (op_is_binary(node->op_kind))
    bytecode_visit_expr_op_binary(bc, (ast_expr_op_bin_t*)node);
  else
    unreachable();
  
  // TODO
  // if (node->op_kind == OP_MEMBER ||
  //     node->op_kind == OP_IND_MEMBER ||
  //     node->op_kind == OP_NULL_SAFE_MEMBER)
  //   return analyzer_visit_expr_op_member(bc, (ast_expr_op_bin_t*)node);  
}

void bytecode_visit_expr(bytecode_t* bc, ast_expr_t* node)
{
  switch (node->kind)
  {
  case AST_EXPR_LIT_INT:
    bytecode_emit_opcode(bc, OPCODE_PUSH4);
    int32_t int_value = (int32_t)strtoll(node->tok->loc->cur, NULL, 10); // TODO
    bytecode_emit_bytes(bc, &int_value, sizeof(int32_t));
    break;
  case AST_EXPR_OP:
    bytecode_visit_expr_op(bc, (ast_expr_op_t*)node);
    break;
  case AST_DECL_VAR:
    bytecode_emit_opcode(bc, OPCODE_LOAD4BP);
    int64_t var_offset = bytecode_label_offset(bc, (ast_node_t*)node);
    bytecode_emit_bytes(bc, &var_offset, sizeof(int64_t));
    break;
  case AST_DECL_FUN:
    bytecode_emit_opcode(bc, OPCODE_INVOKE);
    int64_t fun_offset = bytecode_label_offset(bc, (ast_node_t*)node);
    bytecode_emit_bytes(bc, &fun_offset, sizeof(int64_t));
    break;
  case AST_PARAM:
    bytecode_emit_opcode(bc, OPCODE_PUSH8);
    int64_t param_offset = bytecode_label_offset(bc, (ast_node_t*)node);
    bytecode_emit_bytes(bc, &param_offset, sizeof(int64_t));
    bytecode_emit_opcode(bc, OPCODE_LOAD4BP); // TODO
    break;
  case AST_EXPR_LIT_FLT: // TODO
  case AST_EXPR_LIT_STR: // TODO
  case AST_EXPR_LIT_CHAR: // TODO
  case AST_EXPR_LIT_BOOL: // TODO
  case AST_EXPR_LIT_NULL: // TODO
  case AST_DECL_GEN: // TODO
  case AST_PARAM_DEFAULT: // TODO
  case AST_PARAM_VARIADIC: // TODO
  default: unreachable();
  }
}

void bytecode_visit_stmt_if(bytecode_t* bc, ast_stmt_if_t* node)
{
  unused(bc);
  unused(node);
  unreachable();
}

void bytecode_visit_stmt_for(bytecode_t* bc, ast_stmt_for_t* node)
{
  unused(bc);
  unused(node);
  unreachable();
}

void bytecode_visit_stmt_while(bytecode_t* bc, ast_stmt_while_t* node)
{
  unused(bc);
  unused(node);
  unreachable();
}

void bytecode_visit_stmt_break(bytecode_t* bc, ast_stmt_break_t* node)
{
  unused(bc);
  unused(node);
  unreachable();
}

void bytecode_visit_stmt_continue(bytecode_t* bc, ast_stmt_continue_t* node)
{
  unused(bc);
  unused(node);
  unreachable();
}

void bytecode_visit_stmt_return(bytecode_t* bc, ast_stmt_return_t* node)
{
  if (node->expr != NULL)
    bytecode_visit_expr(bc, (ast_expr_t*)node->expr);

  bytecode_emit_opcode(bc, OPCODE_RETURN);
}

void bytecode_visit_stmt_yield(bytecode_t* bc, ast_stmt_yield_t* node)
{
  unused(bc);
  unused(node);
  unreachable();
}

void bytecode_visit_stmt_block(bytecode_t* bc, ast_stmt_block_t* node)
{
  LIST_FOR_LOOP(it, node->stmts)
    bytecode_visit_stmt(bc, (ast_stmt_t*)list_node_get(it));
}

void bytecode_visit_stmt_expr(bytecode_t* bc, ast_stmt_expr_t* node)
{
  bytecode_visit_expr(bc, (ast_expr_t*)node->expr);
}

void bytecode_visit_stmt(bytecode_t* bc, ast_stmt_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:      bytecode_visit_decl_var(bc, (ast_decl_var_t*)node); break;
  case AST_STMT_IF:       bytecode_visit_stmt_if(bc, (ast_stmt_if_t*)node); break;
  case AST_STMT_FOR:      bytecode_visit_stmt_for(bc, (ast_stmt_for_t*)node); break;
  case AST_STMT_WHILE:    bytecode_visit_stmt_while(bc, (ast_stmt_while_t*)node); break;
  case AST_STMT_BREAK:    bytecode_visit_stmt_break(bc, (ast_stmt_break_t*)node); break;
  case AST_STMT_CONTINUE: bytecode_visit_stmt_continue(bc, (ast_stmt_continue_t*)node); break;
  case AST_STMT_RETURN:   bytecode_visit_stmt_return(bc, (ast_stmt_return_t*)node); break;
  case AST_STMT_YIELD:    bytecode_visit_stmt_yield(bc, (ast_stmt_yield_t*)node); break;
  case AST_STMT_BLOCK:    bytecode_visit_stmt_block(bc, (ast_stmt_block_t*)node); break;
  case AST_STMT_EXPR:     bytecode_visit_stmt_expr(bc, (ast_stmt_expr_t*)node); break;
  default: unreachable();
  }
}

void bytecode_visit_decl_var(bytecode_t* bc, ast_decl_var_t* node)
{
  bytecode_reg_var(bc, node);
  bc->sp += ast_desc_of((ast_node_t*)node)->size;

  if (node->init != NULL)
    bytecode_visit_expr(bc, (ast_expr_t*)node->init);
}

void bytecode_visit_decl_fun(bytecode_t* bc, ast_decl_fun_t* node)
{
  bytecode_reg_fun(bc, node);

  if (node->params != NULL)
  {
    int64_t param_offset = 0;

    LIST_FOR_LOOP(it, node->params)
      param_offset += ast_desc_of((ast_node_t*)list_node_get(it))->size;

    LIST_FOR_LOOP(it, node->params)
    {
      bytecode_reg_param(bc, (ast_param_t*)list_node_get(it), param_offset);
      param_offset -= ast_desc_of((ast_node_t*)list_node_get(it))->size;
    }
  }

  bc->sp = 0;
  bytecode_visit_stmt(bc, (ast_stmt_t*)node->stmt);
}

void bytecode_visit_decl_gen(bytecode_t* bc, ast_decl_gen_t* node)
{
  unused(bc);
  unused(node);
  unreachable();
}

void bytecode_visit_decl_mod(bytecode_t* bc, ast_decl_mod_t* node)
{
  LIST_FOR_LOOP(it, node->decls)
    bytecode_visit_decl(bc, (ast_decl_t*)list_node_get(it));
}

void bytecode_visit_decl_generic(bytecode_t* bc, ast_decl_generic_t* node)
{
  bytecode_visit_decl(bc, (ast_decl_t*)node->decl);
}

void bytecode_visit_decl(bytecode_t* bc, ast_decl_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:     bytecode_visit_decl_var(bc, (ast_decl_var_t*)node); break;
  case AST_DECL_FUN:     bytecode_visit_decl_fun(bc, (ast_decl_fun_t*)node); break;
  case AST_DECL_GEN:     bytecode_visit_decl_gen(bc, (ast_decl_gen_t*)node); break;
  case AST_DECL_STRUCT:
  case AST_DECL_UNION:
  case AST_DECL_ENUM:    break;
  case AST_DECL_MOD:     bytecode_visit_decl_mod(bc, (ast_decl_mod_t*)node); break;
  case AST_DECL_GENERIC: bytecode_visit_decl_generic(bc, (ast_decl_generic_t*)node); break;
  default: unreachable();
  }
}

void bytecode_visit_prog(bytecode_t* bc, ast_prog_t* node)
{
  LIST_FOR_LOOP(it, node->decls)
    bytecode_visit_decl(bc, (ast_decl_t*)list_node_get(it));

  bytecode_emit_opcode(bc, OPCODE_HALT);
}

void bytecode_dump(FILE* stream, bytecode_t* bc)
{
  uint8_t* ptr = bc->data;

  while (ptr < bc->data + bc->size)
  {
    opcode_t opcode;
    memcpy(&opcode, ptr, sizeof(opcode_t));
    ptr += sizeof(opcode_t);

    fprintf(stream, opcode_to_string(opcode));

    switch (opcode)
    {
    case OPCODE_PUSH:
    {
      uint8_t u8_value;
      memcpy(&u8_value, ptr, sizeof(uint8_t));
      fprintf(stream, " %02hhX", u8_value);
      ptr += sizeof(uint8_t);
      break;
    }
    case OPCODE_PUSH2:
    {
      uint16_t u16_value;
      memcpy(&u16_value, ptr, sizeof(uint16_t));
      fprintf(stream, " %04hX", u16_value);
      ptr += sizeof(uint16_t);
      break;
    }
    case OPCODE_PUSH4:
    {
      uint32_t u32_value;
      memcpy(&u32_value, ptr, sizeof(uint32_t));
      fprintf(stream, " %08X", u32_value);
      ptr += sizeof(uint32_t);
      break;
    }
    case OPCODE_PUSH8:
    {
      uint64_t u64_value;
      memcpy(&u64_value, ptr, sizeof(uint64_t));
      fprintf(stream, " %016llX", u64_value);
      ptr += sizeof(uint64_t);
      break;
    }
    case OPCODE_INVOKE:
    {
      uint64_t addr_value;
      memcpy(&addr_value, ptr, sizeof(uint64_t));
      fprintf(stream, " %016llX", addr_value);
      ptr += sizeof(uint64_t);
      break;
    }
    default:
      fallthrough();
    }

    fputc('\n', stream);
  }
}
