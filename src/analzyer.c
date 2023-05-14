#include "analzyer.h"

#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "list.h"
#include "stack.h"
#include "token.h"
#include "op.h"
#include "ast.h"
#include "symtable.h"
#include "typedesc.h"
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

typedesc_t* analyzer_visit_expr_op_is(analyzer_t* analyzer, symtable_t* table, ast_expr_op_bin_t* node)
{
  analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->lhs);
  typedesc_t* type_lhs = pair.type;
  node->lhs = pair.node;

  node->rhs = (ast_node_t*)analyzer_visit_type(analyzer, table, (ast_type_t*)node->rhs);

  typedesc_t* type_rhs = typedesc_of(node->rhs);

  return typedesc_builtin(TYPEDESC_BOOL);
}

typedesc_t* analyzer_visit_expr_op_as(analyzer_t* analyzer, symtable_t* table, ast_expr_op_bin_t* node)
{
  analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->lhs);
  typedesc_t* type_lhs = pair.type;
  node->lhs = pair.node;

  node->rhs = (ast_node_t*)analyzer_visit_type(analyzer, table, (ast_type_t*)node->rhs);

  typedesc_t* type_rhs = typedesc_of(node->rhs);
  
  // TODO: check convertability

  return type_rhs;
}

typedesc_t* analyzer_visit_expr_op_unary(analyzer_t* analyzer, symtable_t* table, ast_expr_op_un_t* node)
{
  analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->param);
  typedesc_t* param_type = pair.type;
  node->param = pair.node;

  switch (node->op_kind)
  {
  case OP_SIZEOF:
  case OP_ALIGNOF:
    return typedesc_builtin(TYPEDESC_USIZE);
  case OP_TYPEOF:
    return typedesc_builtin(TYPEDESC_TYPE);
  case OP_ARIT_INC_PRE:
  case OP_ARIT_DEC_PRE:
    if (param_type->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->tok->loc);

    if (!typedesc_is_arithmetic(((typedesc_ref_t*)param_type)->base_type) && ((typedesc_ref_t*)param_type)->base_type->kind != TYPEDESC_PTR)
      report_error_expected_arithmetic_type(node->tok->loc);

    return param_type;
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_POST:
    if (param_type->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->tok->loc);

    if (!typedesc_is_arithmetic(((typedesc_ref_t*)param_type)->base_type) && ((typedesc_ref_t*)param_type)->base_type->kind != TYPEDESC_PTR)
      report_error_expected_arithmetic_type(node->tok->loc);

    return ((typedesc_ref_t*)param_type)->base_type;
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_BIT_NOT:
    param_type = typedesc_remove_ref(param_type);

    if (!typedesc_is_arithmetic(param_type))
      report_error_expected_arithmetic_type(node->tok->loc);

    return param_type;
  case OP_LOGIC_NOT:
    param_type = typedesc_remove_ref(param_type);

    if (param_type->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->tok->loc);

    return param_type;
  case OP_IND:
    param_type = typedesc_remove_ref(param_type);

    if (param_type->kind != TYPEDESC_PTR)
      report_error_expected_ptr_type(node->tok->loc);
    
    return typedesc_make_ref(((typedesc_ptr_t*)param_type)->base_type);
  case OP_ADDR:
    if (param_type->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->tok->loc);

    return typedesc_make_ptr(((typedesc_ref_t*)param_type)->base_type);
  default:
    unreachable();
  }

  return NULL;
}

typedesc_t* analyzer_visit_expr_op_binary(analyzer_t* analyzer, symtable_t* table, ast_expr_op_bin_t* node)
{
  if (node->op_kind == OP_IS)
    return analyzer_visit_expr_op_is(analyzer, table, (ast_expr_op_bin_t*)node);
  
  if (node->op_kind == OP_AS)
    return analyzer_visit_expr_op_as(analyzer, table, (ast_expr_op_bin_t*)node);

  analyzer_type_node_pair_t pair_lhs = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->lhs);
  typedesc_t* type_lhs = pair_lhs.type;
  node->lhs = pair_lhs.node;

  analyzer_type_node_pair_t pair_rhs = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->rhs);
  typedesc_t* type_rhs = pair_rhs.type;
  node->rhs = pair_rhs.node;

  switch (node->op_kind)
  {
  case OP_IN:
    type_rhs = typedesc_remove_ref(type_rhs);

    if (type_rhs->kind != TYPEDESC_GEN)
      report_error_expected_generator_type(node->rhs->tok->loc);

    if (typedesc_is_same(type_lhs, ((typedesc_gen_t*)type_rhs)->yield_type))
      report_error_type_mismatch(node->tok->loc, type_lhs, type_rhs);

    return typedesc_builtin(TYPEDESC_BOOL);
  case OP_ARIT_ADD:
  case OP_ARIT_SUB:
  case OP_ARIT_MUL:
  case OP_ARIT_DIV:
  case OP_ARIT_MOD:
  case OP_BIT_AND:
  case OP_BIT_OR:
  case OP_BIT_XOR:
    type_lhs = typedesc_remove_ref(type_lhs);
    type_rhs = typedesc_remove_ref(type_rhs);

    if (!typedesc_is_arithmetic(type_lhs))
      report_error_expected_arithmetic_type(node->lhs->tok->loc);

    if (!typedesc_is_arithmetic(type_rhs))
      report_error_expected_arithmetic_type(node->rhs->tok->loc);

    if (typedesc_is_signed(type_lhs) != typedesc_is_signed(type_rhs))
      report_warning_mixed_signedness(node->tok->loc);

    return typedesc_promote(type_lhs, type_rhs);
  case OP_BIT_LSH:
  case OP_BIT_RSH:
    type_lhs = typedesc_remove_ref(type_lhs);
    type_rhs = typedesc_remove_ref(type_rhs);

    if (!typedesc_is_integer(type_lhs))
      report_error_expected_integer_type(node->lhs->tok->loc);

    if (!typedesc_is_integer(type_rhs))
      report_error_expected_integer_type(node->rhs->tok->loc);

    return type_lhs;
  case OP_LOGIC_AND:
  case OP_LOGIC_OR:
    type_lhs = typedesc_remove_ref(type_lhs);
    type_rhs = typedesc_remove_ref(type_rhs);

    if (type_lhs->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->lhs->tok->loc);

    if (type_rhs->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->rhs->tok->loc);

    return typedesc_builtin(TYPEDESC_BOOL);
  case OP_COMP_EQ:
  case OP_COMP_NE:
  case OP_COMP_LT:
  case OP_COMP_LE:
  case OP_COMP_GT:
  case OP_COMP_GE:
    type_lhs = typedesc_remove_ref(type_lhs);
    type_rhs = typedesc_remove_ref(type_rhs);

    if (!typedesc_is_arithmetic(type_lhs))
      report_error_expected_arithmetic_type(node->lhs->tok->loc);

    if (!typedesc_is_arithmetic(type_rhs))
      report_error_expected_arithmetic_type(node->rhs->tok->loc);

    return typedesc_builtin(TYPEDESC_BOOL);
  case OP_SUBS:
    type_lhs = typedesc_remove_ref(type_lhs);
    type_rhs = typedesc_remove_ref(type_rhs);

    if (type_lhs->kind != TYPEDESC_ARRAY)
      report_error_expected_subscriptable(node->lhs->tok->loc);

    if (!typedesc_is_integer(type_rhs))
      report_error_expected_integer_type(node->rhs->tok->loc);

    return typedesc_make_ref(((typedesc_array_t*)type_lhs)->base_type);
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
    if (type_lhs->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->lhs->tok->loc);

    if (!typedesc_is_same(typedesc_remove_ref(type_lhs), typedesc_remove_ref(type_rhs)))
      report_error_type_mismatch(node->tok->loc, type_lhs, type_rhs);

    return type_lhs;
  case OP_RANGE:
    type_lhs = typedesc_remove_ref(type_lhs);
    type_rhs = typedesc_remove_ref(type_rhs);

    if (!typedesc_is_integer(type_lhs))
      report_error_expected_integer_type(node->lhs->tok->loc);

    if (!typedesc_is_integer(type_rhs))
      report_error_expected_integer_type(node->rhs->tok->loc);

    if (!typedesc_is_same(type_lhs, type_rhs))
      report_error_type_mismatch(node->tok->loc, type_lhs, type_rhs);

    typedesc_gen_t* range_type = typedesc_gen_init();
    range_type->param_types = NULL;
    range_type->yield_type = type_lhs;

    return (typedesc_t*)range_type;
  case OP_SEMICOLON: // TODO
  default:
    unreachable();
  }

  return NULL;
}

typedesc_t* analyzer_visit_expr_op_call(analyzer_t* analyzer, symtable_t* table, ast_expr_op_call_t* node)
{
  analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->callee);
  typedesc_t* callee_type = pair.type;
  node->callee = pair.node;

  list_t* param_types = NULL;
  typedesc_t* return_type = NULL;

  if (callee_type->kind == TYPEDESC_FUN)
  {
    if (node->params != NULL && ((typedesc_fun_t*)callee_type)->param_types == NULL)
      report_error_too_many_arguments(node->tok->loc);
    
    if (node->params == NULL && ((typedesc_fun_t*)callee_type)->param_types != NULL)
      report_error_too_few_arguments(node->tok->loc);

    if (list_size(node->params) > list_size(((typedesc_fun_t*)callee_type)->param_types))
      report_error_too_many_arguments(node->tok->loc);
    
    if (list_size(node->params) < list_size(((typedesc_fun_t*)callee_type)->param_types))
      report_error_too_few_arguments(node->tok->loc);

    param_types = ((typedesc_fun_t*)callee_type)->param_types;
    return_type = ((typedesc_fun_t*)callee_type)->return_type;
  }
  else if (callee_type->kind == TYPEDESC_GEN)
  {
    if (list_size(node->params) > list_size(((typedesc_gen_t*)callee_type)->param_types))
      report_error_too_many_arguments(node->tok->loc);
    else if (list_size(node->params) < list_size(((typedesc_gen_t*)callee_type)->param_types))
      report_error_too_few_arguments(node->tok->loc);

    param_types = ((typedesc_gen_t*)callee_type)->param_types;
    return_type = ((typedesc_gen_t*)callee_type)->yield_type;
  }
  else
    report_error_expected_callable(node->tok->loc);

  if (param_types != NULL)
    for (list_node_t *param_it      = list_front_node(node->params),
                     *param_type_it = list_front_node(param_types);
          param_it != NULL && param_type_it != NULL;
          param_it      = list_node_next(param_it),
          param_type_it = list_node_next(param_type_it))
    {
      ast_node_t* param = (ast_node_t*)list_node_get(param_it);

      analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, table, (ast_expr_t*)param);
      typedesc_t* param_type = pair.type;
      list_node_set(param_it, pair.node);

      typedesc_t* expected_param_type = (typedesc_t*)list_node_get(param_type_it);

      if (!typedesc_is_assignable(expected_param_type, param_type))
        report_error_incompatible_param_type(param->tok->loc);
    }

  return return_type;
}

typedesc_t* analyzer_visit_expr_op_member(analyzer_t* analyzer, symtable_t* table, ast_expr_op_bin_t* node)
{
  if (node->rhs->kind != AST_ID)
    report_error_expected_member(node->rhs->tok->loc);

  analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->lhs);
  typedesc_t* type_owner = typedesc_remove_ref(pair.type);
  node->lhs = pair.node;

  switch (node->op_kind)
  {
  case OP_MEMBER:
    if (!typedesc_is_owner(type_owner))
      report_error_expected_owner(node->lhs->tok->loc);
    break;
  case OP_IND_MEMBER:
    if (type_owner->kind != TYPEDESC_PTR)
      report_error_expected_ptr_type(node->lhs->tok->loc);

    if (!typedesc_is_owner(((typedesc_ptr_t*)type_owner)->base_type))
      report_error_expected_ptr_to_owner(node->lhs->tok->loc);

    type_owner = ((typedesc_ptr_t*)type_owner)->base_type;
    break;
  case OP_NULL_SAFE_MEMBER:
    if (type_owner->kind != TYPEDESC_OPT)
      report_error_expected_optional_type(node->lhs->tok->loc);

    if (!typedesc_is_owner(((typedesc_opt_t*)type_owner)->base_type))
      report_error_expected_owner(node->lhs->tok->loc);

    type_owner = ((typedesc_opt_t*)type_owner)->base_type;
    break;
  default:
    unreachable();
  }

  token_id_t* tok_rhs = (token_id_t*)node->rhs->tok;
  
  switch (type_owner->kind)
  {
  case TYPEDESC_STRUCT:
    LIST_FOR_LOOP(it, ((ast_decl_struct_t*)((typedesc_struct_t*)type_owner)->node)->members)
    {
      ast_decl_var_t* member = (ast_decl_var_t*)list_node_get(it);

      if (strncmp(ast_id_ptr(member->id), tok_rhs->value, ast_id_len(member->id)) == 0)
      {
        node->rhs = (ast_node_t*)member;
        return typedesc_of((ast_node_t*)member);
      }
    }

    report_error_no_member_with_name(tok_rhs->loc);
    break;
  case TYPEDESC_UNION:
    LIST_FOR_LOOP(it, ((ast_decl_union_t*)((typedesc_union_t*)type_owner)->node)->members)
    {
      ast_decl_var_t* member = (ast_decl_var_t*)list_node_get(it);

      if (strncmp(ast_id_ptr(member->id), tok_rhs->value, ast_id_len(member->id)) == 0)
      {
        node->rhs = (ast_node_t*)member;
        return typedesc_of((ast_node_t*)member);
      }
    }

    report_error_no_member_with_name(tok_rhs->loc);
    break;
  case TYPEDESC_ENUM:
    LIST_FOR_LOOP(it, ((ast_decl_enum_t*)((typedesc_enum_t*)type_owner)->node)->values)
    {
      ast_enumerator_t* enumerator = (ast_enumerator_t*)list_node_get(it);

      if (strncmp(ast_id_ptr(enumerator->id), tok_rhs->value, ast_id_len(enumerator->id)) == 0)
      {
        node->rhs = (ast_node_t*)enumerator;
        return type_owner;
      }
    }

    report_error_no_member_with_name(tok_rhs->loc);
    break;
  case TYPEDESC_MOD:
    LIST_FOR_LOOP(it, ((ast_decl_mod_t*)((typedesc_mod_t*)type_owner)->node)->decls)
    {
      ast_decl_t* decl = (ast_decl_t*)list_node_get(it);

      if (strncmp(ast_id_ptr(decl->id), tok_rhs->value, ast_id_len(decl->id)) == 0)
      {
        node->rhs = (ast_node_t*)decl;
        return typedesc_of((ast_node_t*)decl);
      }
    }

    report_error_no_member_with_name(tok_rhs->loc);
    break;
  default:
    report_error_expected_owner(tok_rhs->loc);
  }

  return NULL;
}

typedesc_t* analyzer_visit_expr_op(analyzer_t* analyzer, symtable_t* table, ast_expr_op_t* node)
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

analyzer_type_node_pair_t analyzer_visit_expr(analyzer_t* analyzer, symtable_t* table, ast_expr_t* node)
{
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
    
    return (analyzer_type_node_pair_t){ typedesc_of(id_sym->node), id_sym->node };
  case AST_EXPR_LIT_INT:
    return (analyzer_type_node_pair_t){ typedesc_builtin(TYPEDESC_I32), (ast_node_t*)node };
  case AST_EXPR_LIT_FLT:
    return (analyzer_type_node_pair_t){ typedesc_builtin(TYPEDESC_F32), (ast_node_t*)node };
  case AST_EXPR_LIT_STR:
    typedesc_ptr_t* str_type = typedesc_ptr_init();
    str_type->base_type = typedesc_builtin(TYPEDESC_U8);
    return (analyzer_type_node_pair_t){ (typedesc_t*)str_type, (ast_node_t*)node };
  case AST_EXPR_LIT_CHAR:
    return (analyzer_type_node_pair_t){ typedesc_builtin(TYPEDESC_U8), (ast_node_t*)node };
  case AST_EXPR_LIT_BOOL:
    return (analyzer_type_node_pair_t){ typedesc_builtin(TYPEDESC_BOOL), (ast_node_t*)node };
  case AST_EXPR_LIT_NULL:
    return (analyzer_type_node_pair_t){ typedesc_builtin(TYPEDESC_NULL), (ast_node_t*)node };
    break;
  case AST_EXPR_OP:
    return (analyzer_type_node_pair_t){ analyzer_visit_expr_op(analyzer, table, (ast_expr_op_t*)node), (ast_node_t*)node };
  default:
    unreachable();
  }

  return (analyzer_type_node_pair_t){ NULL, NULL };
}

ast_node_t* analyzer_visit_type_member(analyzer_t* analyzer, symtable_t* table, ast_type_member_t* node)
{
  token_id_t* owner_id = (token_id_t*)node->owner->tok;
  symbol_t* owner_sym = symtable_lookup(table, owner_id->value);

  if (owner_sym == NULL)
    report_error_undefined_symbol(owner_id->loc);

  if (owner_sym->node->kind != AST_DECL_MOD)
    report_error_expected_module(owner_id->loc);

  ast_decl_mod_t* owner = (ast_decl_mod_t*)owner_sym->node;

  if (node->member->kind == AST_ID)
  {
    token_id_t* member_id = (token_id_t*)node->member->tok;
    symbol_t* member_sym = symtable_lookup(owner_sym->scope, member_id->value);

    if (member_sym == NULL)
      report_error_no_member_with_name(node->member->tok->loc);

    return member_sym->node;
  }
  else if (node->member->kind == AST_TYPE_MEMBER)
    return analyzer_visit_type_member(analyzer, owner_sym->scope, (ast_type_member_t*)node->member);

  unreachable();
  
  return NULL;
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
    LIST_FOR_LOOP(it, ((ast_type_fun_t*)node)->params)
      list_node_set(it, analyzer_visit_type(analyzer, table, (ast_type_t*)list_node_get(it)));
    break;
  case AST_TYPE_GEN:
    ((ast_type_gen_t*)node)->yield_type = analyzer_visit_type(analyzer, table, (ast_type_t*)((ast_type_gen_t*)node)->yield_type);
    LIST_FOR_LOOP(it, ((ast_type_gen_t*)node)->params)
      list_node_set(it, analyzer_visit_type(analyzer, table, (ast_type_t*)list_node_get(it)));
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
  case AST_TYPE_MEMBER:
    return analyzer_visit_type_member(analyzer, table, (ast_type_member_t*)node);
  default:
    unreachable();
  }

  return (ast_node_t*)node;
}

void analyzer_visit_stmt_if(analyzer_t* analyzer, symtable_t* table, ast_stmt_if_t* node)
{
  symtable_t* if_table = symtable_init(table);

  analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, if_table, (ast_expr_t*)node->cond);
  typedesc_t* cond_type = typedesc_remove_ref(pair.type);
  node->cond = pair.node;

  if (cond_type->kind != TYPEDESC_BOOL)
    report_error_expected_bool_type(node->cond->tok->loc);

  analyzer_visit_stmt(analyzer, if_table, (ast_stmt_t*)node->stmt);

  if (node->stmt_else != NULL)
    analyzer_visit_stmt(analyzer, if_table, (ast_stmt_t*)node->stmt_else);
}

void analyzer_visit_stmt_for(analyzer_t* analyzer, symtable_t* table, ast_stmt_for_t* node)
{
  symtable_t* for_table = symtable_init(table);

  analyzer_visit_decl_loop_var(analyzer, for_table, (ast_decl_loop_var_t*)node->var);
  
  analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, for_table, (ast_expr_t*)node->range);
  typedesc_t* range_type = typedesc_remove_ref(pair.type);
  node->range = pair.node;

  if (range_type->kind != TYPEDESC_GEN)
    report_error_expected_generator_type(node->range->tok->loc);

  analyzer_visit_stmt(analyzer, for_table, (ast_stmt_t*)node->stmt);
}

void analyzer_visit_stmt_while(analyzer_t* analyzer, symtable_t* table, ast_stmt_while_t* node)
{
  symtable_t* while_table = symtable_init(table);

  analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, while_table, (ast_expr_t*)node->cond);
  typedesc_t* cond_type = typedesc_remove_ref(pair.type);
  node->cond = pair.node;

  if (cond_type->kind != TYPEDESC_BOOL)
    report_error_expected_bool_type(node->cond->tok->loc);

  analyzer_visit_stmt(analyzer, while_table, (ast_stmt_t*)node->stmt);
}

void analyzer_visit_stmt_break(analyzer_t* analyzer, symtable_t* table, ast_stmt_break_t* node)
{
}

void analyzer_visit_stmt_continue(analyzer_t* analyzer, symtable_t* table, ast_stmt_continue_t* node)
{
}

typedesc_t* analyzer_visit_stmt_return(analyzer_t* analyzer, symtable_t* table, ast_stmt_return_t* node)
{
  typedesc_t* type = typedesc_builtin(TYPEDESC_UNIT);

  if (node->expr != NULL)
  {
    analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->expr);
    type = pair.type;
    node->expr = pair.node;
  }

  typedesc_t* expected_type = stack_top(analyzer->ret_types);

  if (!typedesc_is_same(type, expected_type))
    report_error_incompatible_return_type(node->tok->loc);

  return type;
}

typedesc_t* analyzer_visit_stmt_yield(analyzer_t* analyzer, symtable_t* table, ast_stmt_yield_t* node)
{
  analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->expr);
  typedesc_t* type = pair.type;
  node->expr = pair.node;

  typedesc_t* expected_type = stack_top(analyzer->ret_types);
  
  if (!typedesc_is_same(type, expected_type))
    report_error_incompatible_return_type(node->tok->loc);

  return type;
}

void analyzer_visit_stmt_block(analyzer_t* analyzer, symtable_t* table, ast_stmt_block_t* node)
{
  symtable_t* block_table = symtable_init(table);

  LIST_FOR_LOOP(it, node->stmts)
    analyzer_visit_stmt(analyzer, block_table, (ast_stmt_t*)list_node_get(it));
}

typedesc_t* analyzer_visit_stmt_expr(analyzer_t* analyzer, symtable_t* table, ast_stmt_expr_t* node)
{
  analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->expr);
  node->expr = pair.node;
  return pair.type;
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
  symbol_t* var_sym = symbol_init(tok_id->value, (ast_node_t*)node, typedesc_of((ast_node_t*)node->type));
  symbol_t* lookup = symtable_lookup(table, tok_id->value);
  symbol_t* collision = symtable_insert(table, var_sym);

  if (collision != NULL && collision->node->kind == AST_DECL_VAR)
    report_error_variable_redeclaration(tok_id->loc, collision->node->tok->loc);

  if (lookup != NULL && lookup->node->kind == AST_DECL_VAR)
    report_warning_shadowed_variable(tok_id->loc);

  if (node->init != NULL)
  {
    analyzer_type_node_pair_t pair = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->init);
    typedesc_t* init_type = pair.type;
    node->init = pair.node;

    if (!typedesc_is_same(init_type, var_sym->type))
      report_error_type_mismatch(node->tok->loc, var_sym->type, init_type);
  }
}

void analyzer_visit_decl_loop_var(analyzer_t* analyzer, symtable_t* table, ast_decl_loop_var_t* node)
{
  node->type = analyzer_visit_type(analyzer, table, (ast_type_t*)node->type);

  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* var_sym = symbol_init(tok_id->value, (ast_node_t*)node, typedesc_of((ast_node_t*)node));
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
    LIST_FOR_LOOP(it, node->params)
      analyzer_visit_param(analyzer, fun_table, (ast_param_t*)list_node_get(it));

  node->return_type = analyzer_visit_type(analyzer, fun_table, (ast_type_t*)node->return_type);

  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* fun_sym = symbol_init(tok_id->value, (ast_node_t*)node, typedesc_of((ast_node_t*)node));
  fun_sym->scope = fun_table;

  symbol_t* collision = symtable_insert(table, fun_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  stack_push(analyzer->ret_types, ((typedesc_fun_t*)fun_sym->type)->return_type);
  analyzer_visit_stmt(analyzer, fun_table, (ast_stmt_t*)node->stmt);
  stack_pop(analyzer->ret_types);
}

void analyzer_visit_decl_gen(analyzer_t* analyzer, symtable_t* table, ast_decl_gen_t* node)
{
  symtable_t* gen_table = symtable_init(table);

  if (node->params != NULL)
    LIST_FOR_LOOP(it, node->params)
      analyzer_visit_param(analyzer, gen_table, (ast_param_t*)list_node_get(it));

  node->yield_type = analyzer_visit_type(analyzer, gen_table, (ast_type_t*)node->yield_type);

  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* gen_sym = symbol_init(tok_id->value, (ast_node_t*)node, typedesc_of((ast_node_t*)node));
  gen_sym->scope = gen_table;

  symbol_t* collision = symtable_insert(table, gen_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);
  
  stack_push(analyzer->ret_types, ((typedesc_gen_t*)gen_sym->type)->yield_type);
  analyzer_visit_stmt(analyzer, gen_table, (ast_stmt_t*)node->stmt);
  stack_pop(analyzer->ret_types);
}

void analyzer_visit_decl_struct(analyzer_t* analyzer, symtable_t* table, ast_decl_struct_t* node)
{
  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* struct_sym = symbol_init(tok_id->value, (ast_node_t*)node, typedesc_of((ast_node_t*)node));
  symbol_t* lookup = symtable_lookup(table, tok_id->value);
  symbol_t* collision = symtable_insert(table, struct_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* struct_table = symtable_init(table);
  struct_sym->scope = struct_table;

  LIST_FOR_LOOP(it, node->members)
    analyzer_visit_decl_var(analyzer, struct_table, (ast_decl_var_t*)list_node_get(it));
}

void analyzer_visit_decl_union(analyzer_t* analyzer, symtable_t* table, ast_decl_union_t* node)
{
  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* union_sym = symbol_init(tok_id->value, (ast_node_t*)node, typedesc_of((ast_node_t*)node));
  symbol_t* lookup = symtable_lookup(table, tok_id->value);
  symbol_t* collision = symtable_insert(table, union_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* union_table = symtable_init(table);
  union_sym->scope = union_table;

  LIST_FOR_LOOP(it, node->members)
    analyzer_visit_decl_var(analyzer, union_table, (ast_decl_var_t*)list_node_get(it));
}

void analyzer_visit_decl_enum(analyzer_t* analyzer, symtable_t* table, ast_decl_enum_t* node)
{
  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* enum_sym = symbol_init(tok_id->value, (ast_node_t*)node, typedesc_of((ast_node_t*)node));
  symbol_t* lookup = symtable_lookup(table, tok_id->value);
  symbol_t* collision = symtable_insert(table, enum_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* enum_table = symtable_init(table);
  enum_sym->scope = enum_table;

  LIST_FOR_LOOP(it, node->values)
    analyzer_visit_enumerator(analyzer, enum_table, enum_sym, (ast_enumerator_t*)list_node_get(it));
}

void analyzer_visit_decl_mod(analyzer_t* analyzer, symtable_t* table, ast_decl_mod_t* node)
{
  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* mod_sym = symbol_init(tok_id->value, (ast_node_t*)node, typedesc_of((ast_node_t*)node));
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

  LIST_FOR_LOOP(it, node->decls)
    analyzer_visit_decl(analyzer, mod_table, (ast_decl_t*)list_node_get(it));
}

void analyzer_visit_decl_generic(analyzer_t* analyzer, symtable_t* table, ast_decl_generic_t* node)
{
  symtable_t* generic_table = symtable_init(table);

  if (node->params != NULL)
    LIST_FOR_LOOP(it, node->params)
      analyzer_visit_param_generic(analyzer, generic_table, (ast_param_generic_t*)list_node_get(it));

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
  symbol_t* param_sym = symbol_init(tok_id->value, (ast_node_t*)node, typedesc_of((ast_node_t*)node));
  symbol_t* collision = symtable_insert(table, param_sym);

  if (collision != NULL && ast_is_param(collision->node))
    report_error_parameter_redefinition(tok_id->loc, collision->node->tok->loc);

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
  symbol_t* param_sym = symbol_init(tok_id->value, (ast_node_t*)node, typedesc_of((ast_node_t*)node));
  symbol_t* collision = symtable_insert(table, param_sym);

  if (collision != NULL && ast_is_param(collision->node))
    report_error_parameter_redefinition(tok_id->loc, collision->node->tok->loc);
}

void analyzer_visit_enumerator(analyzer_t* analyzer, symtable_t* table, symbol_t* enum_sym, ast_enumerator_t* node)
{
  token_id_t* tok_id = (token_id_t*)node->id->tok;
  symbol_t* enumerator_sym = symbol_init(tok_id->value, (ast_node_t*)node, enum_sym->type);
  symbol_t* collision = symtable_insert(table, enumerator_sym);

  if (collision != NULL)
    report_error_enumerator_redeclaration(tok_id->loc, collision->node->tok->loc);
}

void analyzer_visit_prog(analyzer_t* analyzer, symtable_t* table, ast_prog_t* node)
{
  symtable_t* prog_table = symtable_init(table);

  LIST_FOR_LOOP(it, node->decls)
    analyzer_visit_decl(analyzer, prog_table, (ast_decl_t*)list_node_get(it));
}

symtable_t* analyzer_analyze(analyzer_t* analyzer, ast_node_t* root)
{
  assert(root->kind == AST_PROG);
  analyzer_visit_prog(analyzer, analyzer->root, (ast_prog_t*)root);
  return analyzer->root;
}
