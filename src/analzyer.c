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

void analyzer_visit_param(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  if (node->kind == AST_VARIADIC_PARAM)
  {
    analyzer_visit_variadic_param(analyzer, table, node);
    return;
  }

  node->param.type = analyzer_visit_type(analyzer, table, node->param.type);

  symbol_t* param_sym = symbol_init(node->param.id->tok->id.value, node, type_of(node));
  symbol_t* collision = symtable_insert(table, param_sym);

  if (collision != NULL)
    switch (collision->node->kind)
    {
    case AST_PARAM:          report_error_parameter_redefinition(node->param.id->tok->loc);
    case AST_GENERIC_PARAM:  report_error_parameter_redefinition(node->generic_param.id->tok->loc);
    case AST_VARIADIC_PARAM: report_error_parameter_redefinition(node->variadic_param.id->tok->loc);
    }

  if (node->param.init != NULL)
    analyzer_visit_expr(analyzer, table, node->param.init);
}

void analyzer_visit_variadic_param(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  node->variadic_param.type = analyzer_visit_type(analyzer, table, node->variadic_param.type);

  symbol_t* param_sym = symbol_init(node->generic_param.id->tok->id.value, node, type_of(node));
  symbol_t* collision = symtable_insert(table, param_sym);

  if (collision != NULL)
    switch (collision->node->kind)
    {
    case AST_PARAM:          report_error_parameter_redefinition(node->param.id->tok->loc);
    case AST_GENERIC_PARAM:  report_error_parameter_redefinition(node->generic_param.id->tok->loc);
    case AST_VARIADIC_PARAM: report_error_parameter_redefinition(node->variadic_param.id->tok->loc);
    }
}

void analyzer_visit_generic_param(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  if (node->generic_param.type->kind != AST_TYPE_TYPE)
    node->generic_param.type = analyzer_visit_type(analyzer, table, node->param.type);

  symbol_t* param_sym = symbol_init(node->generic_param.id->tok->id.value, node, type_of(node));
  symbol_t* collision = symtable_insert(table, param_sym);

  if (collision != NULL && collision->node->kind == AST_PARAM)
    report_error_parameter_redefinition(node->generic_param.id->tok->loc);
}

void analyzer_visit_loop_var(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  node->loop_var.type = analyzer_visit_type(analyzer, table, node->loop_var.type);

  symbol_t* var_sym = symbol_init(node->loop_var.id->tok->id.value, node, type_of(node));
  symbol_t* lookup = symtable_lookup(table, node->loop_var.id->tok->id.value);
  symbol_t* collision = symtable_insert(table, var_sym);

  assert(collision == NULL);

  if (lookup != NULL)
    report_warning_shadowed_variable(node->loop_var.id->tok->loc);
}

void analyzer_visit_enumerator(analyzer_t* analyzer, symtable_t* table, symbol_t* enum_sym, ast_node_t* node)
{
  symbol_t* enumerator_sym = symbol_init(node->enumerator.id->tok->id.value, node, enum_sym->type);
  symbol_t* collision = symtable_insert(table, enumerator_sym);

  if (collision != NULL)
    report_error_enumerator_redeclaration(node->enumerator.id->tok->loc);
}

type_t* analyzer_visit_expr_op_is(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  type_t* type_lhs = analyzer_visit_expr(analyzer, table, node->expr_op.op_binary.lhs);
  type_t* type_rhs = type_of(node->expr_op.op_binary.rhs);

  return type_builtin(TYPE_BOOL);
}

type_t* analyzer_visit_expr_op_as(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  type_t* type_lhs = analyzer_visit_expr(analyzer, table, node->expr_op.op_binary.lhs);
  type_t* type_rhs = type_of(node->expr_op.op_binary.rhs);
  
  // TODO: check convertability

  return type_rhs;
}

type_t* analyzer_visit_expr_op_unary(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  type_t* arg_type = analyzer_visit_expr(analyzer, table, node->expr_op.op_unary.arg);

  switch (node->expr_op.kind)
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

type_t* analyzer_visit_expr_op_binary(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  if (node->expr_op.kind == OP_IS)
    return analyzer_visit_expr_op_is(analyzer, table, node);
  else if (node->expr_op.kind == OP_AS)
    return analyzer_visit_expr_op_as(analyzer, table, node);

  type_t* type_lhs = analyzer_visit_expr(analyzer, table, node->expr_op.op_binary.lhs);
  type_t* type_rhs = analyzer_visit_expr(analyzer, table, node->expr_op.op_binary.rhs);

  switch (node->expr_op.kind)
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
      report_error_expected_integer_type(node->expr_op.op_binary.lhs->tok->loc);

    if (!type_is_integer(type_rhs->kind))
      report_error_expected_integer_type(node->expr_op.op_binary.rhs->tok->loc);

    return type_lhs;
  case OP_LOGIC_AND:
  case OP_LOGIC_OR:
    if (type_lhs->kind != TYPE_BOOL)
      report_error_expected_bool_type(node->expr_op.op_binary.lhs->tok->loc);

    if (type_rhs->kind != TYPE_BOOL)
      report_error_expected_bool_type(node->expr_op.op_binary.rhs->tok->loc);

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
      report_error_expected_subscriptable(node->expr_op.op_binary.lhs->tok->loc);

    if (!type_is_integer(type_rhs->kind))
      report_error_expected_integer_type(node->expr_op.op_binary.rhs->tok->loc);

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

type_t* analyzer_visit_expr_op_call(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  type_t* callee_type = analyzer_visit_expr(analyzer, table, node->expr_op.op_call.callee);
  list_t* callee_param_types = NULL;
  type_t* ret_type = NULL;

  if (callee_type->kind == TYPE_FUN)
  {
    if (list_size(node->expr_op.op_call.args) > list_size(callee_type->type_fun.param_types))
      report_error_too_many_arguments(node->tok->loc);
    else if (list_size(node->expr_op.op_call.args) < list_size(callee_type->type_fun.param_types))
      report_error_too_few_arguments(node->tok->loc);

    callee_param_types = callee_type->type_fun.param_types;
    ret_type = callee_type->type_fun.ret_type;
  }
  else if (callee_type->kind == TYPE_GEN)
  {
    if (list_size(node->expr_op.op_call.args) > list_size(callee_type->type_gen.param_types))
      report_error_too_many_arguments(node->tok->loc);
    else if (list_size(node->expr_op.op_call.args) < list_size(callee_type->type_gen.param_types))
      report_error_too_few_arguments(node->tok->loc);

    callee_param_types = callee_type->type_gen.param_types;
    ret_type = callee_type->type_gen.ret_type;
  }
  else
    report_error_expected_callable(node->tok->loc);

  for (list_elem_t *param_elem      = list_front_elem(node->expr_op.op_call.args),
                    *param_type_elem = list_front_elem(callee_param_types);
        param_elem != NULL && param_type_elem != NULL;
        param_elem      = list_elem_next(param_elem),
        param_type_elem = list_elem_next(param_type_elem))
  {
    ast_node_t* param = (ast_node_t*)list_elem_get(param_elem);
    type_t* type = analyzer_visit_expr(analyzer, table, param);
    type_t* param_type = (type_t*)list_elem_get(param_type_elem);

    if (!type_is_same(type, param_type))
      report_error_incompatible_param_type(param->tok->loc);
  }

  return ret_type;
}

type_t* analyzer_visit_expr_op_member(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  if (node->expr_op.op_binary.rhs->kind != AST_ID)
    report_error_expected_member(node->expr_op.op_binary.rhs->tok->loc);

  type_t* type_owner = analyzer_visit_expr(analyzer, table, node->expr_op.op_binary.lhs);

  switch (node->expr_op.kind)
  {
  case OP_MEMBER:
    if (!type_is_owner(type_owner->kind))
      report_error_expected_owner(node->expr_op.op_binary.lhs->tok->loc);
    break;
  case OP_IND_MEMBER:
    if (type_owner->kind != TYPE_PTR)
      report_error_expected_ptr_type(node->expr_op.op_binary.lhs->tok->loc);

    if (!type_is_owner(type_owner->type_ptr.base_type->kind))
      report_error_expected_ptr_to_owner(node->expr_op.op_binary.lhs->tok->loc);

    type_owner = type_owner->type_ptr.base_type;
    break;
  case OP_NULL_SAFE_MEMBER:
    if (type_owner->kind != TYPE_NULLABLE)
      report_error_expected_nullable_type(node->expr_op.op_binary.lhs->tok->loc);

    if (!type_is_owner(type_owner->type_nullable.base_type->kind))
      report_error_expected_owner(node->expr_op.op_binary.lhs->tok->loc);

    type_owner = type_owner->type_nullable.base_type;
    break;
  default:
    unreachable();
  }
  
  switch (type_owner->kind)
  {
  case TYPE_STRUCT:
    LIST_FOR_EACH(elem, type_owner->type_struct.members)
    {
      type_member_t* member_type = (type_member_t*)list_elem_get(elem);
      
      if (strcmp(member_type->id, node->expr_op.op_binary.rhs->tok->id.value) == 0)
        return member_type->type;
    }

    report_error_no_member_with_name(node->expr_op.op_binary.rhs->tok->loc);
    break;
  case TYPE_UNION:
    LIST_FOR_EACH(elem, type_owner->type_union.members)
    {
      type_member_t* member_type = (type_member_t*)list_elem_get(elem);
      
      if (strcmp(member_type->id, node->expr_op.op_binary.rhs->tok->id.value) == 0)
        return member_type->type;
    }

    report_error_no_member_with_name(node->expr_op.op_binary.rhs->tok->loc);
    break;
  case TYPE_ENUM:
    LIST_FOR_EACH(elem, type_owner->type_enum.members)
    {
      type_member_t* member_type = (type_member_t*)list_elem_get(elem);
      
      if (strcmp(member_type->id, node->expr_op.op_binary.rhs->tok->id.value) == 0)
        return type_owner;
    }

    report_error_no_member_with_name(node->expr_op.op_binary.rhs->tok->loc);
    break;
  case TYPE_MOD:
    LIST_FOR_EACH(elem, type_owner->type_mod.members)
    {
      type_member_t* member_type = (type_member_t*)list_elem_get(elem);
      
      if (strcmp(member_type->id, node->expr_op.op_binary.rhs->tok->id.value) == 0)
        return member_type->type;
    }

    report_error_no_member_with_name(node->expr_op.op_binary.rhs->tok->loc);
    break;
  default:
    report_error_expected_owner(node->expr_op.op_binary.lhs->tok->loc);
  }

  return NULL;
}

type_t* analyzer_visit_expr_op(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  if (node->expr_op.kind == OP_CALL)
    return analyzer_visit_expr_op_call(analyzer, table, node);
  
  if (node->expr_op.kind == OP_MEMBER ||
      node->expr_op.kind == OP_IND_MEMBER ||
      node->expr_op.kind == OP_NULL_SAFE_MEMBER)
    return analyzer_visit_expr_op_member(analyzer, table, node);
  
  if (op_is_unary(node->expr_op.kind))
    return analyzer_visit_expr_op_unary(analyzer, table, node);
  
  if (op_is_binary(node->expr_op.kind))
    return analyzer_visit_expr_op_binary(analyzer, table, node);
  
  unreachable();

  return NULL;
}

type_t* analyzer_visit_expr(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  type_t* type = NULL;

  switch (node->kind)
  {
  case AST_ID:
    symbol_t* id_sym = symtable_lookup(table, node->tok->id.value);
    
    if (id_sym == NULL)
      report_error_undefined_symbol(node->tok->loc);

    switch (id_sym->node->kind)
    {
    case AST_DECL_VAR:
    case AST_DECL_FUN:
    case AST_DECL_GEN:
    case AST_DECL_STRUCT:
    case AST_DECL_UNION:
    case AST_DECL_ENUM:
    case AST_DECL_MOD:
    case AST_PARAM:
    case AST_VARIADIC_PARAM:
    case AST_GENERIC_PARAM:
    case AST_LOOP_VAR:
      break;
    default:
      report_error_symbol_is_not_an_expression(node->tok->loc);
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
    return analyzer_visit_expr_op(analyzer, table, node);
  default:
    unreachable();
  }

  return type;
}

ast_node_t* analyzer_visit_type(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_ID:
    symbol_t* id_sym = symtable_lookup(table, node->tok->id.value);
    
    if (id_sym == NULL)
      report_error_undefined_typename(node->tok->loc);

    if (id_sym->node->kind != AST_DECL_STRUCT &&
        id_sym->node->kind != AST_DECL_UNION &&
        id_sym->node->kind != AST_DECL_ENUM &&
        id_sym->node->kind != AST_GENERIC_PARAM)
      report_error_symbol_is_not_a_typename(node->tok->loc);

    if (id_sym->node->kind == AST_GENERIC_PARAM &&
        id_sym->node->generic_param.type->kind != AST_TYPE_TYPE)
      report_error_symbol_is_not_a_typename(node->tok->loc);

    ast_node_free(node);

    return id_sym->node;
  case AST_TYPE_MUT:
    node->type_mut.base_type = analyzer_visit_type(analyzer, table, node->type_mut.base_type);
    break;
  case AST_TYPE_CONST:
    node->type_const.base_type = analyzer_visit_type(analyzer, table, node->type_const.base_type);
    break;
  case AST_TYPE_PTR:
    node->type_ptr.base_type = analyzer_visit_type(analyzer, table, node->type_ptr.base_type);
    break;
  case AST_TYPE_ARRAY:
    node->type_array.base_type = analyzer_visit_type(analyzer, table, node->type_array.base_type);
    if (node->type_array.size != NULL)
      analyzer_visit_expr(analyzer, table, node->type_array.size);
    break;
  case AST_TYPE_REF:
    node->type_ref.base_type = analyzer_visit_type(analyzer, table, node->type_ref.base_type);
    break;
  case AST_TYPE_NULLABLE:
    node->type_nullable.base_type = analyzer_visit_type(analyzer, table, node->type_nullable.base_type);
    break;
  case AST_TYPE_FUN:
    node->type_fun.ret_type = analyzer_visit_type(analyzer, table, node->type_fun.ret_type);
    LIST_FOR_EACH(elem, node->type_fun.params)
      list_elem_set(elem, analyzer_visit_type(analyzer, table, (ast_node_t*)list_elem_get(elem)));
    break;
  case AST_TYPE_GEN:
    node->type_gen.ret_type = analyzer_visit_type(analyzer, table, node->type_gen.ret_type);
    LIST_FOR_EACH(elem, node->type_gen.params)
      list_elem_set(elem, analyzer_visit_type(analyzer, table, (ast_node_t*)list_elem_get(elem)));
    break;
  case AST_TYPE_BUILTIN_I8:
  case AST_TYPE_BUILTIN_I16:
  case AST_TYPE_BUILTIN_I32:
  case AST_TYPE_BUILTIN_I64:
  case AST_TYPE_BUILTIN_ISIZE:
  case AST_TYPE_BUILTIN_U8:
  case AST_TYPE_BUILTIN_U16:
  case AST_TYPE_BUILTIN_U32:
  case AST_TYPE_BUILTIN_U64:
  case AST_TYPE_BUILTIN_USIZE:
  case AST_TYPE_BUILTIN_F32:
  case AST_TYPE_BUILTIN_F64:
  case AST_TYPE_BUILTIN_BOOL:
  case AST_TYPE_BUILTIN_UNIT:
    break;
  default:
    unreachable();
  }

  return node;
}

void analyzer_visit_stmt_if(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symtable_t* if_table = symtable_init(table);

  analyzer_visit_expr(analyzer, if_table, node->stmt_if.cond);
  analyzer_visit_stmt(analyzer, if_table, node->stmt_if.stmt);

  if (node->stmt_if.stmt_else != NULL)
    analyzer_visit_stmt(analyzer, if_table, node->stmt_if.stmt_else);
}

void analyzer_visit_stmt_for(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symtable_t* for_table = symtable_init(table);

  analyzer_visit_loop_var(analyzer, for_table, node->stmt_for.var);
  analyzer_visit_expr(analyzer, for_table, node->stmt_for.range);
  analyzer_visit_stmt(analyzer, for_table, node->stmt_for.stmt);
}

void analyzer_visit_stmt_while(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symtable_t* while_table = symtable_init(table);

  analyzer_visit_expr(analyzer, while_table, node->stmt_while.cond);
  analyzer_visit_stmt(analyzer, while_table, node->stmt_while.stmt);
}

void analyzer_visit_stmt_break(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
}

void analyzer_visit_stmt_continue(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
}

type_t* analyzer_visit_stmt_return(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  type_t* type = type_builtin(TYPE_UNIT);

  if (node->stmt_return.expr != NULL)
    type = analyzer_visit_expr(analyzer, table, node->stmt_return.expr);

  type_t* expected_type = stack_peek(analyzer->ret_types);

  if (!type_is_same(type, expected_type))
    report_error_incompatible_return_type(node->tok->loc);

  return type;
}

type_t* analyzer_visit_stmt_yield(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  type_t* type = analyzer_visit_expr(analyzer, table, node->stmt_yield.expr);
  type_t* expected_type = stack_peek(analyzer->ret_types);
  
  if (!type_is_same(type, expected_type))
    report_error_incompatible_return_type(node->tok->loc);

  return type;
}

void analyzer_visit_stmt_block(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symtable_t* block_table = symtable_init(table);

  LIST_FOR_EACH(elem, node->stmt_block.stmts)
    analyzer_visit_stmt(analyzer, block_table, (ast_node_t*)list_elem_get(elem));
}

type_t* analyzer_visit_stmt_expr(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  return analyzer_visit_expr(analyzer, table, node->stmt_expr.expr);
}

void analyzer_visit_stmt(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:      analyzer_visit_decl_var(analyzer, table, node); break;
  case AST_STMT_IF:       analyzer_visit_stmt_if(analyzer, table, node); break;
  case AST_STMT_FOR:      analyzer_visit_stmt_for(analyzer, table, node); break;
  case AST_STMT_WHILE:    analyzer_visit_stmt_while(analyzer, table, node); break;
  case AST_STMT_BREAK:    analyzer_visit_stmt_break(analyzer, table, node); break;
  case AST_STMT_CONTINUE: analyzer_visit_stmt_continue(analyzer, table, node); break;
  case AST_STMT_RETURN:   analyzer_visit_stmt_return(analyzer, table, node); break;
  case AST_STMT_YIELD:    analyzer_visit_stmt_yield(analyzer, table, node); break;
  case AST_STMT_BLOCK:    analyzer_visit_stmt_block(analyzer, table, node); break;
  case AST_STMT_EXPR:     analyzer_visit_stmt_expr(analyzer, table, node); break;
  default: unreachable();
  }
}

void analyzer_visit_decl_var(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  node->decl_var.type = analyzer_visit_type(analyzer, table, node->decl_var.type);

  symbol_t* var_sym = symbol_init(node->decl_var.id->tok->id.value, node, type_of(node->decl_var.type));
  symbol_t* lookup = symtable_lookup(table, node->decl_var.id->tok->id.value);
  symbol_t* collision = symtable_insert(table, var_sym);

  if (collision != NULL && collision->node->kind == AST_DECL_VAR)
    report_error_variable_redeclaration(node->decl_var.id->tok->loc);

  if (lookup != NULL && lookup->node->kind == AST_DECL_VAR)
    report_warning_shadowed_variable(node->decl_var.id->tok->loc);

  if (node->decl_var.init != NULL)
  {
    type_t* init_type = analyzer_visit_expr(analyzer, table, node->decl_var.init);

    if (!type_is_same(init_type, var_sym->type))
      report_error_type_mismatch(node->tok->loc, var_sym->type, init_type);
  }
}

void analyzer_visit_decl_fun(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symtable_t* fun_table = symtable_init(table);

  if (node->decl_fun.generic_params != NULL)
    LIST_FOR_EACH(elem, node->decl_fun.generic_params)
      analyzer_visit_generic_param(analyzer, fun_table, (ast_node_t*)list_elem_get(elem));

  if (node->decl_fun.params != NULL)
    LIST_FOR_EACH(elem, node->decl_fun.params)
      analyzer_visit_param(analyzer, fun_table, (ast_node_t*)list_elem_get(elem));

  node->decl_fun.ret_type = analyzer_visit_type(analyzer, fun_table, node->decl_fun.ret_type);

  symbol_t* fun_sym = symbol_init(node->decl_fun.id->tok->id.value, node, type_of(node));
  fun_sym->scope = fun_table;

  symbol_t* collision = symtable_insert(table, fun_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  stack_push(analyzer->ret_types, fun_sym->type->type_fun.ret_type);
  analyzer_visit_stmt(analyzer, fun_table, node->decl_fun.stmt);
  stack_pop(analyzer->ret_types);
}

void analyzer_visit_decl_gen(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symtable_t* gen_table = symtable_init(table);

  if (node->decl_gen.generic_params != NULL)
    LIST_FOR_EACH(elem, node->decl_gen.generic_params)
      analyzer_visit_generic_param(analyzer, gen_table, (ast_node_t*)list_elem_get(elem));

  if (node->decl_gen.params != NULL)
    LIST_FOR_EACH(elem, node->decl_gen.params)
      analyzer_visit_param(analyzer, gen_table, (ast_node_t*)list_elem_get(elem));

  node->decl_gen.ret_type = analyzer_visit_type(analyzer, gen_table, node->decl_gen.ret_type);

  symbol_t* gen_sym = symbol_init(node->decl_gen.id->tok->id.value, node, type_of(node));
  gen_sym->scope = gen_table;

  symbol_t* collision = symtable_insert(table, gen_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);
  
  stack_push(analyzer->ret_types, gen_sym->type->type_gen.ret_type);
  analyzer_visit_stmt(analyzer, gen_table, node->decl_gen.stmt);
  stack_pop(analyzer->ret_types);
}

void analyzer_visit_decl_struct_member(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  analyzer_visit_decl_var(analyzer, table, node->decl_member.decl);
}

void analyzer_visit_decl_struct(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* struct_sym = symbol_init(node->decl_struct.id->tok->id.value, node, type_of(node));
  symbol_t* lookup = symtable_lookup(table, node->decl_struct.id->tok->id.value);
  symbol_t* collision = symtable_insert(table, struct_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* struct_table = symtable_init(table);
  struct_sym->scope = struct_table;

  if (node->decl_struct.generic_params != NULL)
    LIST_FOR_EACH(elem, node->decl_struct.generic_params)
      analyzer_visit_generic_param(analyzer, struct_table, (ast_node_t*)list_elem_get(elem));

  LIST_FOR_EACH(elem, node->decl_struct.members)
    analyzer_visit_decl_struct_member(analyzer, struct_table, (ast_node_t*)list_elem_get(elem));
}

void analyzer_visit_decl_union_member(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  analyzer_visit_decl_var(analyzer, table, node->decl_member.decl);
}

void analyzer_visit_decl_union(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* union_sym = symbol_init(node->decl_union.id->tok->id.value, node, type_of(node));
  symbol_t* lookup = symtable_lookup(table, node->decl_union.id->tok->id.value);
  symbol_t* collision = symtable_insert(table, union_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* union_table = symtable_init(table);
  union_sym->scope = union_table;

  LIST_FOR_EACH(elem, node->decl_union.members)
    analyzer_visit_decl_union_member(analyzer, union_table, (ast_node_t*)list_elem_get(elem));
}

void analyzer_visit_decl_enum(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* enum_sym = symbol_init(node->decl_enum.id->tok->id.value, node, type_of(node));
  symbol_t* lookup = symtable_lookup(table, node->decl_enum.id->tok->id.value);
  symbol_t* collision = symtable_insert(table, enum_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* enum_table = symtable_init(table);
  enum_sym->scope = enum_table;

  LIST_FOR_EACH(elem, node->decl_enum.members)
    analyzer_visit_enumerator(analyzer, enum_table, enum_sym, (ast_node_t*)list_elem_get(elem));
}

void analyzer_visit_decl_mod_member(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  analyzer_visit_decl(analyzer, table, node->decl_member.decl);
}

void analyzer_visit_decl_mod(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* mod_sym = symbol_init(node->decl_enum.id->tok->id.value, node, type_of(node));
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

  LIST_FOR_EACH(elem, node->decl_mod.members)
    analyzer_visit_decl_mod_member(analyzer, mod_table, (ast_node_t*)list_elem_get(elem));
}

void analyzer_visit_decl(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:    analyzer_visit_decl_var(analyzer, table, node); break;
  case AST_DECL_FUN:    analyzer_visit_decl_fun(analyzer, table, node); break;
  case AST_DECL_GEN:    analyzer_visit_decl_gen(analyzer, table, node); break;
  case AST_DECL_STRUCT: analyzer_visit_decl_struct(analyzer, table, node); break;
  case AST_DECL_UNION:  analyzer_visit_decl_union(analyzer, table, node); break;
  case AST_DECL_ENUM:   analyzer_visit_decl_enum(analyzer, table, node); break;
  case AST_DECL_MOD:    analyzer_visit_decl_mod(analyzer, table, node); break;
  default: unreachable();
  }
}

void analyzer_visit_prog(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symtable_t* prog_table = symtable_init(table);

  LIST_FOR_EACH(elem, node->prog.decls)
    analyzer_visit_decl(analyzer, prog_table, (ast_node_t*)list_elem_get(elem));
}

symtable_t* analyzer_analyze(analyzer_t* analyzer, ast_node_t* root)
{
  analyzer_visit_prog(analyzer, analyzer->root, root);
  return analyzer->root;
}
