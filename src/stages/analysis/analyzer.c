/**
 * \file analyzer.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/analyzer.h"

#include <stdlib.h>
#include <string.h>

#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

struct analyzer_t
{
  symtable_t* symtable; // Pointer to the root symbol table.
  typetable_t* typetable; // Pointer to the type table.
  typebuilder_t* typebuilder; // Pointer to the type typebuilder.
  list_t* scopes; // List of nodes representing scopes.
};

analyzer_t* analyzer_init(void)
{
  analyzer_t* analyzer = (analyzer_t*)malloc(sizeof(analyzer_t));
  memset(analyzer, 0, sizeof(analyzer_t));

  analyzer->scopes = list_init();

  return analyzer;
}

void analyzer_free(analyzer_t* analyzer)
{
  list_free(analyzer->scopes);
  free(analyzer);
}

void analyzer_scope_push(analyzer_t* analyzer, ast_node_t* node)
{
  list_push_front(analyzer->scopes, node);
}

ast_node_t* analyzer_scope_pop(analyzer_t* analyzer)
{
  return (ast_node_t*)list_pop_front(analyzer->scopes);
}

ast_node_t* analyzer_scope_innermost_loop(analyzer_t* analyzer)
{
  LIST_FOR_LOOP(it, analyzer->scopes)
  {
    ast_node_t* node = (ast_node_t*)list_node_get(it);

    if (node->kind == AST_STMT_WHILE ||
        node->kind == AST_STMT_FOR)
      return node;
  }

  return NULL;
}

ast_node_t* analyzer_scope_innermost_fun(analyzer_t* analyzer)
{
  LIST_FOR_LOOP(it, analyzer->scopes)
  {
    ast_node_t* node = (ast_node_t*)list_node_get(it);

    if (node->kind == AST_DECL_FUN)
      return node;
  }

  return NULL;
}

ast_node_t* analyzer_scope_innermost_defer(analyzer_t* analyzer)
{
  LIST_FOR_LOOP(it, analyzer->scopes)
  {
    ast_node_t* node = (ast_node_t*)list_node_get(it);

    if (node->kind == AST_STMT_DEFER)
      return node;
  }

  return NULL;
}

bool analyzer_scope_can_early_exit_loop(analyzer_t* analyzer)
{
  LIST_FOR_LOOP(it, analyzer->scopes)
  {
    ast_node_t* node = (ast_node_t*)list_node_get(it);

    switch (node->kind)
    {
    case AST_STMT_WHILE:
    case AST_STMT_FOR:   return true;
    case AST_STMT_DEFER: return false;
    default: noop();
    }
  }

  return false;
}

bool analyzer_scope_can_return(analyzer_t* analyzer)
{
  LIST_FOR_LOOP(it, analyzer->scopes)
  {
    ast_node_t* node = (ast_node_t*)list_node_get(it);

    switch (node->kind)
    {
    case AST_DECL_FUN:   return true;
    case AST_STMT_DEFER: return false;
    default: noop();
    }
  }

  return false;
}

void analyzer_visit_expr_op_unary(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_un_t* node)
{
  node->expr = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->expr);

  typedesc_t* expr_desc = typetable_lookup(analyzer->typetable, node->expr);
  assert(expr_desc != NULL);

  typedesc_t* node_desc = NULL;

  switch (node->op_kind)
  {
  case OP_SIZEOF:
  case OP_ALIGNOF:
  {
    node_desc = typebuilder_build_const(analyzer->typebuilder, typebuilder_build_usize(analyzer->typebuilder));
    break;
  }
  case OP_ARIT_INC_PRE:
  case OP_ARIT_DEC_PRE:
  {
    if (typedesc_remove_const(expr_desc)->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->expr->tok->loc);

    if (typedesc_remove_const_ref(expr_desc)->kind != TYPEDESC_MUT)
      report_error_expected_mutable_type(node->expr->tok->loc);

    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(expr_desc)))
      report_error_expected_arithmetic_type(node->expr->tok->loc);

    node_desc = typedesc_remove_const(expr_desc);
    break;
  }
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_POST:
  {
    if (typedesc_remove_const(expr_desc)->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->expr->tok->loc);

    if (typedesc_remove_const_ref(expr_desc)->kind != TYPEDESC_MUT)
      report_error_expected_mutable_type(node->expr->tok->loc);

    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(expr_desc)))
      report_error_expected_arithmetic_type(node->expr->tok->loc);

    node_desc = typedesc_remove_const_ref_mut(expr_desc);
    break;
  }
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_BIT_NOT:
  {
    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(expr_desc)))
      report_error_expected_arithmetic_type(node->expr->tok->loc);

    node_desc = typedesc_remove_const_ref_mut(expr_desc);
    break;
  }
  case OP_LOGIC_NOT:
  {
    if (typedesc_remove_const_ref_mut(expr_desc)->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->expr->tok->loc);

    node_desc = typedesc_remove_const_ref_mut(expr_desc);
    break;
  }
  case OP_IND:
  {
    if (typedesc_remove_const_ref_mut(expr_desc)->kind != TYPEDESC_PTR)
      report_error_expected_ptr_type(node->expr->tok->loc);
    
    typedesc_t* pointed_desc = ((typedesc_ptr_t*)typedesc_remove_const_ref_mut(expr_desc))->base_type;

    node_desc = typebuilder_build_ref(analyzer->typebuilder, pointed_desc);
    break;
  }
  case OP_ADDR:
  {
    if (typedesc_remove_const(expr_desc)->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->expr->tok->loc);

    node_desc = typebuilder_build_ptr(analyzer->typebuilder, typedesc_remove_const_ref(expr_desc));
    break;
  }
  default:
    unreachable();
  }

  if (expr_desc->kind == TYPEDESC_CONST && node_desc->kind != TYPEDESC_CONST)
    node_desc = typebuilder_build_const(analyzer->typebuilder, node_desc);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, node_desc);
}

void analyzer_visit_expr_op_binary(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node)
{
  node->lhs = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->lhs);
  node->rhs = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->rhs);

  typedesc_t* lhs_desc = typetable_lookup(analyzer->typetable, node->lhs);
  assert(lhs_desc != NULL);
  
  typedesc_t* rhs_desc = typetable_lookup(analyzer->typetable, node->rhs);
  assert(rhs_desc != NULL);

  typedesc_t* node_desc = NULL;

  switch (node->op_kind)
  {
  case OP_ARIT_ADD:
  case OP_ARIT_SUB:
  case OP_ARIT_MUL:
  case OP_ARIT_DIV:
  case OP_ARIT_MOD:
  case OP_BIT_AND:
  case OP_BIT_OR:
  case OP_BIT_XOR:
  {
    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(lhs_desc)))
      report_error_expected_arithmetic_type(node->lhs->tok->loc);

    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(rhs_desc)))
      report_error_expected_arithmetic_type(node->rhs->tok->loc);

    if (typedesc_is_signed(typedesc_remove_const_ref_mut(lhs_desc)) != typedesc_is_signed(typedesc_remove_const_ref_mut(rhs_desc)))
      report_warning_mixed_signedness(node->tok->loc);

    node_desc = typedesc_arithmetic_promote(typedesc_remove_const_ref_mut(lhs_desc), typedesc_remove_const_ref_mut(rhs_desc));
    break;
  }
  case OP_BIT_LSH:
  case OP_BIT_RSH:
  {
    if (!typedesc_is_integer(typedesc_remove_const_ref_mut(lhs_desc)))
      report_error_expected_integer_type(node->lhs->tok->loc);

    if (!typedesc_is_integer(typedesc_remove_const_ref_mut(rhs_desc)))
      report_error_expected_integer_type(node->rhs->tok->loc);

    node_desc = typedesc_remove_const_ref_mut(lhs_desc);
    break;
  }
  case OP_LOGIC_AND:
  case OP_LOGIC_OR:
  {
    if (typedesc_remove_const_ref_mut(lhs_desc)->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->lhs->tok->loc);

    if (typedesc_remove_const_ref_mut(rhs_desc)->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->rhs->tok->loc);

    node_desc = typebuilder_build_bool(analyzer->typebuilder);
    break;
  }
  case OP_COMP_EQ:
  case OP_COMP_NE:
  case OP_COMP_LT:
  case OP_COMP_LE:
  case OP_COMP_GT:
  case OP_COMP_GE:
  {
    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(lhs_desc)))
      report_error_expected_arithmetic_type(node->lhs->tok->loc);

    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(rhs_desc)))
      report_error_expected_arithmetic_type(node->rhs->tok->loc);

    node_desc = typebuilder_build_bool(analyzer->typebuilder);
    break;
  }
  case OP_ASSIGN:
  {
    if (typedesc_remove_const(lhs_desc)->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->lhs->tok->loc);

    if (typedesc_remove_const_ref(lhs_desc)->kind != TYPEDESC_MUT)
      report_error_expected_mutable_type(node->lhs->tok->loc);

    if (typedesc_remove_const_ref_mut(lhs_desc) != typedesc_remove_const_ref_mut(rhs_desc))
      report_error_type_mismatch(node->lhs->tok->loc, lhs_desc, rhs_desc);

    node_desc = lhs_desc;
    break;
  }
  default:
    unreachable();
  }

  if (lhs_desc->kind == TYPEDESC_CONST && rhs_desc->kind == TYPEDESC_CONST)
    node_desc = typebuilder_build_const(analyzer->typebuilder, node_desc);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, node_desc);
}

void analyzer_visit_expr_op_call_fun(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_call_t* node)
{
  typedesc_t* callee_desc = typetable_lookup(analyzer->typetable, node->callee);
  assert(callee_desc != NULL);

  typedesc_fun_t* fun_desc = (typedesc_fun_t*)typedesc_underlying_callable(callee_desc);
  assert(fun_desc->kind == TYPEDESC_FUN);

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_expr_t* expr = (ast_expr_t*)vector_get(node->params, i);
    expr = (ast_expr_t*)analyzer_visit_expr(analyzer, scope, (ast_expr_t*)expr);
    vector_set(node->params, i, expr);
  }

  size_t i = 0;
  for (; i < vector_size(node->params) && i < vector_size(fun_desc->param_types); i++)
  {
    ast_node_t* caller_param = (ast_node_t*)vector_get(node->params, i);

    typedesc_t* caller_param_desc = typetable_lookup(analyzer->typetable, caller_param);
    assert(caller_param_desc != NULL);

    typedesc_t* callee_param_desc = (typedesc_t*)vector_get(fun_desc->param_types, i);

    if (!typedesc_is_implicitly_convertible(caller_param_desc, callee_param_desc))
      report_error_type_mismatch(caller_param->tok->loc, callee_param_desc, caller_param_desc);
  }

  if (i == vector_size(node->params) && i != vector_size(fun_desc->param_types))
    report_error_too_few_arguments(node->tok->loc);

  if (i != vector_size(node->params) && i == vector_size(fun_desc->param_types))
    if (fun_desc->callconv != CALLCONV_CDECL)
      report_error_too_many_arguments(node->tok->loc);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, fun_desc->return_type);
}

void analyzer_visit_expr_op_call(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_call_t* node)
{
  node->callee = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->callee);

  typedesc_t* callee_desc = typetable_lookup(analyzer->typetable, node->callee);
  assert(callee_desc != NULL);

  typedesc_t* underlying_callee_desc = typedesc_underlying_callable(callee_desc);

  if (underlying_callee_desc->kind == TYPEDESC_FUN)
    analyzer_visit_expr_op_call_fun(analyzer, scope, node);
  else
    report_error_expected_callable(node->callee->tok->loc);
}

void analyzer_visit_expr_op_member(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_bin_t* node)
{
  if (node->rhs->kind != AST_EXPR_ID)
    report_error_expected_member(node->rhs->tok->loc);

  node->lhs = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->lhs);

  typedesc_t* lhs_desc = typetable_lookup(analyzer->typetable, node->lhs);
  assert(lhs_desc != NULL);

  typedesc_t* owner_desc = NULL;

  switch (node->op_kind)
  {
  case OP_ACCESS:
  {
    if (!typedesc_is_composite(typedesc_remove_const_ref_mut(lhs_desc)))
      report_error_expected_owner(node->lhs->tok->loc);

    owner_desc = typedesc_remove_const_ref_mut(lhs_desc);
    break;
  }
  case OP_IND_ACCESS:
  {
    if (typedesc_remove_const_ref_mut(lhs_desc)->kind != TYPEDESC_PTR)
      report_error_expected_ptr_type(node->lhs->tok->loc);

    if (!typedesc_is_composite(typedesc_remove_mut(((typedesc_ptr_t*)typedesc_remove_const_ref_mut(lhs_desc))->base_type)))
      report_error_expected_ptr_to_owner(node->lhs->tok->loc);

    owner_desc = typedesc_remove_mut(((typedesc_ptr_t*)typedesc_remove_const_ref_mut(lhs_desc))->base_type);
    break;
  }
  case OP_NULL_SAFE_ACCESS:
  {
    if (typedesc_remove_const_ref_mut(lhs_desc)->kind != TYPEDESC_OPT)
      report_error_expected_optional_type(node->lhs->tok->loc);

    if (!typedesc_is_composite(typedesc_remove_mut(((typedesc_opt_t*)typedesc_remove_const_ref_mut(lhs_desc))->base_type)))
      report_error_expected_owner(node->lhs->tok->loc);

    owner_desc = typedesc_remove_mut(((typedesc_opt_t*)typedesc_remove_const_ref_mut(lhs_desc))->base_type);
    break;
  }
  default:
    unreachable();
  }

  if (owner_desc->kind != TYPEDESC_STRUCT &&
      owner_desc->kind != TYPEDESC_UNION &&
      owner_desc->kind != TYPEDESC_ENUM)
    report_error_expected_owner(node->rhs->tok->loc);

  string_view_t id_view = token_to_string_view(node->rhs->tok);
  
  ast_decl_t* decl_node = (ast_decl_t*)((typedesc_decl_t*)owner_desc)->node;
  symtable_t* decl_scope = NULL;

  switch (decl_node->kind)
  {
  case AST_DECL_STRUCT: decl_scope = ((ast_decl_struct_t*)decl_node)->scope; break;
  case AST_DECL_UNION:  decl_scope = ((ast_decl_union_t* )decl_node)->scope; break;
  case AST_DECL_ENUM:   decl_scope = ((ast_decl_enum_t*  )decl_node)->scope; break;
  default: unreachable();
  }

  symbol_t* member_sym = symtable_get_with_str_view(decl_scope, id_view);

  if (member_sym == NULL)
    report_error_no_member_with_name(node->rhs->tok->loc);

  typedesc_t* member_desc = typetable_lookup(analyzer->typetable, member_sym->node);
  assert(member_desc != NULL);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, member_desc);
}

void analyzer_visit_expr_op(analyzer_t* analyzer, symtable_t* scope, ast_expr_op_t* node)
{
  if (node->op_kind == OP_CALL)
    analyzer_visit_expr_op_call(analyzer, scope, (ast_expr_op_call_t*)node);
  else if (node->op_kind == OP_ACCESS ||
    node->op_kind == OP_IND_ACCESS ||
    node->op_kind == OP_NULL_SAFE_ACCESS)
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
  case AST_EXPR_ID:
  {
    string_view_t id_view = token_to_string_view(node->tok);
    symbol_t* id_sym = symtable_lookup_with_str_view(scope, id_view);
    
    if (id_sym == NULL)
      report_error_undefined_symbol(node->tok->loc);

    switch (id_sym->node->kind)
    {
    case AST_DECL_VAR:
    case AST_DECL_PARAM:
    case AST_DECL_FUN:
    {
      ast_expr_id_t* decl = ast_expr_id_init();
      decl->tok = node->tok;
      decl->decl = id_sym->node;

      typedesc_t* desc = typetable_lookup(analyzer->typetable, decl->decl);
      assert(desc != NULL);

      desc = typebuilder_build_ref(analyzer->typebuilder, desc);

      typetable_insert(analyzer->typetable, (ast_node_t*)decl, desc);

      return (ast_node_t*)decl;
    }
    default:
      report_error_symbol_is_not_an_expression(node->tok->loc);
    }
  }
  case AST_EXPR_LIT_INT:
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_i32(analyzer->typebuilder));
    break;
  case AST_EXPR_LIT_FLT:
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_f32(analyzer->typebuilder));
    break;
  case AST_EXPR_LIT_STR:
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_ptr(analyzer->typebuilder, typebuilder_build_u8(analyzer->typebuilder)));
    break;
  case AST_EXPR_LIT_CHAR:
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_u8(analyzer->typebuilder));
    break;
  case AST_EXPR_LIT_BOOL:
    typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_bool(analyzer->typebuilder));
    break;
  case AST_EXPR_OP_UNARY:
  case AST_EXPR_OP_BINARY:
  case AST_EXPR_OP_CALL:
    analyzer_visit_expr_op(analyzer, scope, (ast_expr_op_t*)node);
    break;
  default:
    unreachable();
  }

  return (ast_node_t*)node;
}

ast_node_t* analyzer_visit_type_member(analyzer_t* analyzer, symtable_t* scope, ast_type_mbr_t* node)
{
  assert(node->member->kind == AST_TYPE_ID);

  switch (node->parent->kind)
  {
  case AST_TYPE_MEMBER:
    node->parent = analyzer_visit_type_member(analyzer, scope, (ast_type_mbr_t*)node->parent);
    break;
  case AST_TYPE_ID:
  {
    string_view_t id_view = token_to_string_view(node->parent->tok);
    symbol_t* owner_sym = symtable_lookup_with_str_view(scope, id_view);

    if (owner_sym == NULL)
      report_error_undefined_symbol(node->parent->tok->loc);

    if (owner_sym->node->kind != AST_DECL_MOD)
      report_error_expected_module(node->parent->tok->loc);

    node->parent = owner_sym->node;
    break;
  }
  default:
    unreachable();
  }

  assert(node->parent->kind == AST_DECL_MOD);

  string_view_t id_view = token_to_string_view(node->member->tok);
  symbol_t* member_sym = symtable_get_with_str_view(((ast_decl_mod_t*)node->parent)->scope, id_view);

  if (member_sym == NULL)
    report_error_no_member_with_name(node->member->tok->loc);

  if (member_sym->node->kind != AST_DECL_MOD)
  {
    ast_type_id_t* type_node = ast_type_id_init();
    type_node->tok = node->tok;
    type_node->decl = member_sym->node;

    typedesc_t* decl_desc = typetable_lookup(analyzer->typetable, type_node->decl);
    assert(decl_desc != NULL);

    typetable_insert(analyzer->typetable, (ast_node_t*)type_node, decl_desc);

    return (ast_node_t*)type_node;
  }

  return member_sym->node;
}

ast_node_t* analyzer_visit_type_id(analyzer_t* analyzer, symtable_t* scope, ast_id_t* node)
{
  string_view_t id_view = token_to_string_view(node->tok);
  symbol_t* id_sym = symtable_lookup_with_str_view(scope, id_view);
  
  if (id_sym == NULL)
    report_error_undefined_typename(node->tok->loc);

  if (id_sym->node->kind != AST_DECL_STRUCT &&
      id_sym->node->kind != AST_DECL_UNION &&
      id_sym->node->kind != AST_DECL_ENUM)
    report_error_symbol_is_not_a_typename(node->tok->loc);

  ast_type_id_t* type_node = ast_type_id_init();
  type_node->tok = node->tok;
  type_node->decl = id_sym->node;

  typedesc_t* decl_desc = typetable_lookup(analyzer->typetable, type_node->decl);
  assert(decl_desc != NULL);

  typetable_insert(analyzer->typetable, (ast_node_t*)type_node, decl_desc);

  return (ast_node_t*)type_node;
}

void analyzer_visit_type_mut(analyzer_t* analyzer, symtable_t* scope, ast_type_mut_t* node)
{
  node->base_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->base_type);

  typedesc_t* base_type = typetable_lookup(analyzer->typetable, node->base_type);
  assert(base_type != NULL);

  assert(typedesc_can_add_mut(base_type));

  typedesc_t* desc = typebuilder_build_mut(analyzer->typebuilder, base_type);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, desc);
}

void analyzer_visit_type_const(analyzer_t* analyzer, symtable_t* scope, ast_type_const_t* node)
{
  node->base_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->base_type);

  typedesc_t* base_type = typetable_lookup(analyzer->typetable, node->base_type);
  assert(base_type != NULL);

  assert(typedesc_can_add_const(base_type));

  typedesc_t* desc = typebuilder_build_const(analyzer->typebuilder, base_type);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, desc);
}

void analyzer_visit_type_ptr(analyzer_t* analyzer, symtable_t* scope, ast_type_ptr_t* node)
{
  node->base_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->base_type);
  
  typedesc_t* base_type = typetable_lookup(analyzer->typetable, node->base_type);
  assert(base_type != NULL);
  
  assert(typedesc_can_add_ptr(base_type));

  typedesc_t* desc = typebuilder_build_ptr(analyzer->typebuilder, base_type);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, desc);
}

void analyzer_visit_type_array(analyzer_t* analyzer, symtable_t* scope, ast_type_array_t* node)
{
  node->base_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->base_type);

  typedesc_t* base_type = typetable_lookup(analyzer->typetable, node->base_type);
  assert(base_type != NULL);

  int64_t size_value = 0;

  if (node->size != NULL)
  {
    node->size = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->size);

    typedesc_t* size_desc = typetable_lookup(analyzer->typetable, node->size);

    if (!typedesc_is_integer(size_desc))
      report_error_expected_integer_type(node->size->tok->loc);

    assert(node->size->kind == AST_EXPR_LIT_INT);

    size_value = strtoll(node->size->tok->loc->ptr, NULL, 0);
  }

  assert(size_value > 0);
  assert(typedesc_can_add_array(base_type));

  typedesc_t* desc = typebuilder_build_array(analyzer->typebuilder, (size_t)size_value, base_type);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, desc);
}

void analyzer_visit_type_ref(analyzer_t* analyzer, symtable_t* scope, ast_type_ref_t* node)
{
  node->base_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->base_type);

  typedesc_t* base_type = typetable_lookup(analyzer->typetable, node->base_type);
  assert(base_type != NULL);
  
  assert(typedesc_can_add_ref(base_type));

  typedesc_t* desc = typebuilder_build_ref(analyzer->typebuilder, base_type);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, desc);
}

void analyzer_visit_type_opt(analyzer_t* analyzer, symtable_t* scope, ast_type_opt_t* node)
{
  node->base_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->base_type);

  typedesc_t* base_type = typetable_lookup(analyzer->typetable, node->base_type);
  assert(base_type != NULL);

  assert(typedesc_can_add_opt(base_type));

  typedesc_t* desc = typebuilder_build_opt(analyzer->typebuilder, base_type);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, desc);
}

void analyzer_visit_type_fun(analyzer_t* analyzer, symtable_t* scope, ast_type_fun_t* node)
{
  node->return_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->return_type);

  VECTOR_FOR_LOOP(i, node->params)
    vector_set(node->params, i, analyzer_visit_type(analyzer, scope, (ast_type_t*)vector_get(node->params, i)));

  typedesc_t* return_type = typetable_lookup(analyzer->typetable, node->return_type);
  assert(return_type != NULL);

  typedesc_t** param_types = (typedesc_t**)malloc(sizeof(typedesc_t*) * vector_size(node->params));

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_node_t* param = (ast_node_t*)vector_get(node->params, i);

    typedesc_t* param_desc = typetable_lookup(analyzer->typetable, param);
    assert(param_desc != NULL);

    param_types[i] = param_desc;
  }

  typedesc_t* desc = typebuilder_build_fun(analyzer->typebuilder, return_type, param_types, vector_size(node->params), node->is_vararg, node->callconv);

  free(param_types);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, desc);
}

ast_node_t* analyzer_visit_type(analyzer_t* analyzer, symtable_t* scope, ast_type_t* node)
{
  switch (node->kind)
  {
  case AST_TYPE_ID:         return analyzer_visit_type_id(analyzer, scope, (ast_id_t*)node);
  case AST_TYPE_MUT:        analyzer_visit_type_mut  (analyzer, scope, (ast_type_mut_t*  )node); break;
  case AST_TYPE_CONST:      analyzer_visit_type_const(analyzer, scope, (ast_type_const_t*)node); break;
  case AST_TYPE_PTR:        analyzer_visit_type_ptr  (analyzer, scope, (ast_type_ptr_t*  )node); break;
  case AST_TYPE_ARRAY:      analyzer_visit_type_array(analyzer, scope, (ast_type_array_t*)node); break;
  case AST_TYPE_REF:        analyzer_visit_type_ref  (analyzer, scope, (ast_type_ref_t*  )node); break;
  case AST_TYPE_OPT:        analyzer_visit_type_opt  (analyzer, scope, (ast_type_opt_t*  )node); break;
  case AST_TYPE_FUN:        analyzer_visit_type_fun  (analyzer, scope, (ast_type_fun_t*  )node); break;
  case AST_TYPE_PRIM_I8:    typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_i8   (analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_I16:   typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_i16  (analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_I32:   typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_i32  (analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_I64:   typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_i64  (analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_ISIZE: typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_isize(analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_U8:    typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_u8   (analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_U16:   typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_u16  (analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_U32:   typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_u32  (analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_U64:   typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_u64  (analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_USIZE: typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_usize(analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_F32:   typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_f32  (analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_F64:   typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_f64  (analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_BOOL:  typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_bool (analyzer->typebuilder)); break;
  case AST_TYPE_PRIM_UNIT:  typetable_insert(analyzer->typetable, (ast_node_t*)node, typebuilder_build_unit (analyzer->typebuilder)); break;
  case AST_TYPE_MEMBER: return analyzer_visit_type_member(analyzer, scope, (ast_type_mbr_t*)node);
  default: unreachable();
  }

  return (ast_node_t*)node;
}

void analyzer_visit_stmt_if(analyzer_t* analyzer, symtable_t* scope, ast_stmt_if_t* node)
{
  symtable_t* if_scope = symtable_init(scope);

  node->cond = analyzer_visit_expr(analyzer, if_scope, (ast_expr_t*)node->cond);

  typedesc_t* cond_desc = typetable_lookup(analyzer->typetable, node->cond);
  assert(cond_desc != NULL);

  if (typedesc_remove_const_ref_mut(cond_desc)->kind != TYPEDESC_BOOL)
    report_error_expected_bool_type(node->cond->tok->loc);

  analyzer_visit_stmt(analyzer, if_scope, (ast_stmt_t*)node->stmt);

  if (node->stmt_else != NULL)
    analyzer_visit_stmt(analyzer, if_scope, (ast_stmt_t*)node->stmt_else);
}

void analyzer_visit_stmt_for(analyzer_t* analyzer, symtable_t* scope, ast_stmt_for_t* node)
{
  symtable_t* for_scope = symtable_init(scope);

  analyzer_visit_decl_var(analyzer, for_scope, (ast_decl_var_t*)node->var);
  
  node->range = analyzer_visit_expr(analyzer, for_scope, (ast_expr_t*)node->range);

  typedesc_t* range_desc = typetable_lookup(analyzer->typetable, node->range);
  assert(range_desc != NULL);

  analyzer_scope_push(analyzer, (ast_node_t*)node);
  analyzer_visit_stmt(analyzer, for_scope, (ast_stmt_t*)node->stmt);
  analyzer_scope_pop(analyzer);
}

void analyzer_visit_stmt_while(analyzer_t* analyzer, symtable_t* scope, ast_stmt_while_t* node)
{
  symtable_t* while_scope = symtable_init(scope);

  node->cond = analyzer_visit_expr(analyzer, while_scope, (ast_expr_t*)node->cond);
  
  typedesc_t* cond_desc = typetable_lookup(analyzer->typetable, node->cond);
  assert(cond_desc != NULL);

  if (typedesc_remove_const_ref_mut(cond_desc)->kind != TYPEDESC_BOOL)
    report_error_expected_bool_type(node->cond->tok->loc);

  analyzer_scope_push(analyzer, (ast_node_t*)node);
  analyzer_visit_stmt(analyzer, while_scope, (ast_stmt_t*)node->stmt);
  analyzer_scope_pop(analyzer);
}

void analyzer_visit_stmt_break(analyzer_t* analyzer, symtable_t* scope, ast_stmt_break_t* node)
{
  unused(scope);

  if (!analyzer_scope_can_early_exit_loop(analyzer))
    report_error_break_outside_loop(node->tok->loc);

  node->loop = analyzer_scope_innermost_loop(analyzer);
}

void analyzer_visit_stmt_continue(analyzer_t* analyzer, symtable_t* scope, ast_stmt_continue_t* node)
{
  unused(scope);

  if (!analyzer_scope_can_early_exit_loop(analyzer))
    report_error_continue_outside_loop(node->tok->loc);

  node->loop = analyzer_scope_innermost_loop(analyzer);
}

typedesc_t* analyzer_visit_stmt_return(analyzer_t* analyzer, symtable_t* scope, ast_stmt_return_t* node)
{
  if (analyzer_scope_innermost_fun(analyzer) == NULL)
    report_error_return_outside_function(node->tok->loc);

  if (!analyzer_scope_can_return(analyzer))
    report_error_return_inside_defer(node->tok->loc);

  typedesc_t* expr_desc = typebuilder_build_unit(analyzer->typebuilder);

  if (node->expr != NULL)
  {
    node->expr = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->expr);
    
    expr_desc = typetable_lookup(analyzer->typetable, node->expr);
    assert(expr_desc != NULL);
  }

  ast_decl_fun_t* fun_node = (ast_decl_fun_t*)analyzer_scope_innermost_fun(analyzer);

  typedesc_t* expected_desc = typetable_lookup(analyzer->typetable, fun_node->return_type);
  assert(expected_desc != NULL);

  if (!typedesc_is_implicitly_convertible(expr_desc, expected_desc))
    report_error_incompatible_return_type(node->tok->loc);

  return expr_desc;
}

void analyzer_visit_stmt_defer(analyzer_t* analyzer, symtable_t* scope, ast_stmt_defer_t* node)
{
  analyzer_visit_stmt(analyzer, scope, (ast_stmt_t*)node->stmt);
}

void analyzer_visit_stmt_block(analyzer_t* analyzer, symtable_t* scope, ast_stmt_block_t* node)
{
  symtable_t* block_scope = symtable_init(scope);

  VECTOR_FOR_LOOP(i, node->stmts)
    analyzer_visit_stmt(analyzer, block_scope, (ast_stmt_t*)vector_get(node->stmts, i));
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
  case AST_DECL_VAR:      analyzer_visit_decl_var     (analyzer, scope, (ast_decl_var_t*     )node); break;
  case AST_STMT_IF:       analyzer_visit_stmt_if      (analyzer, scope, (ast_stmt_if_t*      )node); break;
  case AST_STMT_FOR:      analyzer_visit_stmt_for     (analyzer, scope, (ast_stmt_for_t*     )node); break;
  case AST_STMT_WHILE:    analyzer_visit_stmt_while   (analyzer, scope, (ast_stmt_while_t*   )node); break;
  case AST_STMT_BREAK:    analyzer_visit_stmt_break   (analyzer, scope, (ast_stmt_break_t*   )node); break;
  case AST_STMT_CONTINUE: analyzer_visit_stmt_continue(analyzer, scope, (ast_stmt_continue_t*)node); break;
  case AST_STMT_RETURN:   analyzer_visit_stmt_return  (analyzer, scope, (ast_stmt_return_t*  )node); break;
  case AST_STMT_DEFER:    analyzer_visit_stmt_defer   (analyzer, scope, (ast_stmt_defer_t*   )node); break;
  case AST_STMT_BLOCK:    analyzer_visit_stmt_block   (analyzer, scope, (ast_stmt_block_t*   )node); break;
  case AST_STMT_EXPR:     analyzer_visit_stmt_expr    (analyzer, scope, (ast_stmt_expr_t*    )node); break;
  default: unreachable();
  }
}

void analyzer_visit_decl_var(analyzer_t* analyzer, symtable_t* scope, ast_decl_var_t* node)
{
  node->type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->type);

  typedesc_t* var_desc = typetable_lookup(analyzer->typetable, node->type);
  assert(var_desc != NULL);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, var_desc);

  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* var_sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup_with_str_view(scope, id_view);
  symbol_t* collision = symtable_insert(scope, var_sym);

  if (collision != NULL && collision->node->kind == AST_DECL_VAR)
    report_error_variable_redeclaration(node->id->tok->loc, collision->node->tok->loc);

  if (lookup != NULL && lookup->node->kind == AST_DECL_VAR)
    report_warning_shadowed_variable(node->id->tok->loc);

  if (node->expr != NULL)
  {
    node->expr = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->expr);

    typedesc_t* expr_desc = typetable_lookup(analyzer->typetable, node->expr);
    assert(expr_desc != NULL);

    if (!typedesc_is_implicitly_convertible(expr_desc, var_desc))
      report_error_type_mismatch(node->tok->loc, var_desc, expr_desc);
  }
}

void analyzer_visit_decl_param(analyzer_t* analyzer, symtable_t* scope, ast_decl_param_t* node)
{
  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* param_sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);
  symbol_t* collision = symtable_insert(scope, param_sym);

  if (collision != NULL && collision->node->kind == AST_DECL_PARAM)
    report_error_parameter_redefinition(node->id->tok->loc, collision->node->tok->loc);

  node->type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->type);

  typedesc_t* param_desc = typetable_lookup(analyzer->typetable, node->type);
  assert(param_desc != NULL);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, param_desc);

  if (node->expr != NULL)
  {
    node->expr = analyzer_visit_expr(analyzer, scope, (ast_expr_t*)node->expr);

    typedesc_t* expr_desc = typetable_lookup(analyzer->typetable, node->expr);
    assert(expr_desc != NULL);

    if (!typedesc_is_implicitly_convertible(expr_desc, param_desc))
      report_error_type_mismatch(node->tok->loc, param_desc, expr_desc);
  }
}

void analyzer_visit_decl_fun(analyzer_t* analyzer, symtable_t* scope, ast_decl_fun_t* node)
{
  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* fun_sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup_with_str_view(scope, id_view);
  symbol_t* collision = symtable_insert(scope, fun_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* fun_scope = symtable_init(scope);

  size_t param_count = vector_size(node->params);
  typedesc_t** param_types = NULL;

  if (param_count > 0)
  {
    param_types = (typedesc_t**)malloc(sizeof(typedesc_t*) * param_count);

    VECTOR_FOR_LOOP(i, node->params)
    {
      ast_decl_param_t* param = (ast_decl_param_t*)vector_get(node->params, i);
      analyzer_visit_decl_param(analyzer, fun_scope, param);

      typedesc_t* param_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)param);
      assert(param_desc != NULL);

      param_types[i] = param_desc;
    }
  }

  node->return_type = analyzer_visit_type(analyzer, scope, (ast_type_t*)node->return_type);

  typedesc_t* return_desc = typetable_lookup(analyzer->typetable, node->return_type);
  assert(return_desc != NULL);

  typedesc_t* fun_desc = typebuilder_build_fun(analyzer->typebuilder, return_desc, param_types, param_count, node->is_vararg, node->callconv);

  free(param_types);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, fun_desc);

  if (!node->is_extern)
  {
    analyzer_scope_push(analyzer, (ast_node_t*)node);
    analyzer_visit_stmt(analyzer, fun_scope, (ast_stmt_t*)node->stmt);
    analyzer_scope_pop(analyzer);
  }
}

void analyzer_visit_decl_struct(analyzer_t* analyzer, symtable_t* scope, ast_decl_struct_t* node)
{
  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* struct_sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup_with_str_view(scope, id_view);
  symbol_t* collision = symtable_insert(scope, struct_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  node->scope = symtable_init(scope);

  typedesc_t** field_types = (typedesc_t**)malloc(sizeof(typedesc_t*) * vector_size(node->members));

  VECTOR_FOR_LOOP(i, node->members)
  {
    ast_decl_var_t* var = (ast_decl_var_t*)vector_get(node->members, i);
    analyzer_visit_decl_var(analyzer, node->scope, var);

    typedesc_t* var_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)var);
    assert(var_desc != NULL);

    field_types[i] = var_desc;
  }

  typedesc_t* struct_desc = typebuilder_build_struct(analyzer->typebuilder, (ast_node_t*)node, field_types, vector_size(node->members));

  free(field_types);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, struct_desc);
}

void analyzer_visit_decl_union(analyzer_t* analyzer, symtable_t* scope, ast_decl_union_t* node)
{
  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* union_sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup_with_str_view(scope, id_view);
  symbol_t* collision = symtable_insert(scope, union_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  node->scope = symtable_init(scope);

  typedesc_t** field_types = (typedesc_t**)malloc(sizeof(typedesc_t*) * vector_size(node->members));

  VECTOR_FOR_LOOP(i, node->members)
  {
    ast_decl_var_t* var = (ast_decl_var_t*)vector_get(node->members, i);
    analyzer_visit_decl_var(analyzer, node->scope, var);

    typedesc_t* var_desc = typetable_lookup(analyzer->typetable, (ast_node_t*)var);
    assert(var_desc != NULL);

    field_types[i] = var_desc;
  }

  typedesc_t* union_desc = typebuilder_build_union(analyzer->typebuilder, (ast_node_t*)node, field_types, vector_size(node->members));

  free(field_types);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, union_desc);
}

void analyzer_visit_decl_enum(analyzer_t* analyzer, symtable_t* scope, ast_decl_enum_t* node)
{
  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* enum_sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);
  symbol_t* lookup = symtable_lookup_with_str_view(scope, id_view);
  symbol_t* collision = symtable_insert(scope, enum_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  node->scope = symtable_init(scope);

  typedesc_t* enum_desc = typebuilder_build_enum(analyzer->typebuilder, (ast_node_t*)node);

  VECTOR_FOR_LOOP(i, node->members)
    analyzer_visit_decl_enum_constant(analyzer, node->scope, (typedesc_enum_t*)enum_desc, (ast_decl_enum_constant_t*)vector_get(node->members, i));

  typetable_insert(analyzer->typetable, (ast_node_t*)node, enum_desc);
}

void analyzer_visit_decl_enum_constant(analyzer_t* analyzer, symtable_t* scope, typedesc_enum_t* enum_desc, ast_decl_enum_constant_t* node)
{
  unused(analyzer);

  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* enum_constant_sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);
  symbol_t* collision = symtable_insert(scope, enum_constant_sym);

  if (collision != NULL)
    report_error_enumerator_redeclaration(node->id->tok->loc, collision->node->tok->loc);

  typetable_insert(analyzer->typetable, (ast_node_t*)node, (typedesc_t*)enum_desc);
}

void analyzer_visit_decl_mod(analyzer_t* analyzer, symtable_t* scope, ast_decl_mod_t* node)
{
  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* mod_sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);
  symbol_t* collision = symtable_insert(scope, mod_sym);

  if (collision != NULL)
    report_error_enumerator_redeclaration(node->id->tok->loc, collision->node->tok->loc);

  node->scope = symtable_init(scope);

  VECTOR_FOR_LOOP(i, node->members)
  {
    ast_decl_t* decl = (ast_decl_t*)vector_get(node->members, i);
    analyzer_visit_decl(analyzer, node->scope, decl);
  }
}

void analyzer_visit_decl(analyzer_t* analyzer, symtable_t* scope, ast_decl_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:    analyzer_visit_decl_var   (analyzer, scope, (ast_decl_var_t*   )node); break;
  case AST_DECL_PARAM:  analyzer_visit_decl_param (analyzer, scope, (ast_decl_param_t* )node); break;
  case AST_DECL_FUN:    analyzer_visit_decl_fun   (analyzer, scope, (ast_decl_fun_t*   )node); break;
  case AST_DECL_STRUCT: analyzer_visit_decl_struct(analyzer, scope, (ast_decl_struct_t*)node); break;
  case AST_DECL_UNION:  analyzer_visit_decl_union (analyzer, scope, (ast_decl_union_t* )node); break;
  case AST_DECL_ENUM:   analyzer_visit_decl_enum  (analyzer, scope, (ast_decl_enum_t*  )node); break;
  case AST_DECL_MOD:    analyzer_visit_decl_mod   (analyzer, scope, (ast_decl_mod_t*   )node); break;
  default: unreachable();
  }
}

void analyzer_visit_prog(analyzer_t* analyzer, symtable_t* scope, ast_prog_t* node)
{
  symtable_t* prog_scope = symtable_init(scope);

  VECTOR_FOR_LOOP(i, node->decls)
    analyzer_visit_decl(analyzer, prog_scope, (ast_decl_t*)vector_get(node->decls, i));
}

void analyzer_analyze(analyzer_t* analyzer, symtable_t* symtable, typetable_t* typetable, typebuilder_t* typebuilder, ast_node_t* node)
{
  assert(node->kind == AST_PROG);

  analyzer->symtable = symtable;
  analyzer->typetable = typetable;
  analyzer->typebuilder = typebuilder;

  analyzer_visit_prog(analyzer, analyzer->symtable, (ast_prog_t*)node);
}
