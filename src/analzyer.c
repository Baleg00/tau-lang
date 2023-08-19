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

void analyzer_visit_expr_op_is(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_lhs(node, analyzer_visit_expr(analyzer, scope, ast_get_lhs(node)));
  ast_set_rhs(node, analyzer_visit_type(analyzer, scope, ast_get_rhs(node)));

  typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_BOOL));
}

void analyzer_visit_expr_op_as(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_lhs(node, analyzer_visit_expr(analyzer, scope, ast_get_lhs(node)));
  ast_set_rhs(node, analyzer_visit_type(analyzer, scope, ast_get_rhs(node)));

  typedesc_t* type_rhs = typetable_lookup(analyzer->typetable, ast_get_rhs(node));
  assert(type_rhs != NULL);
  
  // TODO: check convertability

  typetable_insert(analyzer->typetable, node, type_rhs);
}

void analyzer_visit_expr_op_unary(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_expr(node, analyzer_visit_expr(analyzer, scope, ast_get_expr(node)));

  typedesc_t* param_desc = typetable_lookup(analyzer->typetable, ast_get_expr(node));
  assert(param_desc != NULL);

  switch (ast_get_op(node))
  {
  case OP_SIZEOF:
  case OP_ALIGNOF:
    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_USIZE));
    return;
  case OP_TYPEOF:
    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_TYPE));
    return;
  case OP_ARIT_INC_PRE:
  case OP_ARIT_DEC_PRE:
    if (!typedesc_is_ref(param_desc))
      report_error_expected_reference_type(token_get_loc(ast_get_token(node)));

    if (!typedesc_is_arithmetic(typedesc_get_base(param_desc)) && !typedesc_is_ptr(typedesc_get_base(param_desc)))
      report_error_expected_arithmetic_type(token_get_loc(ast_get_token(node)));

    typetable_insert(analyzer->typetable, node, param_desc);
    return;
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_POST:
    if (!typedesc_is_ref(param_desc))
      report_error_expected_reference_type(token_get_loc(ast_get_token(node)));

    if (!typedesc_is_arithmetic(typedesc_get_base(param_desc)) && !typedesc_is_ptr(typedesc_get_base(param_desc)))
      report_error_expected_arithmetic_type(token_get_loc(ast_get_token(node)));

    typetable_insert(analyzer->typetable, node, typedesc_get_base(param_desc));
    return;
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_BIT_NOT:
    param_desc = typedesc_remove_ref(param_desc);

    if (!typedesc_is_arithmetic(param_desc))
      report_error_expected_arithmetic_type(token_get_loc(ast_get_token(node)));

    typetable_insert(analyzer->typetable, node, param_desc);
    return;
  case OP_LOGIC_NOT:
    param_desc = typedesc_remove_ref(param_desc);

    if (!typedesc_is_bool(param_desc))
      report_error_expected_bool_type(token_get_loc(ast_get_token(node)));

    typetable_insert(analyzer->typetable, node, param_desc);
    return;
  case OP_IND:
    param_desc = typedesc_remove_ref(param_desc);

    if (!typedesc_is_ptr(param_desc))
      report_error_expected_ptr_type(token_get_loc(ast_get_token(node)));
    
    typetable_insert(analyzer->typetable, node, typedesc_add_ref(typedesc_get_base(param_desc)));
    return;
  case OP_ADDR:
    if (!typedesc_is_ref(param_desc))
      report_error_expected_reference_type(token_get_loc(ast_get_token(node)));

    typetable_insert(analyzer->typetable, node, typedesc_add_ptr(typedesc_get_base(param_desc)));
    return;
  default:
    unreachable();
  }
}

void analyzer_visit_expr_op_binary(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  if (ast_get_op(node) == OP_IS)
    analyzer_visit_expr_op_is(analyzer, scope, node);
  else if (ast_get_op(node) == OP_AS)
    analyzer_visit_expr_op_as(analyzer, scope, node);
  else
  {
    ast_set_lhs(node, analyzer_visit_expr(analyzer, scope, ast_get_lhs(node)));
    typedesc_t* lhs_desc = typetable_lookup(analyzer->typetable, ast_get_lhs(node));
    assert(lhs_desc != NULL);

    ast_set_rhs(node, analyzer_visit_expr(analyzer, scope, ast_get_rhs(node)));
    typedesc_t* rhs_desc = typetable_lookup(analyzer->typetable, ast_get_rhs(node));
    assert(rhs_desc != NULL);

    switch (ast_get_op(node))
    {
    case OP_IN:
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (!typedesc_is_gen(rhs_desc))
        report_error_expected_generator_type(token_get_loc(ast_get_token(ast_get_rhs(node))));

      if (typedesc_is_same(lhs_desc, typedesc_get_yield(rhs_desc)))
        report_error_type_mismatch(token_get_loc(ast_get_token(node)), lhs_desc, rhs_desc);

      typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_BOOL));
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
        report_error_expected_arithmetic_type(token_get_loc(ast_get_token(ast_get_lhs(node))));

      if (!typedesc_is_arithmetic(rhs_desc))
        report_error_expected_arithmetic_type(token_get_loc(ast_get_token(ast_get_rhs(node))));

      if (typedesc_is_signed(lhs_desc) != typedesc_is_signed(rhs_desc))
        report_warning_mixed_signedness(token_get_loc(ast_get_token(node)));

      // TODO: type promotion
      typetable_insert(analyzer->typetable, node, lhs_desc);
      break;
    case OP_BIT_LSH:
    case OP_BIT_RSH:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (!typedesc_is_integer(lhs_desc))
        report_error_expected_integer_type(token_get_loc(ast_get_token(ast_get_lhs(node))));

      if (!typedesc_is_integer(rhs_desc))
        report_error_expected_integer_type(token_get_loc(ast_get_token(ast_get_rhs(node))));

      typetable_insert(analyzer->typetable, node, lhs_desc);
      break;
    case OP_LOGIC_AND:
    case OP_LOGIC_OR:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (!typedesc_is_bool(lhs_desc))
        report_error_expected_bool_type(token_get_loc(ast_get_token(ast_get_lhs(node))));

      if (!typedesc_is_bool(rhs_desc))
        report_error_expected_bool_type(token_get_loc(ast_get_token(ast_get_rhs(node))));

      typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_BOOL));
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
        report_error_expected_arithmetic_type(token_get_loc(ast_get_token(ast_get_lhs(node))));

      if (!typedesc_is_arithmetic(rhs_desc))
        report_error_expected_arithmetic_type(token_get_loc(ast_get_token(ast_get_rhs(node))));

      typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_BOOL));
      break;
    case OP_SUBS:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (!typedesc_is_array(lhs_desc))
        report_error_expected_subscriptable(token_get_loc(ast_get_token(ast_get_lhs(node))));

      if (!typedesc_is_integer(rhs_desc))
        report_error_expected_integer_type(token_get_loc(ast_get_token(ast_get_rhs(node))));

      typetable_insert(analyzer->typetable, node, typedesc_add_ref(typedesc_get_base(lhs_desc)));
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
      if (!typedesc_is_ref(lhs_desc))
        report_error_expected_reference_type(token_get_loc(ast_get_token(ast_get_lhs(node))));

      if (!typedesc_is_same(typedesc_remove_ref(lhs_desc), typedesc_remove_ref(rhs_desc)))
        report_error_type_mismatch(token_get_loc(ast_get_token(node)), lhs_desc, rhs_desc);

      typetable_insert(analyzer->typetable, node, lhs_desc);
      break;
    case OP_RANGE:
      lhs_desc = typedesc_remove_ref(lhs_desc);
      rhs_desc = typedesc_remove_ref(rhs_desc);

      if (!typedesc_is_integer(lhs_desc))
        report_error_expected_integer_type(token_get_loc(ast_get_token(ast_get_lhs(node))));

      if (!typedesc_is_integer(rhs_desc))
        report_error_expected_integer_type(token_get_loc(ast_get_token(ast_get_rhs(node))));

      if (!typedesc_is_same(lhs_desc, rhs_desc))
        report_error_type_mismatch(token_get_loc(ast_get_token(node)), lhs_desc, rhs_desc);

      typedesc_t* range_desc = typedesc_init(TYPEDESC_GEN);
      typedesc_set_params(range_desc, NULL);
      typedesc_set_yield(range_desc, lhs_desc);

      typetable_insert(analyzer->typetable, node, range_desc);
      break;
    case OP_SEMICOLON: // TODO
    default:
      unreachable();
    }
  }
}

void analyzer_visit_expr_op_call(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_callee(node, analyzer_visit_expr(analyzer, scope, ast_get_callee(node)));
  typedesc_t* callee_desc = typetable_lookup(analyzer->typetable, ast_get_callee(node));
  assert(callee_desc != NULL);

  list_t* param_types = NULL;
  typedesc_t* return_type = NULL;

  if (typedesc_is_fun(callee_desc))
  {
    param_types = typedesc_get_params(callee_desc);
    return_type = typedesc_get_return(callee_desc);

    if (ast_get_params(node) == NULL)
    {
      if (param_types != NULL && list_size(param_types) != 0)
        report_error_too_few_arguments(token_get_loc(ast_get_token(node)));
    }
    else
    {
      if (param_types == NULL)
      {
        if (list_size(ast_get_params(node)) != 0)
          report_error_too_many_arguments(token_get_loc(ast_get_token(node)));
      }
      else
      {
        if (list_size(ast_get_params(node)) < list_size(param_types))
          report_error_too_few_arguments(token_get_loc(ast_get_token(node)));

        if (list_size(ast_get_params(node)) > list_size(param_types))
          report_error_too_many_arguments(token_get_loc(ast_get_token(node)));
      }
    }
  }
  else if (typedesc_is_gen(callee_desc))
  {
    if (list_size(ast_get_params(node)) > list_size(typedesc_get_params(callee_desc)))
      report_error_too_many_arguments(token_get_loc(ast_get_token(node)));
    else if (list_size(ast_get_params(node)) < list_size(typedesc_get_params(callee_desc)))
      report_error_too_few_arguments(token_get_loc(ast_get_token(node)));

    param_types = typedesc_get_params(callee_desc);
    return_type = typedesc_get_yield(callee_desc);
  }
  else
    report_error_expected_callable(token_get_loc(ast_get_token(node)));

  if (param_types != NULL)
    for (list_node_t *param_it      = list_front_node(ast_get_params(node)),
                     *param_type_it = list_front_node(param_types);
          param_it != NULL && param_type_it != NULL;
          param_it      = list_node_next(param_it),
          param_type_it = list_node_next(param_type_it))
    {
      ast_node_t* param = (ast_node_t*)list_node_get(param_it);
      param = analyzer_visit_expr(analyzer, scope, param);
      list_node_set(param_it, param);

      // typedesc_t* expected_param_desc = (typedesc_t*)list_node_get(param_type_it);

      typedesc_t* param_desc = typetable_lookup(analyzer->typetable, param);
      assert(param_desc != NULL);

      // TODO: check assignability
    }

  typetable_insert(analyzer->typetable, node, return_type);
}

void analyzer_visit_expr_op_member(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  if (ast_get_kind(ast_get_rhs(node)) != AST_ID)
    report_error_expected_member(token_get_loc(ast_get_token(ast_get_rhs(node))));

  ast_set_lhs(node, analyzer_visit_expr(analyzer, scope, ast_get_lhs(node)));

  typedesc_t* lhs_desc = typetable_lookup(analyzer->typetable, ast_get_lhs(node));
  assert(lhs_desc != NULL);

  typedesc_t* owner_desc = typedesc_remove_ref(lhs_desc);

  switch (ast_get_op(node))
  {
  case OP_MEMBER:
    if (!typedesc_is_composite(owner_desc))
      report_error_expected_owner(token_get_loc(ast_get_token(ast_get_lhs(node))));
    break;
  case OP_IND_MEMBER:
    if (!typedesc_is_ptr(owner_desc))
      report_error_expected_ptr_type(token_get_loc(ast_get_token(ast_get_lhs(node))));

    if (!typedesc_is_composite(typedesc_get_base(owner_desc)))
      report_error_expected_ptr_to_owner(token_get_loc(ast_get_token(ast_get_lhs(node))));

    owner_desc = typedesc_get_base(owner_desc);
    break;
  case OP_NULL_SAFE_MEMBER:
    if (!typedesc_is_opt(owner_desc))
      report_error_expected_optional_type(token_get_loc(ast_get_token(ast_get_lhs(node))));

    if (!typedesc_is_composite(typedesc_get_base(owner_desc)))
      report_error_expected_owner(token_get_loc(ast_get_token(ast_get_lhs(node))));

    owner_desc = typedesc_get_base(owner_desc);
    break;
  default:
    unreachable();
  }

  token_t* tok_rhs = ast_get_token(ast_get_rhs(node));
  
  switch (typedesc_get_kind(owner_desc))
  {
  case TYPEDESC_STRUCT:
    LIST_FOR_LOOP(it, ast_get_members(typedesc_get_node(owner_desc)))
    {
      ast_node_t* member = (ast_node_t*)list_node_get(it);
      location_t* id_loc = token_get_loc(ast_get_token(ast_get_id(member)));

      if (strncmp(location_get_ptr(id_loc), location_get_ptr(token_get_loc(tok_rhs)), location_get_len(id_loc)) == 0)
      {
        ast_set_rhs(node, member);

        typedesc_t* member_desc = typetable_lookup(analyzer->typetable, member);
        assert(member_desc != NULL);

        typetable_insert(analyzer->typetable, node, member_desc);
        return;
      }
    }

    report_error_no_member_with_name(token_get_loc(tok_rhs));
    break;
  case TYPEDESC_UNION:
    LIST_FOR_LOOP(it, ast_get_members(typedesc_get_node(owner_desc)))
    {
      ast_node_t* member = (ast_node_t*)list_node_get(it);
      location_t* id_loc = token_get_loc(ast_get_token(ast_get_id(member)));

      if (strncmp(location_get_ptr(id_loc), location_get_ptr(token_get_loc(tok_rhs)), location_get_len(id_loc)) == 0)
      {
        ast_set_rhs(node, member);
        
        typedesc_t* member_desc = typetable_lookup(analyzer->typetable, member);
        assert(member_desc != NULL);

        typetable_insert(analyzer->typetable, node, member_desc);
        return;
      }
    }

    report_error_no_member_with_name(token_get_loc(tok_rhs));
    break;
  case TYPEDESC_ENUM:
    LIST_FOR_LOOP(it, ast_get_members(typedesc_get_node(owner_desc)))
    {
      ast_node_t* enum_constant = (ast_node_t*)list_node_get(it);
      location_t* id_loc = token_get_loc(ast_get_token(ast_get_id(enum_constant)));

      if (strncmp(location_get_ptr(id_loc), location_get_ptr(token_get_loc(tok_rhs)), location_get_len(id_loc)) == 0)
      {
        ast_set_rhs(node, enum_constant);

        typetable_insert(analyzer->typetable, node, owner_desc);
        return;
      }
    }

    report_error_no_member_with_name(token_get_loc(tok_rhs));
    break;
  case TYPEDESC_MOD:
    LIST_FOR_LOOP(it, ast_get_decls(typedesc_get_node(owner_desc)))
    {
      ast_node_t* decl = (ast_node_t*)list_node_get(it);
      location_t* id_loc = token_get_loc(ast_get_token(ast_get_id(decl)));

      if (strncmp(location_get_ptr(id_loc), location_get_ptr(token_get_loc(tok_rhs)), location_get_len(id_loc)) == 0)
      {
        ast_set_rhs(node, decl);

        typedesc_t* decl_desc = typetable_lookup(analyzer->typetable, decl);
        assert(decl_desc != NULL);

        typetable_insert(analyzer->typetable, node, decl_desc);
        return;
      }
    }

    report_error_no_member_with_name(token_get_loc(tok_rhs));
    break;
  default:
    report_error_expected_owner(token_get_loc(tok_rhs));
  }
}

void analyzer_visit_expr_op(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  if (ast_get_op(node) == OP_CALL)
    analyzer_visit_expr_op_call(analyzer, scope, node);
  else if (ast_get_op(node) == OP_MEMBER ||
    ast_get_op(node) == OP_IND_MEMBER ||
    ast_get_op(node) == OP_NULL_SAFE_MEMBER)
    analyzer_visit_expr_op_member(analyzer, scope, node);
  else if (op_is_unary(ast_get_op(node)))
    analyzer_visit_expr_op_unary(analyzer, scope, node);
  else if (op_is_binary(ast_get_op(node)))
    analyzer_visit_expr_op_binary(analyzer, scope, node);
  else
    unreachable();
}

ast_node_t* analyzer_visit_expr(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  switch (ast_get_kind(node))
  {
  case AST_ID:
  {
    token_t* id_tok = ast_get_token(node);
    symbol_t* id_sym = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
    
    if (id_sym == NULL)
      report_error_undefined_symbol(token_get_loc(id_tok));

    if (!ast_is_decl(id_sym->node))
      report_error_symbol_is_not_an_expression(token_get_loc(id_tok));

    return id_sym->node;
  }
  case AST_EXPR_LIT_INT:
    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_I32));
    break;
  case AST_EXPR_LIT_FLT:
    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_F32));
    break;
  case AST_EXPR_LIT_STR:
  {
    typedesc_t* str_desc = typedesc_init(TYPEDESC_PTR);
    typedesc_set_base(str_desc, typedesc_init(TYPEDESC_U8));
    
    typetable_insert(analyzer->typetable, node, str_desc);
    break;
  }
  case AST_EXPR_LIT_CHAR:
    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_U8));
    break;
  case AST_EXPR_LIT_BOOL:
    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_BOOL));
    break;
  case AST_EXPR_LIT_NULL:
    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_NULL));
    break;
  case AST_EXPR_OP:
    analyzer_visit_expr_op(analyzer, scope, node);
    break;
  default:
    unreachable();
  }

  return node;
}

ast_node_t* analyzer_visit_type_member(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  token_t* owner_id_tok = ast_get_token(ast_get_owner(node));
  symbol_t* owner_sym = symtable_lookup(scope, location_get_ptr(token_get_loc(owner_id_tok)), location_get_len(token_get_loc(owner_id_tok)));
  ast_node_t* member = ast_get_member(node);

  if (owner_sym == NULL)
    report_error_undefined_symbol(token_get_loc(owner_id_tok));

  if (ast_get_kind(owner_sym->node) != AST_DECL_MOD)
    report_error_expected_module(token_get_loc(owner_id_tok));

  if (ast_get_kind(member) == AST_ID)
  {
    token_t* member_id_tok = ast_get_token(member);
    symbol_t* member_sym = symtable_lookup(owner_sym->scope, location_get_ptr(token_get_loc(member_id_tok)), location_get_len(token_get_loc(member_id_tok)));

    if (member_sym == NULL)
      report_error_no_member_with_name(token_get_loc(member_id_tok));

    return member_sym->node;
  }
  else if (ast_get_kind(member) == AST_TYPE_MEMBER)
    return analyzer_visit_type_member(analyzer, owner_sym->scope, member);

  unreachable();
  
  return NULL;
}

static ast_node_t* analyzer_visit_type_id(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  unused(analyzer);

  token_t* id_tok = ast_get_token(node);
  symbol_t* id_sym = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
  
  if (id_sym == NULL)
    report_error_undefined_typename(token_get_loc(id_tok));

  if (ast_get_kind(id_sym->node) != AST_DECL_STRUCT &&
      ast_get_kind(id_sym->node) != AST_DECL_UNION &&
      ast_get_kind(id_sym->node) != AST_DECL_ENUM)
    report_error_symbol_is_not_a_typename(token_get_loc(id_tok));

  return id_sym->node;
}

static void analyzer_visit_type_mut(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_base_type(node, analyzer_visit_type(analyzer, scope, ast_get_base_type(node)));

  typedesc_t* desc = typedesc_init(TYPEDESC_MUT);

  typedesc_t* base_type = typetable_lookup(analyzer->typetable, ast_get_base_type(node));
  assert(base_type != NULL);
  
  typedesc_set_base(desc, base_type);

  typetable_insert(analyzer->typetable, node, desc);
}

static void analyzer_visit_type_const(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_base_type(node, analyzer_visit_type(analyzer, scope, ast_get_base_type(node)));

  typedesc_t* desc = typedesc_init(TYPEDESC_CONST);

  typedesc_t* base_type = typetable_lookup(analyzer->typetable, ast_get_base_type(node));
  assert(base_type != NULL);
  
  typedesc_set_base(desc, base_type);

  typetable_insert(analyzer->typetable, node, desc);
}

static void analyzer_visit_type_ptr(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_base_type(node, analyzer_visit_type(analyzer, scope, ast_get_base_type(node)));
  
  typedesc_t* desc = typedesc_init(TYPEDESC_PTR);

  typedesc_t* base_type = typetable_lookup(analyzer->typetable, ast_get_base_type(node));
  assert(base_type != NULL);
  
  typedesc_set_base(desc, base_type);

  typetable_insert(analyzer->typetable, node, desc);
}

static void analyzer_visit_type_array(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_base_type(node, analyzer_visit_type(analyzer, scope, ast_get_base_type(node)));
  
  typedesc_t* desc = typedesc_init(TYPEDESC_ARRAY);
  
  typedesc_t* base_type = typetable_lookup(analyzer->typetable, ast_get_base_type(node));
  assert(base_type != NULL);
  
  typedesc_set_base(desc, base_type);

  if (ast_get_size(node) != NULL)
    analyzer_visit_expr(analyzer, scope, ast_get_size(node));

  typetable_insert(analyzer->typetable, node, desc);
}

static void analyzer_visit_type_ref(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_base_type(node, analyzer_visit_type(analyzer, scope, ast_get_base_type(node)));

  typedesc_t* desc = typedesc_init(TYPEDESC_REF);

  typedesc_t* base_type = typetable_lookup(analyzer->typetable, ast_get_base_type(node));
  assert(base_type != NULL);
  
  typedesc_set_base(desc, base_type);

  typetable_insert(analyzer->typetable, node, desc);
}

static void analyzer_visit_type_opt(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_base_type(node, analyzer_visit_type(analyzer, scope, ast_get_base_type(node)));

  typedesc_t* desc = typedesc_init(TYPEDESC_OPT);

  typedesc_t* base_type = typetable_lookup(analyzer->typetable, ast_get_base_type(node));
  assert(base_type != NULL);
  
  typedesc_set_base(desc, base_type);

  typetable_insert(analyzer->typetable, node, desc);
}

static void analyzer_visit_type_fun(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_return_type(node, analyzer_visit_type(analyzer, scope, ast_get_return_type(node)));

  LIST_FOR_LOOP(it, ast_get_params(node))
    list_node_set(it, analyzer_visit_type(analyzer, scope, (ast_node_t*)list_node_get(it)));

  typedesc_t* desc = typedesc_init(TYPEDESC_FUN);

  typedesc_t* return_type = typetable_lookup(analyzer->typetable, ast_get_return_type(node));
  assert(return_type != NULL);

  typedesc_set_return(desc, return_type);

  typedesc_set_params(desc, NULL);

  if (ast_get_params(node) != NULL)
  {
    typedesc_set_params(desc, list_init());

    LIST_FOR_LOOP(it, ast_get_params(node))
    {
      ast_node_t* param = (ast_node_t*)list_node_get(it);

      typedesc_t* param_desc = typetable_lookup(analyzer->typetable, param);
      assert(param_desc != NULL);

      list_push_back(typedesc_get_params(desc), param_desc);
    }
  }
}

static void analyzer_visit_type_gen(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_yield_type(node, analyzer_visit_type(analyzer, scope, ast_get_yield_type(node)));

  LIST_FOR_LOOP(it, ast_get_params(node))
    list_node_set(it, analyzer_visit_type(analyzer, scope, (ast_node_t*)list_node_get(it)));

  typedesc_t* desc = typedesc_init(TYPEDESC_GEN);

  typedesc_t* yield_type = typetable_lookup(analyzer->typetable, ast_get_yield_type(node));
  assert(yield_type != NULL);

  typedesc_set_yield(desc, yield_type);

  typedesc_set_params(desc, NULL);

  if (ast_get_params(node) != NULL)
  {
    typedesc_set_params(desc, list_init());

    LIST_FOR_LOOP(it, ast_get_params(node))
    {
      ast_node_t* param = (ast_node_t*)list_node_get(it);

      typedesc_t* param_desc = typetable_lookup(analyzer->typetable, param);
      assert(param_desc != NULL);

      list_push_back(typedesc_get_params(desc), param_desc);
    }
  }
}

ast_node_t* analyzer_visit_type(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  switch (ast_get_kind(node))
  {
  case AST_ID:          return analyzer_visit_type_id(analyzer, scope, node);
  case AST_TYPE_MUT:    analyzer_visit_type_mut(analyzer, scope, node); break;
  case AST_TYPE_CONST:  analyzer_visit_type_const(analyzer, scope, node); break;
  case AST_TYPE_PTR:    analyzer_visit_type_ptr(analyzer, scope, node); break;
  case AST_TYPE_ARRAY:  analyzer_visit_type_array(analyzer, scope, node); break;
  case AST_TYPE_REF:    analyzer_visit_type_ref(analyzer, scope, node); break;
  case AST_TYPE_OPT:    analyzer_visit_type_opt(analyzer, scope, node); break;
  case AST_TYPE_FUN:    analyzer_visit_type_fun(analyzer, scope, node); break;
  case AST_TYPE_GEN:    analyzer_visit_type_gen(analyzer, scope, node); break;
  case AST_TYPE_I8:     typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_I8)); break;
  case AST_TYPE_I16:    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_I16)); break;
  case AST_TYPE_I32:    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_I32)); break;
  case AST_TYPE_I64:    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_I64)); break;
  case AST_TYPE_ISIZE:  typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_ISIZE)); break;
  case AST_TYPE_U8:     typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_U8)); break;
  case AST_TYPE_U16:    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_U16)); break;
  case AST_TYPE_U32:    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_U32)); break;
  case AST_TYPE_U64:    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_U64)); break;
  case AST_TYPE_USIZE:  typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_USIZE)); break;
  case AST_TYPE_F32:    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_F32)); break;
  case AST_TYPE_F64:    typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_F64)); break;
  case AST_TYPE_BOOL:   typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_BOOL)); break;
  case AST_TYPE_UNIT:   typetable_insert(analyzer->typetable, node, typedesc_init(TYPEDESC_UNIT)); break;
  case AST_TYPE_MEMBER: return analyzer_visit_type_member(analyzer, scope, node);
  default: unreachable();
  }

  return node;
}

void analyzer_visit_stmt_if(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  symtable_t* if_table = symtable_init(scope);

  ast_set_cond(node, analyzer_visit_expr(analyzer, if_table, ast_get_cond(node)));

  typedesc_t* cond_desc = typetable_lookup(analyzer->typetable, ast_get_cond(node));
  assert(cond_desc != NULL);

  cond_desc = typedesc_remove_ref(cond_desc);

  if (!typedesc_is_bool(cond_desc))
    report_error_expected_bool_type(token_get_loc(ast_get_token(ast_get_cond(node))));

  analyzer_visit_stmt(analyzer, if_table, ast_get_stmt(node));

  if (ast_get_stmt_else(node) != NULL)
    analyzer_visit_stmt(analyzer, if_table, ast_get_stmt_else(node));
}

void analyzer_visit_stmt_for(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  symtable_t* for_table = symtable_init(scope);

  analyzer_visit_decl_var(analyzer, for_table, ast_get_var(node));
  
  ast_set_range(node, analyzer_visit_expr(analyzer, for_table, ast_get_range(node)));

  typedesc_t* range_desc = typetable_lookup(analyzer->typetable, ast_get_range(node));
  assert(range_desc != NULL);

  range_desc = typedesc_remove_ref(range_desc);

  if (!typedesc_is_gen(range_desc))
    report_error_expected_generator_type(token_get_loc(ast_get_token(ast_get_range(node))));

  analyzer_visit_stmt(analyzer, for_table, ast_get_stmt(node));
}

void analyzer_visit_stmt_while(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  symtable_t* while_table = symtable_init(scope);

  ast_set_cond(node, analyzer_visit_expr(analyzer, while_table, ast_get_cond(node)));
  
  typedesc_t* cond_desc = typetable_lookup(analyzer->typetable, ast_get_cond(node));
  assert(cond_desc != NULL);

  cond_desc = typedesc_remove_ref(cond_desc);

  if (!typedesc_is_bool(cond_desc))
    report_error_expected_bool_type(token_get_loc(ast_get_token(ast_get_cond(node))));

  analyzer_visit_stmt(analyzer, while_table, ast_get_stmt(node));
}

void analyzer_visit_stmt_break(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  unused(analyzer);
  unused(scope);
  unused(node);
}

void analyzer_visit_stmt_continue(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  unused(analyzer);
  unused(scope);
  unused(node);
}

typedesc_t* analyzer_visit_stmt_return(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  typedesc_t* desc = typedesc_init(TYPEDESC_UNIT);

  if (ast_get_expr(node) != NULL)
  {
    ast_set_expr(node, analyzer_visit_expr(analyzer, scope, ast_get_expr(node)));
    
    desc = typetable_lookup(analyzer->typetable, ast_get_expr(node));
    assert(desc != NULL);
  }

  typedesc_t* expected_desc = stack_peek(analyzer->ret_types);

  if (!typedesc_is_same(desc, expected_desc))
    report_error_incompatible_return_type(token_get_loc(ast_get_token(node)));

  return desc;
}

typedesc_t* analyzer_visit_stmt_yield(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_expr(node, analyzer_visit_expr(analyzer, scope, ast_get_expr(node)));

  typedesc_t* expr_desc = typetable_lookup(analyzer->typetable, ast_get_expr(node));
  assert(expr_desc != NULL);

  typedesc_t* expected = stack_peek(analyzer->ret_types);
  
  if (!typedesc_is_same(expr_desc, expected))
    report_error_incompatible_return_type(token_get_loc(ast_get_token(node)));

  return expr_desc;
}

void analyzer_visit_stmt_block(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  symtable_t* block_table = symtable_init(scope);

  LIST_FOR_LOOP(it, ast_get_stmts(node))
    analyzer_visit_stmt(analyzer, block_table, (ast_node_t*)list_node_get(it));
}

typedesc_t* analyzer_visit_stmt_expr(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_expr(node, analyzer_visit_expr(analyzer, scope, ast_get_expr(node)));

  typedesc_t* expr_desc = typetable_lookup(analyzer->typetable, ast_get_expr(node));
  assert(expr_desc != NULL);

  return expr_desc;
}

void analyzer_visit_stmt(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  switch (ast_get_kind(node))
  {
  case AST_DECL_VAR:      analyzer_visit_decl_var     (analyzer, scope, node); break;
  case AST_STMT_IF:       analyzer_visit_stmt_if      (analyzer, scope, node); break;
  case AST_STMT_FOR:      analyzer_visit_stmt_for     (analyzer, scope, node); break;
  case AST_STMT_WHILE:    analyzer_visit_stmt_while   (analyzer, scope, node); break;
  case AST_STMT_BREAK:    analyzer_visit_stmt_break   (analyzer, scope, node); break;
  case AST_STMT_CONTINUE: analyzer_visit_stmt_continue(analyzer, scope, node); break;
  case AST_STMT_RETURN:   analyzer_visit_stmt_return  (analyzer, scope, node); break;
  case AST_STMT_YIELD:    analyzer_visit_stmt_yield   (analyzer, scope, node); break;
  case AST_STMT_BLOCK:    analyzer_visit_stmt_block   (analyzer, scope, node); break;
  case AST_STMT_EXPR:     analyzer_visit_stmt_expr    (analyzer, scope, node); break;
  default: unreachable();
  }
}

void analyzer_visit_decl_var(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  ast_set_type(node, analyzer_visit_type(analyzer, scope, ast_get_type(node)));

  typedesc_t* var_desc = typetable_lookup(analyzer->typetable, ast_get_type(node));
  assert(var_desc != NULL);

  typetable_insert(analyzer->typetable, node, var_desc);

  token_t* id_tok = ast_get_token(ast_get_id(node));
  symbol_t* var_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), node);
  symbol_t* lookup = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
  symbol_t* collision = symtable_insert(scope, var_sym);

  if (collision != NULL && ast_get_kind(collision->node) == AST_DECL_VAR)
    report_error_variable_redeclaration(token_get_loc(id_tok), token_get_loc(ast_get_token(collision->node)));

  if (lookup != NULL && ast_get_kind(lookup->node) == AST_DECL_VAR)
    report_warning_shadowed_variable(token_get_loc(id_tok));

  if (ast_get_expr(node) != NULL)
  {
    ast_set_expr(node, analyzer_visit_expr(analyzer, scope, ast_get_expr(node)));

    typedesc_t* init_desc = typetable_lookup(analyzer->typetable, ast_get_expr(node));
    assert(init_desc != NULL);

    if (!typedesc_is_same(init_desc, var_desc))
      report_error_type_mismatch(token_get_loc(ast_get_token(node)), var_desc, init_desc);
  }
}

void analyzer_visit_decl_fun(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  symtable_t* fun_table = symtable_init(scope);

  typedesc_t* fun_desc = typedesc_init(TYPEDESC_FUN);

  typedesc_set_params(fun_desc, NULL);

  if (ast_get_params(node) != NULL)
  {
    typedesc_set_params(fun_desc, list_init());

    LIST_FOR_LOOP(it, ast_get_params(node))
    {
      ast_node_t* param = list_node_get(it);
      analyzer_visit_decl_param(analyzer, fun_table, param);

      typedesc_t* param_desc = typetable_lookup(analyzer->typetable, param);
      assert(param_desc != NULL);

      list_push_back(typedesc_get_params(fun_desc), param_desc);
    }
  }

  ast_set_return_type(node, analyzer_visit_type(analyzer, scope, ast_get_return_type(node)));

  typedesc_t* return_desc = typetable_lookup(analyzer->typetable, ast_get_return_type(node));
  assert(return_desc != NULL);

  typedesc_set_return(fun_desc, return_desc);

  typetable_insert(analyzer->typetable, node, fun_desc);

  token_t* id_tok = ast_get_token(ast_get_id(node));
  symbol_t* fun_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), node);
  fun_sym->scope = fun_table;

  symbol_t* collision = symtable_insert(scope, fun_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(token_get_loc(ast_get_token(node)));

  stack_push(analyzer->ret_types, return_desc);
  analyzer_visit_stmt(analyzer, fun_table, ast_get_stmt(node));
  stack_pop(analyzer->ret_types);
}

void analyzer_visit_decl_gen(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  symtable_t* gen_table = symtable_init(scope);

  typedesc_t* gen_desc = typedesc_init(TYPEDESC_GEN);

  typedesc_set_params(gen_desc, NULL);

  if (ast_get_params(node) != NULL)
  {
    typedesc_set_params(gen_desc, list_init());

    LIST_FOR_LOOP(it, ast_get_params(node))
    {
      ast_node_t* param = (ast_node_t*)list_node_get(it);
      analyzer_visit_decl_param(analyzer, gen_table, param);

      typedesc_t* param_desc = typetable_lookup(analyzer->typetable, param);
      assert(param_desc != NULL);

      list_push_back(typedesc_get_params(gen_desc), param_desc);
    }
  }

  ast_set_yield_type(node, analyzer_visit_type(analyzer, scope, ast_get_yield_type(node)));

  typedesc_t* yield_desc = typetable_lookup(analyzer->typetable, ast_get_yield_type(node));
  assert(yield_desc != NULL);

  typedesc_set_yield(gen_desc, yield_desc);

  typetable_insert(analyzer->typetable, node, gen_desc);

  token_t* id_tok = ast_get_token(ast_get_id(node));
  symbol_t* gen_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), node);
  gen_sym->scope = gen_table;

  symbol_t* collision = symtable_insert(scope, gen_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(token_get_loc(ast_get_token(node)));
  
  stack_push(analyzer->ret_types, yield_desc);
  analyzer_visit_stmt(analyzer, gen_table, ast_get_stmt(node));
  stack_pop(analyzer->ret_types);
}

void analyzer_visit_decl_struct(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  token_t* id_tok = ast_get_token(ast_get_id(node));
  symbol_t* struct_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), node);
  symbol_t* lookup = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
  symbol_t* collision = symtable_insert(scope, struct_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(token_get_loc(ast_get_token(node)));

  if (lookup != NULL)
    report_warning_shadowed_symbol(token_get_loc(ast_get_token(node)));

  symtable_t* struct_table = symtable_init(scope);
  struct_sym->scope = struct_table;

  typedesc_t* struct_desc = typedesc_init(TYPEDESC_STRUCT);

  typedesc_set_node(struct_desc, node);
  typedesc_set_members(struct_desc, list_init());

  LIST_FOR_LOOP(it, ast_get_members(node))
  {
    ast_node_t* var = (ast_node_t*)list_node_get(it);
    analyzer_visit_decl_var(analyzer, struct_table, var);

    typedesc_t* var_desc = typetable_lookup(analyzer->typetable, var);
    assert(var_desc != NULL);

    list_push_back(typedesc_get_members(struct_desc), var_desc);
  }

  typetable_insert(analyzer->typetable, node, (typedesc_t*)struct_desc);
}

void analyzer_visit_decl_union(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  token_t* id_tok = ast_get_token(ast_get_id(node));
  symbol_t* union_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), node);
  symbol_t* lookup = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
  symbol_t* collision = symtable_insert(scope, union_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(token_get_loc(ast_get_token(node)));

  if (lookup != NULL)
    report_warning_shadowed_symbol(token_get_loc(ast_get_token(node)));

  symtable_t* union_table = symtable_init(scope);
  union_sym->scope = union_table;

  typedesc_t* union_desc = typedesc_init(TYPEDESC_UNION);

  typedesc_set_node(union_desc, node);
  typedesc_set_members(union_desc, list_init());

  LIST_FOR_LOOP(it, ast_get_members(node))
  {
    ast_node_t* var = (ast_node_t*)list_node_get(it);
    analyzer_visit_decl_var(analyzer, union_table, var);

    typedesc_t* var_desc = typetable_lookup(analyzer->typetable, var);
    assert(var_desc != NULL);

    list_push_back(typedesc_get_members(union_desc), var_desc);
  }

  typetable_insert(analyzer->typetable, node, (typedesc_t*)union_desc);
}

void analyzer_visit_decl_enum(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  token_t* id_tok = ast_get_token(ast_get_id(node));
  symbol_t* enum_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), node);
  symbol_t* lookup = symtable_lookup(scope, location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)));
  symbol_t* collision = symtable_insert(scope, enum_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(token_get_loc(ast_get_token(node)));

  if (lookup != NULL)
    report_warning_shadowed_symbol(token_get_loc(ast_get_token(node)));

  symtable_t* enum_table = symtable_init(scope);
  enum_sym->scope = enum_table;

  typedesc_t* enum_desc = typedesc_init(TYPEDESC_ENUM);

  typedesc_set_node(enum_desc, node);

  LIST_FOR_LOOP(it, ast_get_members(node))
    analyzer_visit_decl_enum_constant(analyzer, enum_table, enum_sym, (ast_node_t*)list_node_get(it));

  typetable_insert(analyzer->typetable, node, enum_desc);
}

void analyzer_visit_decl_mod(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  token_t* id_tok = ast_get_token(ast_get_id(node));
  symbol_t* mod_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), node);
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

  typedesc_t* mod_desc = typedesc_init(TYPEDESC_MOD);
  
  typedesc_set_node(mod_desc, node);
  typedesc_set_members(mod_desc, list_init());

  LIST_FOR_LOOP(it, ast_get_decls(node))
  {
    ast_node_t* decl = (ast_node_t*)list_node_get(it);
    analyzer_visit_decl(analyzer, mod_table, decl);

    typedesc_t* decl_desc = typetable_lookup(analyzer->typetable, decl);
    assert(decl_desc != NULL);

    list_push_back(typedesc_get_members(mod_desc), decl_desc);
  }

  typetable_insert(analyzer->typetable, node, (typedesc_t*)mod_desc);
}

void analyzer_visit_decl(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  switch (ast_get_kind(node))
  {
  case AST_DECL_VAR:     analyzer_visit_decl_var(analyzer, scope, node); break;
  case AST_DECL_FUN:     analyzer_visit_decl_fun(analyzer, scope, node); break;
  case AST_DECL_GEN:     analyzer_visit_decl_gen(analyzer, scope, node); break;
  case AST_DECL_STRUCT:  analyzer_visit_decl_struct(analyzer, scope, node); break;
  case AST_DECL_UNION:   analyzer_visit_decl_union(analyzer, scope, node); break;
  case AST_DECL_ENUM:    analyzer_visit_decl_enum(analyzer, scope, node); break;
  case AST_DECL_MOD:     analyzer_visit_decl_mod(analyzer, scope, node); break;
  default: unreachable();
  }
}

void analyzer_visit_decl_param(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  token_t* id_tok = ast_get_token(ast_get_id(node));
  symbol_t* param_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), node);
  symbol_t* collision = symtable_insert(scope, param_sym);

  if (collision != NULL && ast_get_kind(collision->node) == AST_DECL_PARAM)
    report_error_parameter_redefinition(token_get_loc(id_tok), token_get_loc(ast_get_token(collision->node)));

  ast_set_type(node, analyzer_visit_type(analyzer, scope, ast_get_type(node)));

  typedesc_t* param_desc = typetable_lookup(analyzer->typetable, ast_get_type(node));
  assert(param_desc != NULL);

  typetable_insert(analyzer->typetable, node, param_desc);

  if (ast_get_expr(node) != NULL)
    analyzer_visit_expr(analyzer, scope, ast_get_expr(node));
}

void analyzer_visit_decl_enum_constant(analyzer_t* analyzer, symtable_t* scope, symbol_t* enum_sym, ast_node_t* node)
{
  unused(analyzer);

  token_t* id_tok = ast_get_token(ast_get_id(node));
  symbol_t* enum_constant_sym = symbol_init(location_get_ptr(token_get_loc(id_tok)), location_get_len(token_get_loc(id_tok)), node);
  symbol_t* collision = symtable_insert(scope, enum_constant_sym);

  if (collision != NULL)
    report_error_enumerator_redeclaration(token_get_loc(id_tok), token_get_loc(ast_get_token(collision->node)));

  typedesc_t* enum_desc = typetable_lookup(analyzer->typetable, enum_sym->node);
  assert(enum_desc != NULL);

  typetable_insert(analyzer->typetable, node, enum_desc);
}

void analyzer_visit_prog(analyzer_t* analyzer, symtable_t* scope, ast_node_t* node)
{
  symtable_t* prog_table = symtable_init(scope);

  LIST_FOR_LOOP(it, ast_get_decls(node))
    analyzer_visit_decl(analyzer, prog_table, (ast_node_t*)list_node_get(it));
}

void analyzer_analyze(analyzer_t* analyzer, ast_node_t* root, symtable_t* symtable, typetable_t* typetable)
{
  assert(ast_get_kind(root) == AST_PROG);

  analyzer->symtable = symtable;
  analyzer->typetable = typetable;

  analyzer_visit_prog(analyzer, analyzer->symtable, root);
}
