#include "analzyer.h"

#include <stdlib.h>
#include <string.h>

#include "util.h"

#include "location.h"
#include "token.h"
#include "ast.h"
#include "typedesc.h"
#include "op.h"
#include "symtable.h"

#include "diagnostics.h"

void analyzer_init(arena_t* arena, analyzer_t* analyzer)
{
  analyzer->arena = arena;
  analyzer->ret_types = stack_init();
}

void analyzer_free(analyzer_t* analyzer)
{
  stack_free(analyzer->ret_types);
}

void analyzer_visit_expr_op_is(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node)
{
  node->lhs = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->lhs);
  node->rhs = (ast_node_t*)analyzer_visit_type(analyzer, scope, (ast_type_t*)node->rhs);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_BOOL));
}

void analyzer_visit_expr_op_as(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node)
{
  node->lhs = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->lhs);;
  node->rhs = (ast_node_t*)analyzer_visit_type(analyzer, scope, (ast_type_t*)node->rhs);

  typedesc_t* type_rhs = typetable_lookup(analyzer->typetable, node->rhs);
  assert(type_rhs != NULL);
  
  // TODO: check convertability

  typetable_insert(analyzer->typetable, (ast_node_t*)node, type_rhs);
}

void analyzer_visit_expr_op_unary(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_un_t* node)
{
  node->param = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->param);
  typedesc_t* param_desc = typetable_lookup(analyzer->typetable, node->param);
  assert(param_desc != NULL);

  switch (node->op_kind)
  {
  case OP_SIZEOF:
  case OP_ALIGNOF:
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_USIZE));
    return;
  case OP_TYPEOF:
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_TYPE));
    return;
  case OP_ARIT_INC_PRE:
  case OP_ARIT_DEC_PRE:
    if (param_desc->kind != TYPEDESC_REF)
      report_error_expected_reference_type(token_get_loc(node->tok));

    if (!typedesc_is_arithmetic(((typedesc_ref_t*)param_desc)->base_type) && ((typedesc_ref_t*)param_desc)->base_type->kind != TYPEDESC_PTR)
      report_error_expected_arithmetic_type(token_get_loc(node->tok));

    typetable_insert(analyzer->typetable, (ast_node_t*)node, param_desc);
    return;
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_POST:
    if (param_desc->kind != TYPEDESC_REF)
      report_error_expected_reference_type(token_get_loc(node->tok));

    if (!typedesc_is_arithmetic(((typedesc_ref_t*)param_desc)->base_type) && ((typedesc_ref_t*)param_desc)->base_type->kind != TYPEDESC_PTR)
      report_error_expected_arithmetic_type(token_get_loc(node->tok));

    typetable_insert(analyzer->typetable, (ast_node_t*)node, ((typedesc_ref_t*)param_desc)->base_type);
    return;
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_BIT_NOT:
    param_desc = typedesc_remove_ref(param_desc);

    if (!typedesc_is_arithmetic(param_desc))
      report_error_expected_arithmetic_type(token_get_loc(node->tok));

    typetable_insert(analyzer->typetable, (ast_node_t*)node, param_desc);
    return;
  case OP_LOGIC_NOT:
    param_desc = typedesc_remove_ref(param_desc);

    if (param_desc->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(token_get_loc(node->tok));

    typetable_insert(analyzer->typetable, (ast_node_t*)node, param_desc);
    return;
  case OP_IND:
    param_desc = typedesc_remove_ref(param_desc);

    if (param_desc->kind != TYPEDESC_PTR)
      report_error_expected_ptr_type(token_get_loc(node->tok));
    
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_make_ref(analyzer->arena, ((typedesc_ptr_t*)param_desc)->base_type));
    return;
  case OP_ADDR:
    if (param_desc->kind != TYPEDESC_REF)
      report_error_expected_reference_type(token_get_loc(node->tok));

    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_make_ptr(analyzer->arena, ((typedesc_ref_t*)param_desc)->base_type));
    return;
  default:
    unreachable();
  }
}

void analyzer_visit_expr_op_binary(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node)
{
  if (node->op_kind == OP_IS)
    analyzer_visit_expr_op_is(analyzer, scope, (ast_expr_op_bin_t*)node);
  else if (node->op_kind == OP_AS)
    analyzer_visit_expr_op_as(analyzer, scope, (ast_expr_op_bin_t*)node);
  else
  {
    node->lhs = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->lhs);
    typedesc_t* lhs_desc = typetable_lookup(analyzer->typetable, node->lhs);
    assert(lhs_desc != NULL);

    node->rhs = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->rhs);
    typedesc_t* rhs_desc = typetable_lookup(analyzer->typetable, node->rhs);
    assert(rhs_desc != NULL);

    switch (node->op_kind)
    {
    case OP_IN:
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (rhs_desc->kind != TYPEDESC_GEN)
        report_error_expected_generator_type(token_get_loc(node->rhs->tok));

      if (typedesc_is_same(lhs_desc, ((typedesc_gen_t*)rhs_desc)->yield_type))
        report_error_type_mismatch(token_get_loc(node->tok), lhs_desc, rhs_desc);

      typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_BOOL));
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
        report_error_expected_arithmetic_type(token_get_loc(node->lhs->tok));

      if (!typedesc_is_arithmetic(rhs_desc))
        report_error_expected_arithmetic_type(token_get_loc(node->rhs->tok));

      if (typedesc_is_signed(lhs_desc) != typedesc_is_signed(rhs_desc))
        report_warning_mixed_signedness(token_get_loc(node->tok));

      typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_promote(lhs_desc, rhs_desc));
      break;
    case OP_BIT_LSH:
    case OP_BIT_RSH:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (!typedesc_is_integer(lhs_desc))
        report_error_expected_integer_type(token_get_loc(node->lhs->tok));

      if (!typedesc_is_integer(rhs_desc))
        report_error_expected_integer_type(token_get_loc(node->rhs->tok));

      typetable_insert(analyzer->typetable, (ast_node_t*)node, lhs_desc);
      break;
    case OP_LOGIC_AND:
    case OP_LOGIC_OR:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (lhs_desc->kind != TYPEDESC_BOOL)
        report_error_expected_bool_type(token_get_loc(node->lhs->tok));

      if (rhs_desc->kind != TYPEDESC_BOOL)
        report_error_expected_bool_type(token_get_loc(node->rhs->tok));

      typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_BOOL));
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
        report_error_expected_arithmetic_type(token_get_loc(node->lhs->tok));

      if (!typedesc_is_arithmetic(rhs_desc))
        report_error_expected_arithmetic_type(token_get_loc(node->rhs->tok));

      typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_BOOL));
      break;
    case OP_SUBS:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (lhs_desc->kind != TYPEDESC_ARRAY)
        report_error_expected_subscriptable(token_get_loc(node->lhs->tok));

      if (!typedesc_is_integer(rhs_desc))
        report_error_expected_integer_type(token_get_loc(node->rhs->tok));

      typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_make_ref(analyzer->arena, ((typedesc_array_t*)lhs_desc)->base_type));
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
        report_error_expected_reference_type(token_get_loc(node->lhs->tok));

      if (!typedesc_is_same(typedesc_remove_ref(lhs_desc), typedesc_remove_ref(rhs_desc)))
        report_error_type_mismatch(token_get_loc(node->tok), lhs_desc, rhs_desc);

      typetable_insert(analyzer->typetable, (ast_node_t*)node, lhs_desc);
      break;
    case OP_RANGE:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (!typedesc_is_integer(lhs_desc))
        report_error_expected_integer_type(token_get_loc(node->lhs->tok));

      if (!typedesc_is_integer(rhs_desc))
        report_error_expected_integer_type(token_get_loc(node->rhs->tok));

      if (!typedesc_is_same(lhs_desc, rhs_desc))
        report_error_type_mismatch(token_get_loc(node->tok), lhs_desc, rhs_desc);

      typedesc_gen_t* range_type = (typedesc_gen_t*)arena_malloc(analyzer->arena, sizeof(typedesc_gen_t));
      assert(range_type != NULL);
      typedesc_init((typedesc_t*)range_type, TYPEDESC_GEN, 0, 0);

      range_type->param_types = NULL;
      range_type->yield_type = lhs_desc;

      typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)range_type);
      break;
    case OP_SEMICOLON: // TODO
    default:
      unreachable();
    }
  }
}

void analyzer_visit_expr_op_call(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_call_t* node)
{
  node->callee = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->callee);;
  typedesc_t* callee_desc = typetable_lookup(analyzer->typetable, node->callee);
  assert(callee_desc != NULL);

  list_t* param_types = NULL;
  typedesc_t* return_type = NULL;

  if (callee_desc->kind == TYPEDESC_FUN)
  {
    param_types = ((typedesc_fun_t*)callee_desc)->param_types;
    return_type = ((typedesc_fun_t*)callee_desc)->return_type;

    if (node->params == NULL)
    {
      if (param_types != NULL && list_size(param_types) != 0)
        report_error_too_few_arguments(token_get_loc(node->tok));
    }
    else
    {
      if (param_types == NULL)
      {
        if (list_size(node->params) != 0)
          report_error_too_many_arguments(token_get_loc(node->tok));
      }
      else
      {
        if (list_size(node->params) < list_size(param_types))
          report_error_too_few_arguments(token_get_loc(node->tok));

        if (list_size(node->params) > list_size(param_types))
          report_error_too_many_arguments(token_get_loc(node->tok));
      }
    }
  }
  else if (callee_desc->kind == TYPEDESC_GEN)
  {
    if (list_size(node->params) > list_size(((typedesc_gen_t*)callee_desc)->param_types))
      report_error_too_many_arguments(token_get_loc(node->tok));
    else if (list_size(node->params) < list_size(((typedesc_gen_t*)callee_desc)->param_types))
      report_error_too_few_arguments(token_get_loc(node->tok));

    param_types = ((typedesc_gen_t*)callee_desc)->param_types;
    return_type = ((typedesc_gen_t*)callee_desc)->yield_type;
  }
  else
    report_error_expected_callable(token_get_loc(node->tok));

  if (param_types != NULL)
    for (list_node_t *param_it      = list_front_node(node->params),
                     *param_type_it = list_front_node(param_types);
          param_it != NULL && param_type_it != NULL;
          param_it      = list_node_next(param_it),
          param_type_it = list_node_next(param_type_it))
    {
      ast_node_t* param = (ast_node_t*)list_node_get(param_it);
      param = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)param);
      list_node_set(param_it, param);

      typedesc_t* expected_param_desc = (typedesc_t*)list_node_get(param_type_it);

      typedesc_t* param_desc = typetable_lookup(analyzer->typetable, param);
      assert(param_desc != NULL);

      if (!typedesc_is_assignable(expected_param_desc, param_desc))
        report_error_incompatible_param_type(token_get_loc(param->tok));
    }

  typetable_insert(analyzer->typetable, (ast_node_t*)node, return_type);
}

void analyzer_visit_expr_op_member(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node)
{
  if (node->rhs->kind != AST_ID)
    report_error_expected_member(token_get_loc(node->rhs->tok));

  node->lhs = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->lhs);

  typedesc_t* lhs_desc = typetable_lookup(analyzer->typetable, node->lhs);
  assert(lhs_desc != NULL);

  typedesc_t* owner_desc = typedesc_remove_ref(lhs_desc);

  switch (node->op_kind)
  {
  case OP_MEMBER:
    if (!typedesc_is_owner(owner_desc))
      report_error_expected_owner(token_get_loc(node->lhs->tok));
    break;
  case OP_IND_MEMBER:
    if (owner_desc->kind != TYPEDESC_PTR)
      report_error_expected_ptr_type(token_get_loc(node->lhs->tok));

    if (!typedesc_is_owner(((typedesc_ptr_t*)owner_desc)->base_type))
      report_error_expected_ptr_to_owner(token_get_loc(node->lhs->tok));

    owner_desc = ((typedesc_ptr_t*)owner_desc)->base_type;
    break;
  case OP_NULL_SAFE_MEMBER:
    if (owner_desc->kind != TYPEDESC_OPT)
      report_error_expected_optional_type(token_get_loc(node->lhs->tok));

    if (!typedesc_is_owner(((typedesc_opt_t*)owner_desc)->base_type))
      report_error_expected_owner(token_get_loc(node->lhs->tok));

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

      if (strncmp(ast_id_ptr(member->id), location_get_ptr(token_get_loc(tok_rhs)), ast_id_len(member->id)) == 0)
      {
        node->rhs = (ast_node_t*)member;

        typedesc_t* member_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)member);
        assert(member_desc != NULL);

        typetable_insert(analyzer->typetable, (ast_node_t*)node, member_desc);
        return;
      }
    }

    report_error_no_member_with_name(token_get_loc(tok_rhs));
    break;
  case TYPEDESC_UNION:
    LIST_FOR_LOOP(it, ((ast_decl_union_t*)((typedesc_union_t*)owner_desc)->node)->members)
    {
      ast_decl_var_t* member = (ast_decl_var_t*)list_node_get(it);

      if (strncmp(ast_id_ptr(member->id), location_get_ptr(token_get_loc(tok_rhs)), ast_id_len(member->id)) == 0)
      {
        node->rhs = (ast_node_t*)member;
        
        typedesc_t* member_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)member);
        assert(member_desc != NULL);

        typetable_insert(analyzer->typetable, (ast_node_t*)node, member_desc);
        return;
      }
    }

    report_error_no_member_with_name(token_get_loc(tok_rhs));
    break;
  case TYPEDESC_ENUM:
    LIST_FOR_LOOP(it, ((ast_decl_enum_t*)((typedesc_enum_t*)owner_desc)->node)->values)
    {
      ast_decl_enum_constant_t* enum_constant = (ast_decl_enum_constant_t*)list_node_get(it);

      if (strncmp(ast_id_ptr(enum_constant->id), location_get_ptr(token_get_loc(tok_rhs)), ast_id_len(enum_constant->id)) == 0)
      {
        node->rhs = (ast_node_t*)enum_constant;
        typetable_insert(analyzer->typetable, (ast_node_t*)node, owner_desc);
        return;
      }
    }

    report_error_no_member_with_name(token_get_loc(tok_rhs));
    break;
  case TYPEDESC_MOD:
    LIST_FOR_LOOP(it, ((ast_decl_mod_t*)((typedesc_mod_t*)owner_desc)->node)->decls)
    {
      ast_decl_t* decl = (ast_decl_t*)list_node_get(it);

      if (strncmp(ast_id_ptr(decl->id), location_get_ptr(token_get_loc(tok_rhs)), ast_id_len(decl->id)) == 0)
      {
        node->rhs = (ast_node_t*)decl;

        typedesc_t* decl_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)decl);
        assert(decl_desc != NULL);

        typetable_insert(analyzer->typetable, (ast_node_t*)node, decl_desc);
        return;
      }
    }

    report_error_no_member_with_name(token_get_loc(tok_rhs));
    break;
  default:
    report_error_expected_owner(token_get_loc(tok_rhs));
  }
}

void analyzer_visit_expr_op(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_t* node)
{
  if (node->op_kind == OP_CALL)
    analyzer_visit_expr_op_call(analyzer, scope, (ast_expr_op_call_t*)node);
  else if (node->op_kind == OP_MEMBER ||
    node->op_kind == OP_IND_MEMBER ||
    node->op_kind == OP_NULL_SAFE_MEMBER)
    analyzer_visit_expr_op_member(analyzer, scope, (ast_expr_op_bin_t*)node);
  else if (op_is_unary(node->op_kind))
    analyzer_visit_expr_op_unary(analyzer, scope, (ast_expr_op_un_t*)node);
  else if (op_is_binary(node->op_kind))
    analyzer_visit_expr_op_binary(analyzer, scope, (ast_expr_op_bin_t*)node);
  else
    unreachable();
}

ast_node_t* analyzer_visit_expr(analyzer_t* analyzer, symtable_t* scope, ast_expr_t* node)
{
  switch (node->kind)
  {
  case AST_ID:
  {
    token_t* id_tok = node->tok;
    symbol_t* id_sym = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
    
    if (id_sym == NULL)
      report_error_undefined_symbol(token_get_loc(id_tok));

    if (!ast_is_decl(id_sym->node) && !ast_is_decl_param(id_sym->node))
      report_error_symbol_is_not_an_expression(token_get_loc(id_tok));

    return id_sym->node;
  }
  case AST_EXPR_LIT_INT:
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_I32));
    break;
  case AST_EXPR_LIT_FLT:
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_F32));
    break;
  case AST_EXPR_LIT_STR:
  {
    typedesc_ptr_t* str_type = (typedesc_ptr_t*)arena_malloc(analyzer->arena, sizeof(typedesc_ptr_t));
    assert(str_type != NULL);
    typedesc_init((typedesc_t*)str_type, TYPEDESC_PTR, TYPEDESC_PTR_SIZE, TYPEDESC_PTR_ALIGN);

    str_type->base_type = typedesc_builtin(TYPEDESC_U8);
    
    typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)str_type);
    break;
  }
  case AST_EXPR_LIT_CHAR:
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_U8));
    break;
  case AST_EXPR_LIT_BOOL:
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_BOOL));
    break;
  case AST_EXPR_LIT_NULL:
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_NULL));
    break;
  case AST_EXPR_OP:
    analyzer_visit_expr_op(analyzer, scope, (ast_expr_op_t*)node);
    break;
  default:
    unreachable();
  }

  return (ast_node_t*)node;
}

ast_node_t* analyzer_visit_type_member(analyzer_t* analyzer, symtable_t* scope, ast_type_member_t* node)
{
  token_t* owner_id_tok = node->owner->tok;
  symbol_t* owner_sym = symtable_lookup(scope, location_get_ptr(token_get_loc(owner_id_tok)), location_get_len(token_get_loc(owner_id_tok)));

  if (owner_sym == NULL)
    report_error_undefined_symbol(token_get_loc(owner_id_tok));

  if (owner_sym->node->kind != AST_DECL_MOD)
    report_error_expected_module(token_get_loc(owner_id_tok));

  if (node->member->kind == AST_ID)
  {
    token_t* member_id_tok = node->member->tok;
    symbol_t* member_sym = symtable_lookup(owner_sym->scope, location_get_ptr(token_get_loc(member_id_tok)), location_get_len(token_get_loc(member_id_tok)));

    if (member_sym == NULL)
      report_error_no_member_with_name(token_get_loc(node->member->tok));

    return member_sym->node;
  }
  else if (node->member->kind == AST_TYPE_MEMBER)
    return analyzer_visit_type_member(analyzer, owner_sym->scope, (ast_type_member_t*)node->member);

  unreachable();
  
  return NULL;
}

static ast_node_t* analyzer_visit_type_id(analyzer_t* analyzer, symtable_t* scope, ast_id_t* node)
{
  unused(analyzer);

  token_t* id_tok = node->tok;
  symbol_t* id_sym = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
  
  if (id_sym == NULL)
    report_error_undefined_typename(token_get_loc(id_tok));

  if (id_sym->node->kind != AST_DECL_STRUCT &&
      id_sym->node->kind != AST_DECL_UNION &&
      id_sym->node->kind != AST_DECL_ENUM &&
      id_sym->node->kind != AST_DECL_PARAM_GENERIC)
    report_error_symbol_is_not_a_typename(token_get_loc(id_tok));

  if (id_sym->node->kind == AST_DECL_PARAM_GENERIC &&
      ((ast_decl_param_generic_t*)id_sym->node)->type->kind != AST_TYPE_TYPE)
    report_error_symbol_is_not_a_typename(token_get_loc(id_tok));

  ast_node_free((ast_node_t*)node);

  return id_sym->node;
}

static void analyzer_visit_type_mut(analyzer_t* analyzer, symtable_t* scope, ast_type_mut_t* node)
{
  node->base_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->base_type);

  typedesc_mut_t* desc = (typedesc_mut_t*)arena_malloc(analyzer->arena, sizeof(typedesc_mut_t));
  assert(desc != NULL);

  desc->base_type = typetable_lookup(analyzer->typetable, node->base_type);
  assert(desc->base_type != NULL);

  typedesc_init((typedesc_t*)desc, TYPEDESC_MUT, desc->base_type->size, desc->base_type->align);
}

static void analyzer_visit_type_const(analyzer_t* analyzer, symtable_t* scope, ast_type_const_t* node)
{
  node->base_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->base_type);

  typedesc_const_t* desc = (typedesc_const_t*)arena_malloc(analyzer->arena, sizeof(typedesc_const_t));
  assert(desc != NULL);

  desc->base_type = typetable_lookup(analyzer->typetable, node->base_type);
  assert(desc->base_type != NULL);
  
  typedesc_init((typedesc_t*)desc, TYPEDESC_CONST, desc->base_type->size, desc->base_type->align);
}

static void analyzer_visit_type_ptr(analyzer_t* analyzer, symtable_t* scope, ast_type_ptr_t* node)
{
  node->base_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->base_type);
  
  typedesc_ptr_t* desc = (typedesc_ptr_t*)arena_malloc(analyzer->arena, sizeof(typedesc_ptr_t));
  assert(desc != NULL);

  typedesc_init((typedesc_t*)desc, TYPEDESC_PTR, TYPEDESC_PTR_SIZE, TYPEDESC_PTR_ALIGN);

  desc->base_type = typetable_lookup(analyzer->typetable, node->base_type);
  assert(desc->base_type != NULL);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)desc);
}

static void analyzer_visit_type_array(analyzer_t* analyzer, symtable_t* scope, ast_type_array_t* node)
{
  node->base_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->base_type);
  
  typedesc_array_t* desc = (typedesc_array_t*)arena_malloc(analyzer->arena, sizeof(typedesc_array_t));
  assert(desc != NULL);

  // TODO: array size should be a multiple of the size of its base type
  typedesc_init((typedesc_t*)desc, TYPEDESC_ARRAY, TYPEDESC_PTR_SIZE, TYPEDESC_PTR_ALIGN);
  
  desc->base_type = typetable_lookup(analyzer->typetable, node->base_type);
  assert(desc->base_type != NULL);

  if (node->size != NULL)
    analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->size);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)desc);
}

static void analyzer_visit_type_ref(analyzer_t* analyzer, symtable_t* scope, ast_type_ref_t* node)
{
  node->base_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->base_type);

  typedesc_ref_t* desc = (typedesc_ref_t*)arena_malloc(analyzer->arena, sizeof(typedesc_ref_t));
  assert(desc != NULL);

  desc->base_type = typetable_lookup(analyzer->typetable, node->base_type);
  assert(desc->base_type != NULL);

  typedesc_init((typedesc_t*)desc, TYPEDESC_REF, desc->base_type->size, desc->base_type->align);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)desc);
}

static void analyzer_visit_type_opt(analyzer_t* analyzer, symtable_t* scope, ast_type_opt_t* node)
{
  node->base_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->base_type);

  typedesc_opt_t* desc = (typedesc_opt_t*)arena_malloc(analyzer->arena, sizeof(typedesc_opt_t));
  assert(desc != NULL);

  desc->base_type = typetable_lookup(analyzer->typetable, node->base_type);
  assert(desc->base_type != NULL);

  typedesc_init((typedesc_t*)desc, TYPEDESC_OPT, desc->base_type->size, desc->base_type->align);
  
  typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)desc);
}

static void analyzer_visit_type_fun(analyzer_t* analyzer, symtable_t* scope, ast_type_fun_t* node)
{
  node->return_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->return_type);

  LIST_FOR_LOOP(it, node->params)
    list_node_set(it, analyzer_visit_type(analyzer, scope, (ast_type_t*)list_node_get(it)));

  typedesc_fun_t* desc = (typedesc_fun_t*)arena_malloc(analyzer->arena, sizeof(typedesc_fun_t));
  assert(desc != NULL);

  typedesc_init((typedesc_t*)desc, TYPEDESC_FUN, TYPEDESC_PTR_SIZE, TYPEDESC_PTR_ALIGN);
  
  desc->return_type = typetable_lookup(analyzer->typetable, node->return_type);
  assert(desc->return_type != NULL);

  desc->param_types = NULL;

  if (node->params != NULL)
  {
    desc->param_types = list_init();

    LIST_FOR_LOOP(it, node->params)
    {
      ast_decl_param_t* param = (ast_decl_param_t*)list_node_get(it);

      typedesc_t* param_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)param);
      assert(param_desc != NULL);

      list_push_back(desc->param_types, param_desc);
    }
  }
}

static void analyzer_visit_type_gen(analyzer_t* analyzer, symtable_t* scope, ast_type_gen_t* node)
{
  node->yield_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->yield_type);

  LIST_FOR_LOOP(it, node->params)
    list_node_set(it, analyzer_visit_type(analyzer, scope, (ast_type_t*)list_node_get(it)));

  typedesc_gen_t* desc = (typedesc_gen_t*)arena_malloc(analyzer->arena, sizeof(typedesc_gen_t));
  assert(desc != NULL);

  typedesc_init((typedesc_t*)desc, TYPEDESC_GEN, TYPEDESC_PTR_SIZE, TYPEDESC_PTR_ALIGN);
  
  desc->yield_type = typetable_lookup(analyzer->typetable, node->yield_type);
  assert(desc->yield_type != NULL);

  desc->param_types = NULL;

  if (node->params != NULL)
  {
    desc->param_types = list_init();

    LIST_FOR_LOOP(it, node->params)
    {
      ast_decl_param_t* param = (ast_decl_param_t*)list_node_get(it);

      typedesc_t* param_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)param);
      assert(param_desc != NULL);

      list_push_back(desc->param_types, param_desc);
    }
  }
}

ast_node_t* analyzer_visit_type(analyzer_t* analyzer, symtable_t* scope, ast_type_t* node)
{
  switch (node->kind)
  {
  case AST_ID:          return analyzer_visit_type_id(analyzer, scope, (ast_id_t*)node);
  case AST_TYPE_MUT:    analyzer_visit_type_mut(analyzer, scope, (ast_type_mut_t*)node); break;
  case AST_TYPE_CONST:  analyzer_visit_type_const(analyzer, scope, (ast_type_const_t*)node); break;
  case AST_TYPE_PTR:    analyzer_visit_type_ptr(analyzer, scope, (ast_type_ptr_t*)node); break;
  case AST_TYPE_ARRAY:  analyzer_visit_type_array(analyzer, scope, (ast_type_array_t*)node); break;
  case AST_TYPE_REF:    analyzer_visit_type_ref(analyzer, scope, (ast_type_ref_t*)node); break;
  case AST_TYPE_OPT:    analyzer_visit_type_opt(analyzer, scope, (ast_type_opt_t*)node); break;
  case AST_TYPE_FUN:    analyzer_visit_type_fun(analyzer, scope, (ast_type_fun_t*)node); break;
  case AST_TYPE_GEN:    analyzer_visit_type_gen(analyzer, scope, (ast_type_gen_t*)node); break;
  case AST_TYPE_I8:     typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_I8)); break;
  case AST_TYPE_I16:    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_I16)); break;
  case AST_TYPE_I32:    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_I32)); break;
  case AST_TYPE_I64:    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_I64)); break;
  case AST_TYPE_ISIZE:  typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_ISIZE)); break;
  case AST_TYPE_U8:     typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_U8)); break;
  case AST_TYPE_U16:    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_U16)); break;
  case AST_TYPE_U32:    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_U32)); break;
  case AST_TYPE_U64:    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_U64)); break;
  case AST_TYPE_USIZE:  typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_USIZE)); break;
  case AST_TYPE_F32:    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_F32)); break;
  case AST_TYPE_F64:    typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_F64)); break;
  case AST_TYPE_BOOL:   typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_BOOL)); break;
  case AST_TYPE_UNIT:   typetable_insert(analyzer->typetable, (ast_node_t*)node, typedesc_builtin(TYPEDESC_UNIT)); break;
  case AST_TYPE_MEMBER: return analyzer_visit_type_member(analyzer, scope, (ast_type_member_t*)node);
  default: unreachable();
  }

  return (ast_node_t*)node;
}

void analyzer_visit_stmt_if(analyzer_t* analyzer, symtable_t* scope, ast_stmt_if_t* node)
{
  symtable_t* if_table = symtable_init(scope);

  node->cond = analyzer_visit_expr(analyzer, if_table, (ast_expr_t*)node->cond);

  typedesc_t* cond_desc = typetable_lookup(analyzer->typetable, node->cond);
  assert(cond_desc != NULL);

  cond_desc = typedesc_remove_ref(cond_desc);

  if (cond_desc->kind != TYPEDESC_BOOL)
    report_error_expected_bool_type(token_get_loc(node->cond->tok));

  analyzer_visit_stmt(analyzer, if_table, (ast_stmt_t*)node->stmt);

  if (node->stmt_else != NULL)
    analyzer_visit_stmt(analyzer, if_table, (ast_stmt_t*)node->stmt_else);
}

void analyzer_visit_stmt_for(analyzer_t* analyzer, symtable_t* scope, ast_stmt_for_t* node)
{
  symtable_t* for_table = symtable_init(scope);

  analyzer_visit_decl_loop_var(analyzer, for_table, (ast_decl_loop_var_t*)node->var);
  
  node->range = analyzer_visit_expr(analyzer, for_table, (ast_expr_t*)node->range);

  typedesc_t* range_desc = typetable_lookup(analyzer->typetable, node->range);
  assert(range_desc != NULL);

  range_desc = typedesc_remove_ref(range_desc);

  if (range_desc->kind != TYPEDESC_GEN)
    report_error_expected_generator_type(token_get_loc(node->range->tok));

  analyzer_visit_stmt(analyzer, for_table, (ast_stmt_t*)node->stmt);
}

void analyzer_visit_stmt_while(analyzer_t* analyzer, symtable_t* scope, ast_stmt_while_t* node)
{
  symtable_t* while_table = symtable_init(scope);

  node->cond = analyzer_visit_expr(analyzer, while_table, (ast_expr_t*)node->cond);
  
  typedesc_t* cond_desc = typetable_lookup(analyzer->typetable, node->cond);
  assert(cond_desc != NULL);

  cond_desc = typedesc_remove_ref(cond_desc);

  if (cond_desc->kind != TYPEDESC_BOOL)
    report_error_expected_bool_type(token_get_loc(node->cond->tok));

  analyzer_visit_stmt(analyzer, while_table, (ast_stmt_t*)node->stmt);
}

void analyzer_visit_stmt_break(analyzer_t* analyzer, symtable_t* scope, ast_stmt_break_t* node)
{
  unused(analyzer);
  unused(scope);
  unused(node);
}

void analyzer_visit_stmt_continue(analyzer_t* analyzer, symtable_t* scope, ast_stmt_continue_t* node)
{
  unused(analyzer);
  unused(scope);
  unused(node);
}

typedesc_t* analyzer_visit_stmt_return(analyzer_t* analyzer, symtable_t* scope, ast_stmt_return_t* node)
{
  typedesc_t* desc = typedesc_builtin(TYPEDESC_UNIT);

  if (node->expr != NULL)
  {
    node->expr = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->expr);
    
    desc = typetable_lookup(analyzer->typetable, node->expr);
    assert(desc != NULL);
  }

  typedesc_t* expected_desc = stack_peek(analyzer->ret_types);

  if (!typedesc_is_same(desc, expected_desc))
    report_error_incompatible_return_type(token_get_loc(node->tok));

  return desc;
}

typedesc_t* analyzer_visit_stmt_yield(analyzer_t* analyzer, symtable_t* scope, ast_stmt_yield_t* node)
{
  node->expr = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->expr);

  typedesc_t* expr_desc = typetable_lookup(analyzer->typetable, node->expr);
  assert(expr_desc != NULL);

  typedesc_t* expected = stack_peek(analyzer->ret_types);
  
  if (!typedesc_is_same(expr_desc, expected))
    report_error_incompatible_return_type(token_get_loc(node->tok));

  return expr_desc;
}

void analyzer_visit_stmt_block(analyzer_t* analyzer, symtable_t* scope, ast_stmt_block_t* node)
{
  symtable_t* block_table = symtable_init(scope);

  LIST_FOR_LOOP(it, node->stmts)
    analyzer_visit_stmt(analyzer, block_table, (ast_stmt_t*)list_node_get(it));
}

typedesc_t* analyzer_visit_stmt_expr(analyzer_t* analyzer, symtable_t* scope, ast_stmt_expr_t* node)
{
  node->expr = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->expr);

  typedesc_t* expr_desc = typetable_lookup(analyzer->typetable, node->expr);
  assert(expr_desc != NULL);

  return expr_desc;
}

void analyzer_visit_stmt(analyzer_t* analyzer, symtable_t* scope, ast_stmt_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:      analyzer_visit_decl_var(analyzer, scope, (ast_decl_var_t*)node); break;
  case AST_STMT_IF:       analyzer_visit_stmt_if(analyzer, scope, (ast_stmt_if_t*)node); break;
  case AST_STMT_FOR:      analyzer_visit_stmt_for(analyzer, scope, (ast_stmt_for_t*)node); break;
  case AST_STMT_WHILE:    analyzer_visit_stmt_while(analyzer, scope, (ast_stmt_while_t*)node); break;
  case AST_STMT_BREAK:    analyzer_visit_stmt_break(analyzer, scope, (ast_stmt_break_t*)node); break;
  case AST_STMT_CONTINUE: analyzer_visit_stmt_continue(analyzer, scope, (ast_stmt_continue_t*)node); break;
  case AST_STMT_RETURN:   analyzer_visit_stmt_return(analyzer, scope, (ast_stmt_return_t*)node); break;
  case AST_STMT_YIELD:    analyzer_visit_stmt_yield(analyzer, scope, (ast_stmt_yield_t*)node); break;
  case AST_STMT_BLOCK:    analyzer_visit_stmt_block(analyzer, scope, (ast_stmt_block_t*)node); break;
  case AST_STMT_EXPR:     analyzer_visit_stmt_expr(analyzer, scope, (ast_stmt_expr_t*)node); break;
  default: unreachable();
  }
}

void analyzer_visit_decl_var(analyzer_t* analyzer, symtable_t* scope, ast_decl_var_t* node)
{
  node->type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->type);

  typedesc_t* var_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)node->type);
  assert(var_desc != NULL);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, var_desc);

  token_t* id_tok = node->id->tok;
  symbol_t* var_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
  symbol_t* collision = symtable_insert(scope, var_sym);

  if (collision != NULL && collision->node->kind == AST_DECL_VAR)
    report_error_variable_redeclaration(token_get_loc(id_tok), token_get_loc(collision->node->tok));

  if (lookup != NULL && lookup->node->kind == AST_DECL_VAR)
    report_warning_shadowed_variable(token_get_loc(id_tok));

  if (node->init != NULL)
  {
    node->init = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->init);

    typedesc_t* init_desc = typetable_lookup(analyzer->typetable, node->init);
    assert(init_desc != NULL);

    if (!typedesc_is_same(init_desc, var_desc))
      report_error_type_mismatch(token_get_loc(node->tok), var_desc, init_desc);
  }
}

void analyzer_visit_decl_loop_var(analyzer_t* analyzer, symtable_t* scope, ast_decl_loop_var_t* node)
{
  node->type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->type);

  typedesc_t* var_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)node->type);
  assert(var_desc != NULL);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, var_desc);

  token_t* id_tok = node->id->tok;
  symbol_t* var_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
  symbol_t* collision = symtable_insert(scope, var_sym);

  assert(collision == NULL);

  if (lookup != NULL)
    report_warning_shadowed_variable(token_get_loc(id_tok));
}

void analyzer_visit_decl_fun(analyzer_t* analyzer, symtable_t* scope, ast_decl_fun_t* node)
{
  symtable_t* fun_table = symtable_init(scope);

  typedesc_fun_t* fun_desc = (typedesc_fun_t*)arena_malloc(analyzer->arena, sizeof(typedesc_fun_t));
  assert(fun_desc != NULL);
  typedesc_init((typedesc_t*)fun_desc, TYPEDESC_FUN, TYPEDESC_PTR_SIZE, TYPEDESC_PTR_ALIGN);

  fun_desc->param_types = NULL;

  if (node->params != NULL)
  {
    fun_desc->param_types = list_init();

    LIST_FOR_LOOP(it, node->params)
    {
      ast_decl_param_t* param = (ast_decl_param_t*)list_node_get(it);
      analyzer_visit_decl_param(analyzer, fun_table, param);

      typedesc_t* param_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)param);
      assert(param_desc != NULL);

      list_push_back(fun_desc->param_types, param_desc);
    }
  }

  node->return_type = analyzer_visit_type(analyzer, fun_table, (ast_type_t*)node->return_type);

  fun_desc->return_type = typetable_lookup(analyzer->typetable, node->return_type);
  assert(fun_desc->return_type != NULL);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)fun_desc);

  token_t* id_tok = node->id->tok;
  symbol_t* fun_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), (ast_node_t*)node);
  fun_sym->scope = fun_table;

  symbol_t* collision = symtable_insert(scope, fun_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(token_get_loc(node->tok));

  stack_push(analyzer->ret_types, fun_desc->return_type);
  analyzer_visit_stmt(analyzer, fun_table, (ast_stmt_t*)node->stmt);
  stack_pop(analyzer->ret_types);
}

void analyzer_visit_decl_gen(analyzer_t* analyzer, symtable_t* scope, ast_decl_gen_t* node)
{
  symtable_t* gen_table = symtable_init(scope);

  typedesc_gen_t* gen_desc = (typedesc_gen_t*)arena_malloc(analyzer->arena, sizeof(typedesc_gen_t));
  assert(gen_desc != NULL);
  typedesc_init((typedesc_t*)gen_desc, TYPEDESC_GEN, TYPEDESC_PTR_SIZE, TYPEDESC_PTR_ALIGN);

  gen_desc->param_types = NULL;

  if (node->params != NULL)
  {
    gen_desc->param_types = list_init();

    LIST_FOR_LOOP(it, node->params)
    {
      ast_decl_param_t* param = (ast_decl_param_t*)list_node_get(it);
      analyzer_visit_decl_param(analyzer, gen_table, param);

      typedesc_t* param_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)param);
      assert(param_desc != NULL);

      list_push_back(gen_desc->param_types, param_desc);
    }
  }

  node->yield_type = analyzer_visit_type(analyzer, gen_table, (ast_type_t*)node->yield_type);

  gen_desc->yield_type = typetable_lookup(analyzer->typetable, node->yield_type);
  assert(gen_desc->yield_type != NULL);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)gen_desc);

  token_t* id_tok = node->id->tok;
  symbol_t* gen_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), (ast_node_t*)node);
  gen_sym->scope = gen_table;

  symbol_t* collision = symtable_insert(scope, gen_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(token_get_loc(node->tok));
  
  stack_push(analyzer->ret_types, gen_desc->yield_type);
  analyzer_visit_stmt(analyzer, gen_table, (ast_stmt_t*)node->stmt);
  stack_pop(analyzer->ret_types);
}

void analyzer_visit_decl_struct(analyzer_t* analyzer, symtable_t* scope, ast_decl_struct_t* node)
{
  token_t* id_tok = node->id->tok;
  symbol_t* struct_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
  symbol_t* collision = symtable_insert(scope, struct_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(token_get_loc(node->tok));

  if (lookup != NULL)
    report_warning_shadowed_symbol(token_get_loc(node->tok));

  symtable_t* struct_table = symtable_init(scope);
  struct_sym->scope = struct_table;

  typedesc_struct_t* struct_desc = (typedesc_struct_t*)arena_malloc(analyzer->arena, sizeof(typedesc_struct_t));
  assert(struct_desc != NULL);
  typedesc_init((typedesc_t*)struct_desc, TYPEDESC_STRUCT, 0, 1);

  struct_desc->node = (ast_decl_t*)node;
  struct_desc->member_types = list_init();

  LIST_FOR_LOOP(it, node->members)
  {
    ast_decl_var_t* var = (ast_decl_var_t*)list_node_get(it);
    analyzer_visit_decl_var(analyzer, struct_table, var);

    typedesc_t* var_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)var);
    assert(var_desc != NULL);

    list_push_back(struct_desc->member_types, var_desc);

    if (struct_desc->size % var_desc->align != 0)
      struct_desc->size += var_desc->align - (struct_desc->size % var_desc->align);

    struct_desc->size += var_desc->size;

    if (struct_desc->align < var_desc->align)
      struct_desc->align = var_desc->align;
  }

  typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)struct_desc);
}

void analyzer_visit_decl_union(analyzer_t* analyzer, symtable_t* scope, ast_decl_union_t* node)
{
  token_t* id_tok = node->id->tok;
  symbol_t* union_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
  symbol_t* collision = symtable_insert(scope, union_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(token_get_loc(node->tok));

  if (lookup != NULL)
    report_warning_shadowed_symbol(token_get_loc(node->tok));

  symtable_t* union_table = symtable_init(scope);
  union_sym->scope = union_table;

  typedesc_union_t* union_desc = (typedesc_union_t*)arena_malloc(analyzer->arena, sizeof(typedesc_union_t));
  assert(union_desc != NULL);
  typedesc_init((typedesc_t*)union_desc, TYPEDESC_UNION, 0, 1);

  union_desc->node = (ast_decl_t*)node;
  union_desc->member_types = list_init();

  LIST_FOR_LOOP(it, node->members)
  {
    ast_decl_var_t* var = (ast_decl_var_t*)list_node_get(it);
    analyzer_visit_decl_var(analyzer, union_table, var);

    typedesc_t* var_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)var);
    assert(var_desc != NULL);

    list_push_back(union_desc->member_types, var_desc);

    if (union_desc->size < var_desc->size)
      union_desc->size = var_desc->size;

    if (union_desc->align < var_desc->align)
      union_desc->align = var_desc->align;
  }

  typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)union_desc);
}

void analyzer_visit_decl_enum(analyzer_t* analyzer, symtable_t* scope, ast_decl_enum_t* node)
{
  token_t* id_tok = node->id->tok;
  symbol_t* enum_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
  symbol_t* collision = symtable_insert(scope, enum_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(token_get_loc(node->tok));

  if (lookup != NULL)
    report_warning_shadowed_symbol(token_get_loc(node->tok));

  symtable_t* enum_table = symtable_init(scope);
  enum_sym->scope = enum_table;

  typedesc_enum_t* enum_desc = (typedesc_enum_t*)arena_malloc(analyzer->arena, sizeof(typedesc_enum_t));
  assert(enum_desc != NULL);
  typedesc_init((typedesc_t*)enum_desc, TYPEDESC_ENUM, typedesc_builtin(TYPEDESC_I32)->size, typedesc_builtin(TYPEDESC_I32)->align);

  enum_desc->node = (ast_decl_t*)node;

  LIST_FOR_LOOP(it, node->values)
    analyzer_visit_decl_enum_constant(analyzer, enum_table, enum_sym, (ast_decl_enum_constant_t*)list_node_get(it));

  typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)enum_desc);
}

void analyzer_visit_decl_mod(analyzer_t* analyzer, symtable_t* scope, ast_decl_mod_t* node)
{
  token_t* id_tok = node->id->tok;
  symbol_t* mod_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), (ast_node_t*)node);
  symbol_t* collision = symtable_insert(scope, mod_sym);

  symtable_t* mod_table = NULL;
  
  if (collision != NULL)
  {
    mod_table = collision->scope;
    symbol_free(mod_sym);
  }
  else
  {
    mod_table = symtable_init(scope);
    mod_sym->scope = mod_table;
  }

  typedesc_mod_t* mod_desc = (typedesc_mod_t*)arena_malloc(analyzer->arena, sizeof(typedesc_mod_t));
  assert(mod_desc != NULL);
  typedesc_init((typedesc_t*)mod_desc, TYPEDESC_MOD, 0, 0);
  
  mod_desc->node = (ast_decl_t*)node;
  mod_desc->member_types = list_init();

  LIST_FOR_LOOP(it, node->decls)
  {
    ast_decl_t* decl = (ast_decl_t*)list_node_get(it);
    analyzer_visit_decl(analyzer, mod_table, decl);

    typedesc_t* decl_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)decl);
    assert(decl_desc != NULL);

    list_push_back(mod_desc->member_types, decl_desc);
  }

  typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)mod_desc);
}

void analyzer_visit_decl_generic(analyzer_t* analyzer, symtable_t* scope, ast_decl_generic_t* node)
{
  symtable_t* generic_table = symtable_init(scope);

  LIST_FOR_LOOP(it, node->params)
    analyzer_visit_decl_param_generic(analyzer, generic_table, (ast_decl_param_generic_t*)list_node_get(it));

  analyzer_visit_decl(analyzer, generic_table, (ast_decl_t*)node->decl);
}

void analyzer_visit_decl(analyzer_t* analyzer, symtable_t* scope, ast_decl_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:     analyzer_visit_decl_var(analyzer, scope, (ast_decl_var_t*)node); break;
  case AST_DECL_FUN:     analyzer_visit_decl_fun(analyzer, scope, (ast_decl_fun_t*)node); break;
  case AST_DECL_GEN:     analyzer_visit_decl_gen(analyzer, scope, (ast_decl_gen_t*)node); break;
  case AST_DECL_STRUCT:  analyzer_visit_decl_struct(analyzer, scope, (ast_decl_struct_t*)node); break;
  case AST_DECL_UNION:   analyzer_visit_decl_union(analyzer, scope, (ast_decl_union_t*)node); break;
  case AST_DECL_ENUM:    analyzer_visit_decl_enum(analyzer, scope, (ast_decl_enum_t*)node); break;
  case AST_DECL_MOD:     analyzer_visit_decl_mod(analyzer, scope, (ast_decl_mod_t*)node); break;
  case AST_DECL_GENERIC: analyzer_visit_decl_generic(analyzer, scope, (ast_decl_generic_t*)node); break;
  default: unreachable();
  }
}

void analyzer_visit_decl_param(analyzer_t* analyzer, symtable_t* scope, ast_decl_param_t* node)
{
  token_t* id_tok = node->id->tok;
  symbol_t* param_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), (ast_node_t*)node);
  symbol_t* collision = symtable_insert(scope, param_sym);

  if (collision != NULL && ast_is_decl_param(collision->node))
    report_error_parameter_redefinition(token_get_loc(id_tok), token_get_loc(collision->node->tok));

  switch (node->kind)
  {
  case AST_DECL_PARAM_DEFAULT:
    analyzer_visit_decl_param_default(analyzer, scope, (ast_decl_param_default_t*)node);
    break;
  case AST_DECL_PARAM_VARIADIC:
    analyzer_visit_decl_param_variadic(analyzer, scope, (ast_decl_param_variadic_t*)node);
    break;
  case AST_DECL_PARAM_GENERIC:
    analyzer_visit_decl_param_generic(analyzer, scope, (ast_decl_param_generic_t*)node);
    break;
  default:
    node->type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->type);

    typedesc_t* param_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)node->type);
    assert(param_desc != NULL);

    typetable_insert(analyzer->typetable, (ast_node_t*)node, param_desc);
    break;
  }
}

void analyzer_visit_decl_param_default(analyzer_t* analyzer, symtable_t* scope, ast_decl_param_default_t* node)
{
  node->type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->type);

  typedesc_t* param_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)node->type);
  assert(param_desc != NULL);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, param_desc);
  
  if (node->init != NULL)
    analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->init);
}

void analyzer_visit_decl_param_variadic(analyzer_t* analyzer, symtable_t* scope, ast_decl_param_variadic_t* node)
{
  node->type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->type);

  typedesc_t* param_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)node->type);
  assert(param_desc != NULL);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, param_desc);
}

void analyzer_visit_decl_param_generic(analyzer_t* analyzer, symtable_t* scope, ast_decl_param_generic_t* node)
{
  node->type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->type);

  if (node->type->kind != AST_TYPE_TYPE)
  {
    typedesc_t* param_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)node->type);
    assert(param_desc != NULL);

    typetable_insert(analyzer->typetable, (ast_node_t*)node, param_desc);
  }

  token_t* id_tok = node->id->tok;
  symbol_t* param_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), (ast_node_t*)node);
  symbol_t* collision = symtable_insert(scope, param_sym);

  if (collision != NULL && ast_is_decl_param(collision->node))
    report_error_parameter_redefinition(token_get_loc(id_tok), token_get_loc(collision->node->tok));
}

void analyzer_visit_decl_enum_constant(analyzer_t* analyzer, symtable_t* scope, symbol_t* enum_sym, ast_decl_enum_constant_t* node)
{
  unused(analyzer);

  token_t* id_tok = node->id->tok;
  symbol_t* enum_constant_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), (ast_node_t*)node);
  symbol_t* collision = symtable_insert(scope, enum_constant_sym);

  if (collision != NULL)
    report_error_enumerator_redeclaration(token_get_loc(id_tok), token_get_loc(collision->node->tok));

  typedesc_t* enum_desc = typetable_lookup(analyzer->typetable, enum_sym->node);
  assert(enum_desc != NULL);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, enum_desc);
}

void analyzer_visit_prog(analyzer_t* analyzer, symtable_t* scope, ast_prog_t* node)
{
  symtable_t* prog_table = symtable_init(scope);

  LIST_FOR_LOOP(it, node->decls)
    analyzer_visit_decl(analyzer, prog_table, (ast_decl_t*)list_node_get(it));
}

void analyzer_analyze(analyzer_t* analyzer, ast_node_t* root, symtable_t* symtable, typetable_t* typetable)
{
  assert(root->kind == AST_PROG);

  analyzer->symtable = symtable;
  analyzer->typetable = typetable;

  analyzer_visit_prog(analyzer, analyzer->symtable, (ast_prog_t*)root);
}
