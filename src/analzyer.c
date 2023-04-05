#include "analzyer.h"

#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "crumb.h"
#include "list.h"
#include "token.h"
#include "op.h"
#include "ast.h"
#include "symtable.h"
#include "diagnostics.h"
#include "memtrace.h"

analyzer_t* analyzer_init(void)
{
  analyzer_t* analyzer = (analyzer_t*)malloc(sizeof(analyzer_t));
  assert(analyzer != NULL);
  analyzer->root = symtable_init(NULL);
  analyzer->ret_types = stack_init();
  return analyzer;
}

void analyzer_free(analyzer_t* analyzer)
{
  symtable_free(analyzer->root);
  stack_free(analyzer->ret_types);
  free(analyzer);
}

type_t* analyzer_visit_expr_op_is(analyzer_t* analyzer, symtable_t* table, ast_expr_op_bin_t* node)
{
  type_t* type_lhs = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->lhs);
  type_t* type_rhs = type_of(node->rhs);

  return type_builtin(TYPE_BOOL);
}

type_t* analyzer_visit_expr_op_as(analyzer_t* analyzer, symtable_t* table, ast_expr_op_bin_t* node)
{
  type_t* type_lhs = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->lhs);
  type_t* type_rhs = type_of(node->rhs);
  
  // TODO: check convertability

  return type_rhs;
}

type_t* analyzer_visit_expr_op_unary(analyzer_t* analyzer, symtable_t* table, ast_expr_op_un_t* node)
{
  type_t* arg_type = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->param);

  switch (node->op_kind)
  {
  case OP_SIZEOF:
    return type_builtin(TYPE_USIZE);
  case OP_ALIGNOF:
    return type_builtin(TYPE_USIZE);
  case OP_TYPEOF:
    return type_builtin(TYPE_TYPE);
  case OP_ARIT_INC_PRE:
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_PRE:
  case OP_ARIT_DEC_POST:
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_BIT_NOT:
    if (!type_is_arithmetic(arg_type->kind) && arg_type->kind != TYPE_PTR)
      report_error_expected_arithmetic_type(node->tok->loc);
    return arg_type;
  case OP_LOGIC_NOT:
    if (arg_type->kind != TYPE_BOOL)
      report_error_expected_bool_type(node->tok->loc);
    return arg_type;
  case OP_IND:
    if (arg_type->kind != TYPE_PTR)
      report_error_expected_ptr_type(node->tok->loc);
    return arg_type->type_ptr.base_type;
  case OP_ADDR:
    type_t* ptr_type = type_init(TYPE_PTR);
    ptr_type->type_ptr.base_type = arg_type;
    return ptr_type;
  default:
    unreachable();
  }

  return NULL;
}

type_t* analyzer_visit_expr_op_binary(analyzer_t* analyzer, symtable_t* table, ast_expr_op_bin_t* node)
{
  if (node->op_kind == OP_IS)
    return analyzer_visit_expr_op_is(analyzer, table, (ast_expr_op_bin_t*)node);
  else if (node->op_kind == OP_AS)
    return analyzer_visit_expr_op_as(analyzer, table, (ast_expr_op_bin_t*)node);

  type_t* type_lhs = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->lhs);
  type_t* type_rhs = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->rhs);

  switch (node->op_kind)
  {
  case OP_IS:
  case OP_AS:
  case OP_IN:
    return type_builtin(TYPE_BOOL);
  case OP_ARIT_ADD:
  case OP_ARIT_SUB:
  case OP_ARIT_MUL:
  case OP_ARIT_DIV:
  case OP_ARIT_MOD:
  case OP_BIT_AND:
  case OP_BIT_OR:
  case OP_BIT_XOR:
    if (type_is_signed(type_lhs->kind) != type_is_signed(type_rhs->kind))
      report_error_mixed_signedness(node->tok->loc);

    return type_wider(type_lhs, type_rhs);
  case OP_BIT_LSH:
  case OP_BIT_RSH:
    if (!type_is_integer(type_lhs->kind))
      report_error_expected_integer_type(node->lhs->tok->loc);

    if (!type_is_integer(type_rhs->kind))
      report_error_expected_integer_type(node->rhs->tok->loc);

    return type_lhs;
  case OP_LOGIC_AND:
  case OP_LOGIC_OR:
    if (type_lhs->kind != TYPE_BOOL)
      report_error_expected_bool_type(node->lhs->tok->loc);

    if (type_rhs->kind != TYPE_BOOL)
      report_error_expected_bool_type(node->rhs->tok->loc);

    return type_builtin(TYPE_BOOL);
  case OP_COMP_EQ:
  case OP_COMP_NE:
  case OP_COMP_LT:
  case OP_COMP_LE:
  case OP_COMP_GT:
  case OP_COMP_GE:
    return type_builtin(TYPE_BOOL);
  case OP_SUBS:
    if (type_lhs->kind != TYPE_ARRAY)
      report_error_expected_subscriptable(node->lhs->tok->loc);

    if (!type_is_integer(type_rhs->kind))
      report_error_expected_integer_type(node->rhs->tok->loc);

    return type_lhs->type_array.base_type;
  case OP_ASSIGN:
  case OP_ARIT_ADD_ASSIGN:
  case OP_ARIT_SUB_ASSIGN:
  case OP_ARIT_MUL_ASSIGN:
  case OP_ARIT_DIV_ASSIGN:
  case OP_ARIT_MOD_ASSIGN:
  case OP_BIT_AND_ASSIGN:
  case OP_BIT_OR_ASSIGN:
  case OP_BIT_XOR_ASSIGN:
  case OP_BIT_LSH_ASSIGN:
  case OP_BIT_RSH_ASSIGN:
  case OP_RANGE:
  case OP_SEMICOLON:
    // TODO
  default:
    unreachable();
  }

  return NULL;
}

type_t* analyzer_visit_expr_op_call(analyzer_t* analyzer, symtable_t* table, ast_expr_op_call_t* node)
{
  type_t* callee_type = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->callee);
  list_t* callee_param_types = NULL;
  type_t* return_type = NULL;

  if (callee_type->kind == TYPE_FUN)
  {
    if (list_size(node->params) > list_size(callee_type->type_fun.param_types))
      report_error_too_many_arguments(node->tok->loc);
    else if (list_size(node->params) < list_size(callee_type->type_fun.param_types))
      report_error_too_few_arguments(node->tok->loc);

    callee_param_types = callee_type->type_fun.param_types;
    return_type = callee_type->type_fun.return_type;
  }
  else if (callee_type->kind == TYPE_GEN)
  {
    if (list_size(node->params) > list_size(callee_type->type_gen.param_types))
      report_error_too_many_arguments(node->tok->loc);
    else if (list_size(node->params) < list_size(callee_type->type_gen.param_types))
      report_error_too_few_arguments(node->tok->loc);

    callee_param_types = callee_type->type_gen.param_types;
    return_type = callee_type->type_gen.yield_type;
  }
  else
    report_error_expected_callable(node->tok->loc);

  for (list_node_t *param_elem      = list_front_node(node->params),
                   *param_type_elem = list_front_node(callee_param_types);
        param_elem != NULL && param_type_elem != NULL;
        param_elem      = list_node_next(param_elem),
        param_type_elem = list_node_next(param_type_elem))
  {
    ast_node_t* param = (ast_node_t*)list_node_get(param_elem);
    type_t* type = analyzer_visit_expr(analyzer, table, (ast_expr_t*)param);
    type_t* param_type = (type_t*)list_node_get(param_type_elem);

    if (!type_is_same(type, param_type))
      report_error_incompatible_param_type(param->tok->loc);
  }

  return return_type;
}

type_t* analyzer_visit_expr_op_member(analyzer_t* analyzer, symtable_t* table, ast_expr_op_bin_t* node)
{
  if (node->rhs->kind != AST_ID)
    report_error_expected_member(node->rhs->tok->loc);

  type_t* type_owner = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->lhs);

  switch (node->op_kind)
  {
  case OP_MEMBER:
    if (!type_is_owner(type_owner->kind))
      report_error_expected_owner(node->lhs->tok->loc);
    break;
  case OP_IND_MEMBER:
    if (type_owner->kind != TYPE_PTR)
      report_error_expected_ptr_type(node->lhs->tok->loc);

    if (!type_is_owner(type_owner->type_ptr.base_type->kind))
      report_error_expected_ptr_to_owner(node->lhs->tok->loc);

    type_owner = type_owner->type_ptr.base_type;
    break;
  case OP_NULL_SAFE_MEMBER:
    if (type_owner->kind != TYPE_OPT)
      report_error_expected_optional_type(node->lhs->tok->loc);

    if (!type_is_owner(type_owner->type_opt.base_type->kind))
      report_error_expected_owner(node->lhs->tok->loc);

    type_owner = type_owner->type_opt.base_type;
    break;
  default:
    unreachable();
  }

  token_id_t* tok_rhs = (token_id_t*)node->rhs->tok;
  
  switch (type_owner->kind)
  {
  case TYPE_STRUCT:
    LIST_FOR_LOOP(elem, type_owner->type_struct.members)
    {
      type_member_t* member_type = (type_member_t*)list_node_get(elem);
      
      if (strcmp(member_type->id, tok_rhs->value) == 0)
        return member_type->type;
    }

    report_error_no_member_with_name(tok_rhs->loc);
    break;
  case TYPE_UNION:
    LIST_FOR_LOOP(elem, type_owner->type_union.members)
    {
      type_member_t* member_type = (type_member_t*)list_node_get(elem);
      
      if (strcmp(member_type->id, tok_rhs->value) == 0)
        return member_type->type;
    }

    report_error_no_member_with_name(tok_rhs->loc);
    break;
  case TYPE_ENUM:
    LIST_FOR_LOOP(elem, type_owner->type_enum.values)
    {
      type_member_t* value_type = (type_member_t*)list_node_get(elem);
      
      if (strcmp(value_type->id, tok_rhs->value) == 0)
        return type_owner;
    }

    report_error_no_member_with_name(tok_rhs->loc);
    break;
  case TYPE_MOD:
    LIST_FOR_LOOP(elem, type_owner->type_mod.decls)
    {
      type_member_t* decl_type = (type_member_t*)list_node_get(elem);
      
      if (strcmp(decl_type->id, tok_rhs->value) == 0)
        return decl_type->type;
    }

    report_error_no_member_with_name(tok_rhs->loc);
    break;
  default:
    report_error_expected_owner(tok_rhs->loc);
  }

  return NULL;
}

type_t* analyzer_visit_expr_op(analyzer_t* analyzer, symtable_t* table, ast_expr_op_t* node)
{
  if (node->op_kind == OP_CALL)
    return analyzer_visit_expr_op_call(analyzer, table, (ast_expr_op_call_t*)node);
  
  if (node->op_kind == OP_MEMBER ||
      node->op_kind == OP_IND_MEMBER ||
      node->op_kind == OP_NULL_SAFE_MEMBER)
    return analyzer_visit_expr_op_member(analyzer, table, (ast_expr_op_bin_t*)node);
  
  if (op_is_unary(node->op_kind))
    return analyzer_visit_expr_op_unary(analyzer, table, (ast_expr_op_un_t*)node);
  
  if (op_is_binary(node->op_kind))
    return analyzer_visit_expr_op_binary(analyzer, table, (ast_expr_op_bin_t*)node);
  
  unreachable();

  return NULL;
}

type_t* analyzer_visit_expr(analyzer_t* analyzer, symtable_t* table, ast_expr_t* node)
{
  type_t* type = NULL;

  switch (node->kind)
  {
  case AST_ID:
    token_id_t* tok_id = (token_id_t*)node->tok;
    symbol_t* id_sym = symtable_lookup(table, tok_id->value);
    
    if (id_sym == NULL)
      report_error_undefined_symbol(tok_id->loc);

    switch (id_sym->node->kind)
    {
    case AST_DECL_VAR:
    case AST_DECL_LOOP_VAR:
    case AST_DECL_FUN:
    case AST_DECL_GEN:
    case AST_DECL_STRUCT:
    case AST_DECL_UNION:
    case AST_DECL_ENUM:
    case AST_DECL_MOD:
    case AST_PARAM:
    case AST_PARAM_VARIADIC:
    case AST_PARAM_GENERIC:
      break;
    default:
      report_error_symbol_is_not_an_expression(tok_id->loc);
    }
    
    return type_of(id_sym->node);
  case AST_EXPR_LIT_INT:
    return type_builtin(TYPE_I32);
  case AST_EXPR_LIT_FLT:
    return type_builtin(TYPE_F32);
  case AST_EXPR_LIT_STR:
    return type_builtin(TYPE_STR);
  case AST_EXPR_LIT_CHAR:
    return type_builtin(TYPE_U8);
  case AST_EXPR_LIT_BOOL:
    return type_builtin(TYPE_BOOL);
  case AST_EXPR_LIT_NULL:
    return type_builtin(TYPE_NULL);
    break;
  case AST_EXPR_OP:
    return analyzer_visit_expr_op(analyzer, table, (ast_expr_op_t*)node);
  default:
    unreachable();
  }

  return type;
}

ast_node_t* analyzer_visit_type(analyzer_t* analyzer, symtable_t* table, ast_type_t* node)
{
  switch (node->kind)
  {
  case AST_ID:
    token_id_t* tok_id = (token_id_t*)node->tok;
    symbol_t* id_sym = symtable_lookup(table, tok_id->value);
    
    if (id_sym == NULL)
      report_error_undefined_typename(tok_id->loc);

    if (id_sym->node->kind != AST_DECL_STRUCT &&
        id_sym->node->kind != AST_DECL_UNION &&
        id_sym->node->kind != AST_DECL_ENUM &&
        id_sym->node->kind != AST_PARAM_GENERIC)
      report_error_symbol_is_not_a_typename(tok_id->loc);

    if (id_sym->node->kind == AST_PARAM_GENERIC &&
        ((ast_param_generic_t*)id_sym->node)->type->kind != AST_TYPE_TYPE)
      report_error_symbol_is_not_a_typename(tok_id->loc);

    ast_node_free((ast_node_t*)node);

    return id_sym->node;
  case AST_TYPE_MUT:
    ((ast_type_mut_t*)node)->base_type = analyzer_visit_type(analyzer, table, (ast_type_t*)((ast_type_mut_t*)node)->base_type);
    break;
  case AST_TYPE_CONST:
    ((ast_type_const_t*)node)->base_type = analyzer_visit_type(analyzer, table, (ast_type_t*)((ast_type_const_t*)node)->base_type);
    break;
  case AST_TYPE_PTR:
    ((ast_type_ptr_t*)node)->base_type = analyzer_visit_type(analyzer, table, (ast_type_t*)((ast_type_ptr_t*)node)->base_type);
    break;
  case AST_TYPE_ARRAY:
    ((ast_type_array_t*)node)->base_type = analyzer_visit_type(analyzer, table, (ast_type_t*)((ast_type_array_t*)node)->base_type);
    if (((ast_type_array_t*)node)->size != NULL)
      analyzer_visit_expr(analyzer, table, (ast_expr_t*)((ast_type_array_t*)node)->size);
    break;
  case AST_TYPE_REF:
    ((ast_type_ref_t*)node)->base_type = analyzer_visit_type(analyzer, table, (ast_type_t*)((ast_type_ref_t*)node)->base_type);
    break;
  case AST_TYPE_OPT:
    ((ast_type_opt_t*)node)->base_type = analyzer_visit_type(analyzer, table, (ast_type_t*)((ast_type_opt_t*)node)->base_type);
    break;
  case AST_TYPE_FUN:
    ((ast_type_fun_t*)node)->return_type = analyzer_visit_type(analyzer, table, (ast_type_t*)((ast_type_fun_t*)node)->return_type);
    LIST_FOR_LOOP(elem, ((ast_type_fun_t*)node)->params)
      list_node_set(elem, analyzer_visit_type(analyzer, table, (ast_type_t*)list_node_get(elem)));
    break;
  case AST_TYPE_GEN:
    ((ast_type_gen_t*)node)->yield_type = analyzer_visit_type(analyzer, table, (ast_type_t*)((ast_type_gen_t*)node)->yield_type);
    LIST_FOR_LOOP(elem, ((ast_type_gen_t*)node)->params)
      list_node_set(elem, analyzer_visit_type(analyzer, table, (ast_type_t*)list_node_get(elem)));
    break;
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
  default:
    unreachable();
  }

  return (ast_node_t*)node;
}

void analyzer_visit_stmt_if(analyzer_t* analyzer, symtable_t* table, ast_stmt_if_t* node)
{
  symtable_t* if_table = symtable_init(table);

  analyzer_visit_expr(analyzer, if_table, (ast_expr_t*)node->cond);
  analyzer_visit_stmt(analyzer, if_table, (ast_stmt_t*)node->stmt);

  if (node->stmt_else != NULL)
    analyzer_visit_stmt(analyzer, if_table, (ast_stmt_t*)node->stmt_else);
}

void analyzer_visit_stmt_for(analyzer_t* analyzer, symtable_t* table, ast_stmt_for_t* node)
{
  symtable_t* for_table = symtable_init(table);

  analyzer_visit_decl_loop_var(analyzer, for_table, (ast_decl_loop_var_t*)node->var);
  analyzer_visit_expr(analyzer, for_table, (ast_expr_t*)node->range);
  analyzer_visit_stmt(analyzer, for_table, (ast_stmt_t*)node->stmt);
}

void analyzer_visit_stmt_while(analyzer_t* analyzer, symtable_t* table, ast_stmt_while_t* node)
{
  symtable_t* while_table = symtable_init(table);

  analyzer_visit_expr(analyzer, while_table, (ast_expr_t*)node->cond);
  analyzer_visit_stmt(analyzer, while_table, (ast_stmt_t*)node->stmt);
}

void analyzer_visit_stmt_break(analyzer_t* analyzer, symtable_t* table, ast_stmt_break_t* node)
{
}

void analyzer_visit_stmt_continue(analyzer_t* analyzer, symtable_t* table, ast_stmt_continue_t* node)
{
}

type_t* analyzer_visit_stmt_return(analyzer_t* analyzer, symtable_t* table, ast_stmt_return_t* node)
{
  type_t* type = type_builtin(TYPE_UNIT);

  if (node->expr != NULL)
    type = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->expr);

  type_t* expected_type = stack_top(analyzer->ret_types);

  if (!type_is_same(type, expected_type))
    report_error_incompatible_return_type(node->tok->loc);

  return type;
}

type_t* analyzer_visit_stmt_yield(analyzer_t* analyzer, symtable_t* table, ast_stmt_yield_t* node)
{
  type_t* type = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->expr);
  type_t* expected_type = stack_top(analyzer->ret_types);
  
  if (!type_is_same(type, expected_type))
    report_error_incompatible_return_type(node->tok->loc);

  return type;
}

void analyzer_visit_stmt_block(analyzer_t* analyzer, symtable_t* table, ast_stmt_block_t* node)
{
  symtable_t* block_table = symtable_init(table);

  LIST_FOR_LOOP(elem, node->stmts)
    analyzer_visit_stmt(analyzer, block_table, (ast_stmt_t*)list_node_get(elem));
}

type_t* analyzer_visit_stmt_expr(analyzer_t* analyzer, symtable_t* table, ast_stmt_expr_t* node)
{
  return analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->expr);
}

void analyzer_visit_stmt(analyzer_t* analyzer, symtable_t* table, ast_stmt_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:      analyzer_visit_decl_var(analyzer, table, (ast_decl_var_t*)node); break;
  case AST_STMT_IF:       analyzer_visit_stmt_if(analyzer, table, (ast_stmt_if_t*)node); break;
  case AST_STMT_FOR:      analyzer_visit_stmt_for(analyzer, table, (ast_stmt_for_t*)node); break;
  case AST_STMT_WHILE:    analyzer_visit_stmt_while(analyzer, table, (ast_stmt_while_t*)node); break;
  case AST_STMT_BREAK:    analyzer_visit_stmt_break(analyzer, table, (ast_stmt_break_t*)node); break;
  case AST_STMT_CONTINUE: analyzer_visit_stmt_continue(analyzer, table, (ast_stmt_continue_t*)node); break;
  case AST_STMT_RETURN:   analyzer_visit_stmt_return(analyzer, table, (ast_stmt_return_t*)node); break;
  case AST_STMT_YIELD:    analyzer_visit_stmt_yield(analyzer, table, (ast_stmt_yield_t*)node); break;
  case AST_STMT_BLOCK:    analyzer_visit_stmt_block(analyzer, table, (ast_stmt_block_t*)node); break;
  case AST_STMT_EXPR:     analyzer_visit_stmt_expr(analyzer, table, (ast_stmt_expr_t*)node); break;
  default: unreachable();
  }
}

void analyzer_visit_decl_var(analyzer_t* analyzer, symtable_t* table, ast_decl_var_t* node)
{
  node->type = analyzer_visit_type(analyzer, table, (ast_type_t*)node->type);

  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* var_sym = symbol_init(tok_id->value, (ast_node_t*)node, type_of((ast_node_t*)node->type));
  symbol_t* lookup = symtable_lookup(table, tok_id->value);
  symbol_t* collision = symtable_insert(table, var_sym);

  if (collision != NULL && collision->node->kind == AST_DECL_VAR)
    report_error_variable_redeclaration(tok_id->loc);

  if (lookup != NULL && lookup->node->kind == AST_DECL_VAR)
    report_warning_shadowed_variable(tok_id->loc);

  if (node->init != NULL)
  {
    type_t* init_type = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->init);

    if (!type_is_same(init_type, var_sym->type))
      report_error_type_mismatch(node->tok->loc, var_sym->type, init_type);
  }
}

void analyzer_visit_decl_loop_var(analyzer_t* analyzer, symtable_t* table, ast_decl_loop_var_t* node)
{
  node->type = analyzer_visit_type(analyzer, table, (ast_type_t*)node->type);

  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* var_sym = symbol_init(tok_id->value, (ast_node_t*)node, type_of((ast_node_t*)node));
  symbol_t* lookup = symtable_lookup(table, tok_id->value);
  symbol_t* collision = symtable_insert(table, var_sym);

  assert(collision == NULL);

  if (lookup != NULL)
    report_warning_shadowed_variable(tok_id->loc);
}

void analyzer_visit_decl_fun(analyzer_t* analyzer, symtable_t* table, ast_decl_fun_t* node)
{
  symtable_t* fun_table = symtable_init(table);

  if (node->params != NULL)
    LIST_FOR_LOOP(elem, node->params)
      analyzer_visit_param(analyzer, fun_table, (ast_param_t*)list_node_get(elem));

  node->return_type = analyzer_visit_type(analyzer, fun_table, (ast_type_t*)node->return_type);

  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* fun_sym = symbol_init(tok_id->value, (ast_node_t*)node, type_of((ast_node_t*)node));
  fun_sym->scope = fun_table;

  symbol_t* collision = symtable_insert(table, fun_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  stack_push(analyzer->ret_types, fun_sym->type->type_fun.return_type);
  analyzer_visit_stmt(analyzer, fun_table, (ast_stmt_t*)node->stmt);
  stack_pop(analyzer->ret_types);
}

void analyzer_visit_decl_gen(analyzer_t* analyzer, symtable_t* table, ast_decl_gen_t* node)
{
  symtable_t* gen_table = symtable_init(table);

  if (node->params != NULL)
    LIST_FOR_LOOP(elem, node->params)
      analyzer_visit_param(analyzer, gen_table, (ast_param_t*)list_node_get(elem));

  node->yield_type = analyzer_visit_type(analyzer, gen_table, (ast_type_t*)node->yield_type);

  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* gen_sym = symbol_init(tok_id->value, (ast_node_t*)node, type_of((ast_node_t*)node));
  gen_sym->scope = gen_table;

  symbol_t* collision = symtable_insert(table, gen_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);
  
  stack_push(analyzer->ret_types, gen_sym->type->type_gen.yield_type);
  analyzer_visit_stmt(analyzer, gen_table, (ast_stmt_t*)node->stmt);
  stack_pop(analyzer->ret_types);
}

void analyzer_visit_decl_struct(analyzer_t* analyzer, symtable_t* table, ast_decl_struct_t* node)
{
  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* struct_sym = symbol_init(tok_id->value, (ast_node_t*)node, type_of((ast_node_t*)node));
  symbol_t* lookup = symtable_lookup(table, tok_id->value);
  symbol_t* collision = symtable_insert(table, struct_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* struct_table = symtable_init(table);
  struct_sym->scope = struct_table;

  // TODO: Members cannot have initializers
  LIST_FOR_LOOP(elem, node->members)
    analyzer_visit_decl_var(analyzer, struct_table, (ast_decl_var_t*)list_node_get(elem));
}

void analyzer_visit_decl_union(analyzer_t* analyzer, symtable_t* table, ast_decl_union_t* node)
{
  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* union_sym = symbol_init(tok_id->value, (ast_node_t*)node, type_of((ast_node_t*)node));
  symbol_t* lookup = symtable_lookup(table, tok_id->value);
  symbol_t* collision = symtable_insert(table, union_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* union_table = symtable_init(table);
  union_sym->scope = union_table;

  // TODO: Members cannot have initializers
  LIST_FOR_LOOP(elem, node->members)
    analyzer_visit_decl_var(analyzer, union_table, (ast_decl_var_t*)list_node_get(elem));
}

void analyzer_visit_decl_enum(analyzer_t* analyzer, symtable_t* table, ast_decl_enum_t* node)
{
  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* enum_sym = symbol_init(tok_id->value, (ast_node_t*)node, type_of((ast_node_t*)node));
  symbol_t* lookup = symtable_lookup(table, tok_id->value);
  symbol_t* collision = symtable_insert(table, enum_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* enum_table = symtable_init(table);
  enum_sym->scope = enum_table;

  LIST_FOR_LOOP(elem, node->values)
    analyzer_visit_enumerator(analyzer, enum_table, enum_sym, (ast_enumerator_t*)list_node_get(elem));
}

void analyzer_visit_decl_mod(analyzer_t* analyzer, symtable_t* table, ast_decl_mod_t* node)
{
  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* mod_sym = symbol_init(tok_id->value, (ast_node_t*)node, type_of((ast_node_t*)node));
  symbol_t* collision = symtable_insert(table, mod_sym);

  symtable_t* mod_table = NULL;
  
  if (collision != NULL)
  {
    mod_table = collision->scope;
    symbol_free(mod_sym);
  }
  else
  {
    mod_table = symtable_init(table);
    mod_sym->scope = mod_table;
  }

  LIST_FOR_LOOP(elem, node->decls)
    analyzer_visit_decl(analyzer, mod_table, (ast_decl_t*)list_node_get(elem));
}

void analyzer_visit_decl_generic(analyzer_t* analyzer, symtable_t* table, ast_decl_generic_t* node)
{
  symtable_t* generic_table = symtable_init(table);

  if (node->params != NULL)
    LIST_FOR_LOOP(elem, node->params)
      analyzer_visit_param_generic(analyzer, generic_table, (ast_param_generic_t*)list_node_get(elem));

  analyzer_visit_decl(analyzer, generic_table, (ast_decl_t*)node->decl);
}

void analyzer_visit_decl(analyzer_t* analyzer, symtable_t* table, ast_decl_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:     analyzer_visit_decl_var(analyzer, table, (ast_decl_var_t*)node); break;
  case AST_DECL_FUN:     analyzer_visit_decl_fun(analyzer, table, (ast_decl_fun_t*)node); break;
  case AST_DECL_GEN:     analyzer_visit_decl_gen(analyzer, table, (ast_decl_gen_t*)node); break;
  case AST_DECL_STRUCT:  analyzer_visit_decl_struct(analyzer, table, (ast_decl_struct_t*)node); break;
  case AST_DECL_UNION:   analyzer_visit_decl_union(analyzer, table, (ast_decl_union_t*)node); break;
  case AST_DECL_ENUM:    analyzer_visit_decl_enum(analyzer, table, (ast_decl_enum_t*)node); break;
  case AST_DECL_MOD:     analyzer_visit_decl_mod(analyzer, table, (ast_decl_mod_t*)node); break;
  case AST_DECL_GENERIC: analyzer_visit_decl_generic(analyzer, table, (ast_decl_generic_t*)node); break;
  default: unreachable();
  }
}

void analyzer_visit_param(analyzer_t* analyzer, symtable_t* table, ast_param_t* node)
{
  node->type = analyzer_visit_type(analyzer, table, (ast_type_t*)node->type);

  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* param_sym = symbol_init(tok_id->value, (ast_node_t*)node, type_of((ast_node_t*)node));
  symbol_t* collision = symtable_insert(table, param_sym);

  if (collision != NULL && ast_is_param(collision->node))
    report_error_parameter_redefinition(tok_id->loc);

  switch (node->kind)
  {
  case AST_PARAM_DEFAULT: analyzer_visit_param_default(analyzer, table, (ast_param_default_t*)node); return;
  case AST_PARAM_VARIADIC: analyzer_visit_param_variadic(analyzer, table, (ast_param_variadic_t*)node); return;
  case AST_PARAM_GENERIC: analyzer_visit_param_generic(analyzer, table, (ast_param_generic_t*)node); return;
  }
}

void analyzer_visit_param_default(analyzer_t* analyzer, symtable_t* table, ast_param_default_t* node)
{
  if (node->init != NULL)
    analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->init);
}

void analyzer_visit_param_variadic(analyzer_t* analyzer, symtable_t* table, ast_param_variadic_t* node)
{
}

void analyzer_visit_param_generic(analyzer_t* analyzer, symtable_t* table, ast_param_generic_t* node)
{
  if (node->type->kind != AST_TYPE_TYPE)
    node->type = analyzer_visit_type(analyzer, table, (ast_type_t*)node->type);

  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* param_sym = symbol_init(tok_id->value, (ast_node_t*)node, type_of((ast_node_t*)node));
  symbol_t* collision = symtable_insert(table, param_sym);

  if (collision != NULL && ast_is_param(collision->node))
    report_error_parameter_redefinition(tok_id->loc);
}

void analyzer_visit_enumerator(analyzer_t* analyzer, symtable_t* table, symbol_t* enum_sym, ast_enumerator_t* node)
{
  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* enumerator_sym = symbol_init(tok_id->value, (ast_node_t*)node, enum_sym->type);
  symbol_t* collision = symtable_insert(table, enumerator_sym);

  if (collision != NULL)
    report_error_enumerator_redeclaration(tok_id->loc);
}

void analyzer_visit_prog(analyzer_t* analyzer, symtable_t* table, ast_prog_t* node)
{
  symtable_t* prog_table = symtable_init(table);

  LIST_FOR_LOOP(elem, node->decls)
    analyzer_visit_decl(analyzer, prog_table, (ast_decl_t*)list_node_get(elem));
}

symtable_t* analyzer_analyze(analyzer_t* analyzer, ast_node_t* root)
{
  assert(root->kind == AST_PROG);
  analyzer_visit_prog(analyzer, analyzer->root, (ast_prog_t*)root);
  return analyzer->root;
}
