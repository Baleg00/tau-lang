#include "analzyer.h"

#include <stdlib.h>
#include <string.h>

#include "util.h"

#include "list.h"
#include "stack.h"

#include "location.h"
#include "token.h"
#include "ast.h"
#include "typedesc.h"
#include "op.h"
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

void analyzer_visit_expr_op_is(analyzer_t* analyzer, symtable_t* table, ast_expr_op_bin_t* node)
{
  node->lhs = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->lhs);
  node->rhs = (ast_node_t*)analyzer_visit_type(analyzer, table, (ast_type_t*)node->rhs);

  node->desc = typedesc_builtin(TYPEDESC_BOOL);
}

void analyzer_visit_expr_op_as(analyzer_t* analyzer, symtable_t* table, ast_expr_op_bin_t* node)
{
  node->lhs = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->lhs);;
  node->rhs = (ast_node_t*)analyzer_visit_type(analyzer, table, (ast_type_t*)node->rhs);

  typedesc_t* type_rhs = ast_desc_of(node->rhs);
  
  // TODO: check convertability

  node->desc = type_rhs;
}

void analyzer_visit_expr_op_unary(analyzer_t* analyzer, symtable_t* table, ast_expr_op_un_t* node)
{
  node->param = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->param);
  typedesc_t* param_desc = ast_desc_of(node->param);

  switch (node->op_kind)
  {
  case OP_SIZEOF:
  case OP_ALIGNOF:
    node->desc = typedesc_builtin(TYPEDESC_USIZE);
    return;
  case OP_TYPEOF:
    node->desc = typedesc_builtin(TYPEDESC_TYPE);
    return;
  case OP_ARIT_INC_PRE:
  case OP_ARIT_DEC_PRE:
    if (param_desc->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->tok->loc);

    if (!typedesc_is_arithmetic(((typedesc_ref_t*)param_desc)->base_type) && ((typedesc_ref_t*)param_desc)->base_type->kind != TYPEDESC_PTR)
      report_error_expected_arithmetic_type(node->tok->loc);

    node->desc = param_desc;
    return;
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_POST:
    if (param_desc->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->tok->loc);

    if (!typedesc_is_arithmetic(((typedesc_ref_t*)param_desc)->base_type) && ((typedesc_ref_t*)param_desc)->base_type->kind != TYPEDESC_PTR)
      report_error_expected_arithmetic_type(node->tok->loc);

    node->desc = ((typedesc_ref_t*)param_desc)->base_type;
    return;
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_BIT_NOT:
    param_desc = typedesc_remove_ref(param_desc);

    if (!typedesc_is_arithmetic(param_desc))
      report_error_expected_arithmetic_type(node->tok->loc);

    node->desc = param_desc;
    return;
  case OP_LOGIC_NOT:
    param_desc = typedesc_remove_ref(param_desc);

    if (param_desc->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->tok->loc);

    node->desc = param_desc;
    return;
  case OP_IND:
    param_desc = typedesc_remove_ref(param_desc);

    if (param_desc->kind != TYPEDESC_PTR)
      report_error_expected_ptr_type(node->tok->loc);
    
    node->desc = typedesc_make_ref(((typedesc_ptr_t*)param_desc)->base_type);
    return;
  case OP_ADDR:
    if (param_desc->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->tok->loc);

    node->desc = typedesc_make_ptr(((typedesc_ref_t*)param_desc)->base_type);
    return;
  default:
    unreachable();
  }
}

void analyzer_visit_expr_op_binary(analyzer_t* analyzer, symtable_t* table, ast_expr_op_bin_t* node)
{
  if (node->op_kind == OP_IS)
    analyzer_visit_expr_op_is(analyzer, table, (ast_expr_op_bin_t*)node);
  else if (node->op_kind == OP_AS)
    analyzer_visit_expr_op_as(analyzer, table, (ast_expr_op_bin_t*)node);
  else
  {
    node->lhs = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->lhs);
    typedesc_t* lhs_desc = ast_desc_of(node->lhs);

    node->rhs = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->rhs);
    typedesc_t* rhs_desc = ast_desc_of(node->rhs);

    switch (node->op_kind)
    {
    case OP_IN:
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (rhs_desc->kind != TYPEDESC_GEN)
        report_error_expected_generator_type(node->rhs->tok->loc);

      if (typedesc_is_same(lhs_desc, ((typedesc_gen_t*)rhs_desc)->yield_type))
        report_error_type_mismatch(node->tok->loc, lhs_desc, rhs_desc);

      node->desc = typedesc_builtin(TYPEDESC_BOOL);
      break;
    case OP_ARIT_ADD:
    case OP_ARIT_SUB:
    case OP_ARIT_MUL:
    case OP_ARIT_DIV:
    case OP_ARIT_MOD:
    case OP_BIT_AND:
    case OP_BIT_OR:
    case OP_BIT_XOR:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (!typedesc_is_arithmetic(lhs_desc))
        report_error_expected_arithmetic_type(node->lhs->tok->loc);

      if (!typedesc_is_arithmetic(rhs_desc))
        report_error_expected_arithmetic_type(node->rhs->tok->loc);

      if (typedesc_is_signed(lhs_desc) != typedesc_is_signed(rhs_desc))
        report_warning_mixed_signedness(node->tok->loc);

      node->desc = typedesc_promote(lhs_desc, rhs_desc);
      break;
    case OP_BIT_LSH:
    case OP_BIT_RSH:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (!typedesc_is_integer(lhs_desc))
        report_error_expected_integer_type(node->lhs->tok->loc);

      if (!typedesc_is_integer(rhs_desc))
        report_error_expected_integer_type(node->rhs->tok->loc);

      node->desc = lhs_desc;
      break;
    case OP_LOGIC_AND:
    case OP_LOGIC_OR:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (lhs_desc->kind != TYPEDESC_BOOL)
        report_error_expected_bool_type(node->lhs->tok->loc);

      if (rhs_desc->kind != TYPEDESC_BOOL)
        report_error_expected_bool_type(node->rhs->tok->loc);

      node->desc = typedesc_builtin(TYPEDESC_BOOL);
      break;
    case OP_COMP_EQ:
    case OP_COMP_NE:
    case OP_COMP_LT:
    case OP_COMP_LE:
    case OP_COMP_GT:
    case OP_COMP_GE:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (!typedesc_is_arithmetic(lhs_desc))
        report_error_expected_arithmetic_type(node->lhs->tok->loc);

      if (!typedesc_is_arithmetic(rhs_desc))
        report_error_expected_arithmetic_type(node->rhs->tok->loc);

      node->desc = typedesc_builtin(TYPEDESC_BOOL);
      break;
    case OP_SUBS:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (lhs_desc->kind != TYPEDESC_ARRAY)
        report_error_expected_subscriptable(node->lhs->tok->loc);

      if (!typedesc_is_integer(rhs_desc))
        report_error_expected_integer_type(node->rhs->tok->loc);

      node->desc = typedesc_make_ref(((typedesc_array_t*)lhs_desc)->base_type);
      break;
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
      if (lhs_desc->kind != TYPEDESC_REF)
        report_error_expected_reference_type(node->lhs->tok->loc);

      if (!typedesc_is_same(typedesc_remove_ref(lhs_desc), typedesc_remove_ref(rhs_desc)))
        report_error_type_mismatch(node->tok->loc, lhs_desc, rhs_desc);

      node->desc = lhs_desc;
      break;
    case OP_RANGE:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (!typedesc_is_integer(lhs_desc))
        report_error_expected_integer_type(node->lhs->tok->loc);

      if (!typedesc_is_integer(rhs_desc))
        report_error_expected_integer_type(node->rhs->tok->loc);

      if (!typedesc_is_same(lhs_desc, rhs_desc))
        report_error_type_mismatch(node->tok->loc, lhs_desc, rhs_desc);

      typedesc_gen_t* range_type = typedesc_gen_init();
      range_type->param_types = NULL;
      range_type->yield_type = lhs_desc;

      node->desc = (typedesc_t*)range_type;
      break;
    case OP_SEMICOLON: // TODO
    default:
      unreachable();
    }
  }
}

void analyzer_visit_expr_op_call(analyzer_t* analyzer, symtable_t* table, ast_expr_op_call_t* node)
{
  node->callee = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->callee);;
  typedesc_t* callee_desc = ast_desc_of(node->callee);

  list_t* param_types = NULL;
  typedesc_t* return_type = NULL;

  if (callee_desc->kind == TYPEDESC_FUN)
  {
    if (node->params != NULL && ((typedesc_fun_t*)callee_desc)->param_types == NULL)
      report_error_too_many_arguments(node->tok->loc);
    
    if (node->params == NULL && ((typedesc_fun_t*)callee_desc)->param_types != NULL)
      report_error_too_few_arguments(node->tok->loc);

    if (list_size(node->params) > list_size(((typedesc_fun_t*)callee_desc)->param_types))
      report_error_too_many_arguments(node->tok->loc);
    
    if (list_size(node->params) < list_size(((typedesc_fun_t*)callee_desc)->param_types))
      report_error_too_few_arguments(node->tok->loc);

    param_types = ((typedesc_fun_t*)callee_desc)->param_types;
    return_type = ((typedesc_fun_t*)callee_desc)->return_type;
  }
  else if (callee_desc->kind == TYPEDESC_GEN)
  {
    if (list_size(node->params) > list_size(((typedesc_gen_t*)callee_desc)->param_types))
      report_error_too_many_arguments(node->tok->loc);
    else if (list_size(node->params) < list_size(((typedesc_gen_t*)callee_desc)->param_types))
      report_error_too_few_arguments(node->tok->loc);

    param_types = ((typedesc_gen_t*)callee_desc)->param_types;
    return_type = ((typedesc_gen_t*)callee_desc)->yield_type;
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
      param = analyzer_visit_expr(analyzer, table, (ast_expr_t*)param);
      list_node_set(param_it, param);

      typedesc_t* expected_param_type = (typedesc_t*)list_node_get(param_type_it);

      if (!typedesc_is_assignable(expected_param_type, ast_desc_of(param)))
        report_error_incompatible_param_type(param->tok->loc);
    }

  node->desc = return_type;
}

void analyzer_visit_expr_op_member(analyzer_t* analyzer, symtable_t* table, ast_expr_op_bin_t* node)
{
  if (node->rhs->kind != AST_ID)
    report_error_expected_member(node->rhs->tok->loc);

  node->lhs = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->lhs);
  typedesc_t* owner_desc = typedesc_remove_ref(ast_desc_of(node->lhs));

  switch (node->op_kind)
  {
  case OP_MEMBER:
    if (!typedesc_is_owner(owner_desc))
      report_error_expected_owner(node->lhs->tok->loc);
    break;
  case OP_IND_MEMBER:
    if (owner_desc->kind != TYPEDESC_PTR)
      report_error_expected_ptr_type(node->lhs->tok->loc);

    if (!typedesc_is_owner(((typedesc_ptr_t*)owner_desc)->base_type))
      report_error_expected_ptr_to_owner(node->lhs->tok->loc);

    owner_desc = ((typedesc_ptr_t*)owner_desc)->base_type;
    break;
  case OP_NULL_SAFE_MEMBER:
    if (owner_desc->kind != TYPEDESC_OPT)
      report_error_expected_optional_type(node->lhs->tok->loc);

    if (!typedesc_is_owner(((typedesc_opt_t*)owner_desc)->base_type))
      report_error_expected_owner(node->lhs->tok->loc);

    owner_desc = ((typedesc_opt_t*)owner_desc)->base_type;
    break;
  default:
    unreachable();
  }

  token_t* tok_rhs = node->rhs->tok;
  
  switch (owner_desc->kind)
  {
  case TYPEDESC_STRUCT:
    LIST_FOR_LOOP(it, ((ast_decl_struct_t*)((typedesc_struct_t*)owner_desc)->node)->members)
    {
      ast_decl_var_t* member = (ast_decl_var_t*)list_node_get(it);

      if (strncmp(ast_id_ptr(member->id), tok_rhs->loc->cur, ast_id_len(member->id)) == 0)
      {
        node->rhs = (ast_node_t*)member;
        node->desc = ast_desc_of((ast_node_t*)member);
        return;
      }
    }

    report_error_no_member_with_name(tok_rhs->loc);
    break;
  case TYPEDESC_UNION:
    LIST_FOR_LOOP(it, ((ast_decl_union_t*)((typedesc_union_t*)owner_desc)->node)->members)
    {
      ast_decl_var_t* member = (ast_decl_var_t*)list_node_get(it);

      if (strncmp(ast_id_ptr(member->id), tok_rhs->loc->cur, ast_id_len(member->id)) == 0)
      {
        node->rhs = (ast_node_t*)member;
        node->desc = ast_desc_of((ast_node_t*)member);
        return;
      }
    }

    report_error_no_member_with_name(tok_rhs->loc);
    break;
  case TYPEDESC_ENUM:
    LIST_FOR_LOOP(it, ((ast_decl_enum_t*)((typedesc_enum_t*)owner_desc)->node)->values)
    {
      ast_enumerator_t* enumerator = (ast_enumerator_t*)list_node_get(it);

      if (strncmp(ast_id_ptr(enumerator->id), tok_rhs->loc->cur, ast_id_len(enumerator->id)) == 0)
      {
        node->rhs = (ast_node_t*)enumerator;
        node->desc = owner_desc;
        return;
      }
    }

    report_error_no_member_with_name(tok_rhs->loc);
    break;
  case TYPEDESC_MOD:
    LIST_FOR_LOOP(it, ((ast_decl_mod_t*)((typedesc_mod_t*)owner_desc)->node)->decls)
    {
      ast_decl_t* decl = (ast_decl_t*)list_node_get(it);

      if (strncmp(ast_id_ptr(decl->id), tok_rhs->loc->cur, ast_id_len(decl->id)) == 0)
      {
        node->rhs = (ast_node_t*)decl;
        node->desc = ast_desc_of((ast_node_t*)decl);
        return;
      }
    }

    report_error_no_member_with_name(tok_rhs->loc);
    break;
  default:
    report_error_expected_owner(tok_rhs->loc);
  }
}

void analyzer_visit_expr_op(analyzer_t* analyzer, symtable_t* table, ast_expr_op_t* node)
{
  if (node->op_kind == OP_CALL)
    analyzer_visit_expr_op_call(analyzer, table, (ast_expr_op_call_t*)node);
  else if (node->op_kind == OP_MEMBER ||
    node->op_kind == OP_IND_MEMBER ||
    node->op_kind == OP_NULL_SAFE_MEMBER)
    analyzer_visit_expr_op_member(analyzer, table, (ast_expr_op_bin_t*)node);
  else if (op_is_unary(node->op_kind))
    analyzer_visit_expr_op_unary(analyzer, table, (ast_expr_op_un_t*)node);
  else if (op_is_binary(node->op_kind))
    analyzer_visit_expr_op_binary(analyzer, table, (ast_expr_op_bin_t*)node);
  else
    unreachable();
}

ast_node_t* analyzer_visit_expr(analyzer_t* analyzer, symtable_t* table, ast_expr_t* node)
{
  switch (node->kind)
  {
  case AST_ID:
  {
    token_t* id_tok = node->tok;
    symbol_t* id_sym = symtable_lookup(table, id_tok->loc->cur, id_tok->loc->len);
    
    if (id_sym == NULL)
      report_error_undefined_symbol(id_tok->loc);

    if (!ast_is_decl(id_sym->node) && !ast_is_param(id_sym->node))
      report_error_symbol_is_not_an_expression(id_tok->loc);

    return id_sym->node;
  }
  case AST_EXPR_LIT_INT:
    node->desc = typedesc_builtin(TYPEDESC_I32);
    break;
  case AST_EXPR_LIT_FLT:
    node->desc = typedesc_builtin(TYPEDESC_F32);
    break;
  case AST_EXPR_LIT_STR:
  {
    typedesc_ptr_t* str_type = typedesc_ptr_init();
    str_type->base_type = typedesc_builtin(TYPEDESC_U8);
    node->desc = (typedesc_t*)str_type;
    break;
  }
  case AST_EXPR_LIT_CHAR:
    node->desc = typedesc_builtin(TYPEDESC_U8);
    break;
  case AST_EXPR_LIT_BOOL:
    node->desc = typedesc_builtin(TYPEDESC_BOOL);
    break;
  case AST_EXPR_LIT_NULL:
    node->desc =  typedesc_builtin(TYPEDESC_NULL);
    break;
  case AST_EXPR_OP:
    analyzer_visit_expr_op(analyzer, table, (ast_expr_op_t*)node);
    break;
  default:
    unreachable();
  }

  return (ast_node_t*)node;
}

ast_node_t* analyzer_visit_type_member(analyzer_t* analyzer, symtable_t* table, ast_type_member_t* node)
{
  token_t* owner_id_tok = node->owner->tok;
  symbol_t* owner_sym = symtable_lookup(table, owner_id_tok->loc->cur, owner_id_tok->loc->len);

  if (owner_sym == NULL)
    report_error_undefined_symbol(owner_id_tok->loc);

  if (owner_sym->node->kind != AST_DECL_MOD)
    report_error_expected_module(owner_id_tok->loc);

  if (node->member->kind == AST_ID)
  {
    token_t* member_id_tok = node->member->tok;
    symbol_t* member_sym = symtable_lookup(owner_sym->scope, member_id_tok->loc->cur, member_id_tok->loc->len);

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
  {
    token_t* id_tok = node->tok;
    symbol_t* id_sym = symtable_lookup(table, id_tok->loc->cur, id_tok->loc->len);
    
    if (id_sym == NULL)
      report_error_undefined_typename(id_tok->loc);

    if (id_sym->node->kind != AST_DECL_STRUCT &&
        id_sym->node->kind != AST_DECL_UNION &&
        id_sym->node->kind != AST_DECL_ENUM &&
        id_sym->node->kind != AST_PARAM_GENERIC)
      report_error_symbol_is_not_a_typename(id_tok->loc);

    if (id_sym->node->kind == AST_PARAM_GENERIC &&
        ((ast_param_generic_t*)id_sym->node)->type->kind != AST_TYPE_TYPE)
      report_error_symbol_is_not_a_typename(id_tok->loc);

    ast_node_free((ast_node_t*)node);

    return id_sym->node;
  }
  case AST_TYPE_MUT:
    ((ast_type_mut_t*)node)->base_type = analyzer_visit_type(analyzer, table, (ast_type_t*)((ast_type_mut_t*)node)->base_type);
    ((ast_type_mut_t*)node)->desc = ast_desc_of(((ast_type_mut_t*)node)->base_type);
    break;
  case AST_TYPE_CONST:
    ((ast_type_const_t*)node)->base_type = analyzer_visit_type(analyzer, table, (ast_type_t*)((ast_type_const_t*)node)->base_type);
    ((ast_type_const_t*)node)->desc = ast_desc_of(((ast_type_const_t*)node)->base_type);
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
    node->desc = typedesc_builtin(TYPEDESC_I8);
    break;
  case AST_TYPE_I16:
    node->desc = typedesc_builtin(TYPEDESC_I16);
    break;
  case AST_TYPE_I32:
    node->desc = typedesc_builtin(TYPEDESC_I32);
    break;
  case AST_TYPE_I64:
    node->desc = typedesc_builtin(TYPEDESC_I64);
    break;
  case AST_TYPE_ISIZE:
    node->desc = typedesc_builtin(TYPEDESC_ISIZE);
    break;
  case AST_TYPE_U8:
    node->desc = typedesc_builtin(TYPEDESC_U8);
    break;
  case AST_TYPE_U16:
    node->desc = typedesc_builtin(TYPEDESC_U16);
    break;
  case AST_TYPE_U32:
    node->desc = typedesc_builtin(TYPEDESC_U32);
    break;
  case AST_TYPE_U64:
    node->desc = typedesc_builtin(TYPEDESC_U64);
    break;
  case AST_TYPE_USIZE:
    node->desc = typedesc_builtin(TYPEDESC_USIZE);
    break;
  case AST_TYPE_F32:
    node->desc = typedesc_builtin(TYPEDESC_F32);
    break;
  case AST_TYPE_F64:
    node->desc = typedesc_builtin(TYPEDESC_F64);
    break;
  case AST_TYPE_BOOL:
    node->desc = typedesc_builtin(TYPEDESC_BOOL);
    break;
  case AST_TYPE_UNIT:
    node->desc = typedesc_builtin(TYPEDESC_UNIT);
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

  node->cond = analyzer_visit_expr(analyzer, if_table, (ast_expr_t*)node->cond);
  typedesc_t* cond_desc = typedesc_remove_ref(ast_desc_of(node->cond));

  if (cond_desc->kind != TYPEDESC_BOOL)
    report_error_expected_bool_type(node->cond->tok->loc);

  analyzer_visit_stmt(analyzer, if_table, (ast_stmt_t*)node->stmt);

  if (node->stmt_else != NULL)
    analyzer_visit_stmt(analyzer, if_table, (ast_stmt_t*)node->stmt_else);
}

void analyzer_visit_stmt_for(analyzer_t* analyzer, symtable_t* table, ast_stmt_for_t* node)
{
  symtable_t* for_table = symtable_init(table);

  analyzer_visit_decl_loop_var(analyzer, for_table, (ast_decl_loop_var_t*)node->var);
  
  node->range = analyzer_visit_expr(analyzer, for_table, (ast_expr_t*)node->range);
  typedesc_t* range_desc = typedesc_remove_ref(ast_desc_of(node->range));

  if (range_desc->kind != TYPEDESC_GEN)
    report_error_expected_generator_type(node->range->tok->loc);

  analyzer_visit_stmt(analyzer, for_table, (ast_stmt_t*)node->stmt);
}

void analyzer_visit_stmt_while(analyzer_t* analyzer, symtable_t* table, ast_stmt_while_t* node)
{
  symtable_t* while_table = symtable_init(table);

  node->cond = analyzer_visit_expr(analyzer, while_table, (ast_expr_t*)node->cond);
  typedesc_t* cond_desc = typedesc_remove_ref(ast_desc_of(node->cond));

  if (cond_desc->kind != TYPEDESC_BOOL)
    report_error_expected_bool_type(node->cond->tok->loc);

  analyzer_visit_stmt(analyzer, while_table, (ast_stmt_t*)node->stmt);
}

void analyzer_visit_stmt_break(analyzer_t* analyzer, symtable_t* table, ast_stmt_break_t* node)
{
  unused(analyzer);
  unused(table);
  unused(node);
}

void analyzer_visit_stmt_continue(analyzer_t* analyzer, symtable_t* table, ast_stmt_continue_t* node)
{
  unused(analyzer);
  unused(table);
  unused(node);
}

typedesc_t* analyzer_visit_stmt_return(analyzer_t* analyzer, symtable_t* table, ast_stmt_return_t* node)
{
  typedesc_t* desc = typedesc_builtin(TYPEDESC_UNIT);

  if (node->expr != NULL)
  {
    node->expr = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->expr);
    desc = ast_desc_of(node->expr);
  }

  typedesc_t* expected = stack_top(analyzer->ret_types);

  if (!typedesc_is_same(desc, expected))
    report_error_incompatible_return_type(node->tok->loc);

  return desc;
}

typedesc_t* analyzer_visit_stmt_yield(analyzer_t* analyzer, symtable_t* table, ast_stmt_yield_t* node)
{
  node->expr = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->expr);
  typedesc_t* desc = ast_desc_of(node->expr);

  typedesc_t* expected = stack_top(analyzer->ret_types);
  
  if (!typedesc_is_same(desc, expected))
    report_error_incompatible_return_type(node->tok->loc);

  return desc;
}

void analyzer_visit_stmt_block(analyzer_t* analyzer, symtable_t* table, ast_stmt_block_t* node)
{
  symtable_t* block_table = symtable_init(table);

  LIST_FOR_LOOP(it, node->stmts)
    analyzer_visit_stmt(analyzer, block_table, (ast_stmt_t*)list_node_get(it));
}

typedesc_t* analyzer_visit_stmt_expr(analyzer_t* analyzer, symtable_t* table, ast_stmt_expr_t* node)
{
  node->expr = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->expr);
  return ast_desc_of(node->expr);
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
  node->desc = ast_desc_of(node->type);

  token_t* id_tok = node->id->tok;
  symbol_t* var_sym = symbol_init(id_tok->loc->cur, id_tok->loc->len, (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup(table, id_tok->loc->cur, id_tok->loc->len);
  symbol_t* collision = symtable_insert(table, var_sym);

  if (collision != NULL && collision->node->kind == AST_DECL_VAR)
    report_error_variable_redeclaration(id_tok->loc, collision->node->tok->loc);

  if (lookup != NULL && lookup->node->kind == AST_DECL_VAR)
    report_warning_shadowed_variable(id_tok->loc);

  if (node->init != NULL)
  {
    node->init = analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->init);
    typedesc_t* init_desc = ast_desc_of(node->init);

    if (!typedesc_is_same(init_desc, node->desc))
      report_error_type_mismatch(node->tok->loc, node->desc, init_desc);
  }
}

void analyzer_visit_decl_loop_var(analyzer_t* analyzer, symtable_t* table, ast_decl_loop_var_t* node)
{
  node->type = analyzer_visit_type(analyzer, table, (ast_type_t*)node->type);
  node->desc = ast_desc_of(node->type);

  token_t* id_tok = node->id->tok;
  symbol_t* var_sym = symbol_init(id_tok->loc->cur, id_tok->loc->len, (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup(table, id_tok->loc->cur, id_tok->loc->len);
  symbol_t* collision = symtable_insert(table, var_sym);

  assert(collision == NULL);

  if (lookup != NULL)
    report_warning_shadowed_variable(id_tok->loc);
}

void analyzer_visit_decl_fun(analyzer_t* analyzer, symtable_t* table, ast_decl_fun_t* node)
{
  symtable_t* fun_table = symtable_init(table);

  typedesc_fun_t* fun_desc = typedesc_fun_init();
  fun_desc->size = TYPEDESC_PTR_SIZE;
  fun_desc->align = TYPEDESC_PTR_SIZE;

  if (node->params != NULL)
  {
    fun_desc->param_types = list_init();

    LIST_FOR_LOOP(it, node->params)
    {
      ast_param_t* param = (ast_param_t*)list_node_get(it);
      analyzer_visit_param(analyzer, fun_table, param);
      list_push_back(fun_desc->param_types, param->desc);
    }
  }

  node->return_type = analyzer_visit_type(analyzer, fun_table, (ast_type_t*)node->return_type);
  fun_desc->return_type = ast_desc_of(node->return_type);

  node->desc = (typedesc_t*)fun_desc;

  token_t* id_tok = node->id->tok;
  symbol_t* fun_sym = symbol_init(id_tok->loc->cur, id_tok->loc->len, (ast_node_t*)node);
  fun_sym->scope = fun_table;

  symbol_t* collision = symtable_insert(table, fun_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  stack_push(analyzer->ret_types, ((typedesc_fun_t*)node->desc)->return_type);
  analyzer_visit_stmt(analyzer, fun_table, (ast_stmt_t*)node->stmt);
  stack_pop(analyzer->ret_types);
}

void analyzer_visit_decl_gen(analyzer_t* analyzer, symtable_t* table, ast_decl_gen_t* node)
{
  symtable_t* gen_table = symtable_init(table);

  typedesc_gen_t* gen_desc = typedesc_gen_init();
  gen_desc->size = TYPEDESC_PTR_SIZE;
  gen_desc->align = TYPEDESC_PTR_SIZE;

  if (node->params != NULL)
  {
    gen_desc->param_types = list_init();

    LIST_FOR_LOOP(it, node->params)
    {
      ast_param_t* param = (ast_param_t*)list_node_get(it);
      analyzer_visit_param(analyzer, gen_table, param);
      list_push_back(gen_desc->param_types, param->desc);
    }
  }

  node->yield_type = analyzer_visit_type(analyzer, gen_table, (ast_type_t*)node->yield_type);
  gen_desc->yield_type = ast_desc_of(node->yield_type);

  node->desc = (typedesc_t*)gen_desc;

  token_t* id_tok = node->id->tok;
  symbol_t* gen_sym = symbol_init(id_tok->loc->cur, id_tok->loc->len, (ast_node_t*)node);
  gen_sym->scope = gen_table;

  symbol_t* collision = symtable_insert(table, gen_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);
  
  stack_push(analyzer->ret_types, ((typedesc_gen_t*)node->desc)->yield_type);
  analyzer_visit_stmt(analyzer, gen_table, (ast_stmt_t*)node->stmt);
  stack_pop(analyzer->ret_types);
}

void analyzer_visit_decl_struct(analyzer_t* analyzer, symtable_t* table, ast_decl_struct_t* node)
{
  token_t* id_tok = node->id->tok;
  symbol_t* struct_sym = symbol_init(id_tok->loc->cur, id_tok->loc->len, (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup(table, id_tok->loc->cur, id_tok->loc->len);
  symbol_t* collision = symtable_insert(table, struct_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* struct_table = symtable_init(table);
  struct_sym->scope = struct_table;

  typedesc_struct_t* struct_desc = typedesc_struct_init();
  struct_desc->node = (ast_decl_t*)node;
  struct_desc->size = 0;
  struct_desc->align = 1;
  struct_desc->member_types = list_init();

  LIST_FOR_LOOP(it, node->members)
  {
    ast_decl_var_t* var = (ast_decl_var_t*)list_node_get(it);
    analyzer_visit_decl_var(analyzer, struct_table, var);
    list_push_back(struct_desc->member_types, var->desc);

    if (struct_desc->size % var->desc->align != 0)
      struct_desc->size += var->desc->align - (struct_desc->size % var->desc->align);

    struct_desc->size += var->desc->size;

    if (struct_desc->align < var->desc->align)
      struct_desc->align = var->desc->align;
  }

  node->desc = (typedesc_t*)struct_desc;
}

void analyzer_visit_decl_union(analyzer_t* analyzer, symtable_t* table, ast_decl_union_t* node)
{
  token_t* id_tok = node->id->tok;
  symbol_t* union_sym = symbol_init(id_tok->loc->cur, id_tok->loc->len, (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup(table, id_tok->loc->cur, id_tok->loc->len);
  symbol_t* collision = symtable_insert(table, union_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* union_table = symtable_init(table);
  union_sym->scope = union_table;

  typedesc_union_t* union_desc = typedesc_union_init();
  union_desc->node = (ast_decl_t*)node;
  union_desc->size = 0;
  union_desc->align = 1;
  union_desc->member_types = list_init();

  LIST_FOR_LOOP(it, node->members)
  {
    ast_decl_var_t* var = (ast_decl_var_t*)list_node_get(it);
    analyzer_visit_decl_var(analyzer, union_table, var);
    list_push_back(union_desc->member_types, var->desc);

    if (union_desc->size < var->desc->size)
      union_desc->size = var->desc->size;

    if (union_desc->align < var->desc->align)
      union_desc->align = var->desc->align;
  }

  node->desc = (typedesc_t*)union_desc;
}

void analyzer_visit_decl_enum(analyzer_t* analyzer, symtable_t* table, ast_decl_enum_t* node)
{
  token_t* id_tok = node->id->tok;
  symbol_t* enum_sym = symbol_init(id_tok->loc->cur, id_tok->loc->len, (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup(table, id_tok->loc->cur, id_tok->loc->len);
  symbol_t* collision = symtable_insert(table, enum_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* enum_table = symtable_init(table);
  enum_sym->scope = enum_table;

  typedesc_enum_t* enum_desc = typedesc_enum_init();
  enum_desc->node = (ast_decl_t*)node;
  enum_desc->size = typedesc_builtin(TYPEDESC_I32)->size;
  enum_desc->align = typedesc_builtin(TYPEDESC_I32)->align;

  LIST_FOR_LOOP(it, node->values)
    analyzer_visit_enumerator(analyzer, enum_table, enum_sym, (ast_enumerator_t*)list_node_get(it));

  node->desc = (typedesc_t*)enum_desc;
}

void analyzer_visit_decl_mod(analyzer_t* analyzer, symtable_t* table, ast_decl_mod_t* node)
{
  token_t* id_tok = node->id->tok;
  symbol_t* mod_sym = symbol_init(id_tok->loc->cur, id_tok->loc->len, (ast_node_t*)node);
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

  typedesc_mod_t* mod_desc = typedesc_mod_init();
  mod_desc->node = (ast_decl_t*)node;
  mod_desc->member_types = list_init();

  LIST_FOR_LOOP(it, node->decls)
  {
    ast_decl_t* decl = (ast_decl_t*)list_node_get(it);
    analyzer_visit_decl(analyzer, mod_table, decl);
    list_push_back(mod_desc->member_types, decl->desc);
  }

  node->desc = (typedesc_t*)mod_desc;
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
  token_t* id_tok = node->id->tok;
  symbol_t* param_sym = symbol_init(id_tok->loc->cur, id_tok->loc->len, (ast_node_t*)node);
  symbol_t* collision = symtable_insert(table, param_sym);

  if (collision != NULL && ast_is_param(collision->node))
    report_error_parameter_redefinition(id_tok->loc, collision->node->tok->loc);

  switch (node->kind)
  {
  case AST_PARAM_DEFAULT:
    analyzer_visit_param_default(analyzer, table, (ast_param_default_t*)node);
    break;
  case AST_PARAM_VARIADIC:
    analyzer_visit_param_variadic(analyzer, table, (ast_param_variadic_t*)node);
    break;
  case AST_PARAM_GENERIC:
    analyzer_visit_param_generic(analyzer, table, (ast_param_generic_t*)node);
    break;
  default:
    node->type = analyzer_visit_type(analyzer, table, (ast_type_t*)node->type);
    node->desc = ast_desc_of(node->type);
    break;
  }
}

void analyzer_visit_param_default(analyzer_t* analyzer, symtable_t* table, ast_param_default_t* node)
{
  node->type = analyzer_visit_type(analyzer, table, (ast_type_t*)node->type);
  node->desc = ast_desc_of(node->type);
  
  if (node->init != NULL)
    analyzer_visit_expr(analyzer, table, (ast_expr_t*)node->init);
}

void analyzer_visit_param_variadic(analyzer_t* analyzer, symtable_t* table, ast_param_variadic_t* node)
{
  node->type = analyzer_visit_type(analyzer, table, (ast_type_t*)node->type);
  node->desc = ast_desc_of(node->type);
}

void analyzer_visit_param_generic(analyzer_t* analyzer, symtable_t* table, ast_param_generic_t* node)
{
  node->type = analyzer_visit_type(analyzer, table, (ast_type_t*)node->type);

  if (node->type->kind != AST_TYPE_TYPE)
    node->desc = ast_desc_of(node->type);

  token_t* id_tok = node->id->tok;
  symbol_t* param_sym = symbol_init(id_tok->loc->cur, id_tok->loc->len, (ast_node_t*)node);
  symbol_t* collision = symtable_insert(table, param_sym);

  if (collision != NULL && ast_is_param(collision->node))
    report_error_parameter_redefinition(id_tok->loc, collision->node->tok->loc);
}

void analyzer_visit_enumerator(analyzer_t* analyzer, symtable_t* table, symbol_t* enum_sym, ast_enumerator_t* node)
{
  unused(analyzer);

  token_t* id_tok = node->id->tok;
  symbol_t* enumerator_sym = symbol_init(id_tok->loc->cur, id_tok->loc->len, (ast_node_t*)node);
  symbol_t* collision = symtable_insert(table, enumerator_sym);

  if (collision != NULL)
    report_error_enumerator_redeclaration(id_tok->loc, collision->node->tok->loc);

  node->desc = ast_desc_of(enum_sym->node);
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
