#include "analzyer.h"

#include <stdlib.h>

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
  return analyzer;
}

void analyzer_free(analyzer_t* analyzer)
{
  symtable_free(analyzer->root);
  free(analyzer);
}

void analyzer_visit_param(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* param_sym = symbol_init(node->param.id->tok->id.value, node);
  symbol_t* collision = symtable_insert(table, param_sym);

  if (collision != NULL && collision->node->kind == AST_PARAM)
    report_error_parameter_redefinition(node->param.id->tok->loc);

  node->param.type = analyzer_visit_type(analyzer, table, node->param.type);
}

void analyzer_visit_loop_var(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* var_sym = symbol_init(node->loop_var.id->tok->id.value, node);
  symbol_t* lookup = symtable_lookup(table, node->loop_var.id->tok->id.value);
  symbol_t* collision = symtable_insert(table, var_sym);

  assert(collision == NULL);

  if (lookup != NULL)
    report_warning_shadowed_variable(node->loop_var.id->tok->loc);
}

void analyzer_visit_enumerator(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* enumerator_sym = symbol_init(node->enumerator.id->tok->id.value, node);
  symbol_t* collision = symtable_insert(table, enumerator_sym);

  if (collision != NULL)
    report_error_enumerator_redeclaration(node->enumerator.id->tok->loc);
}

void analyzer_visit_expr_op(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  if (op_is_unary(node->expr_op.kind))
    analyzer_visit_expr(analyzer, table, (ast_node_t*)node->expr_op.op_unary.arg);
  else if (op_is_binary(node->expr_op.kind))
  {
    analyzer_visit_expr(analyzer, table, (ast_node_t*)node->expr_op.op_binary.lhs);
    analyzer_visit_expr(analyzer, table, (ast_node_t*)node->expr_op.op_binary.rhs);
  }
  else if (node->expr_op.kind == OP_CALL)
  {
    analyzer_visit_expr(analyzer, table, (ast_node_t*)node->expr_op.op_call.callee);

    for (list_elem_t* elem = list_front_elem(node->expr_op.op_call.args); elem != NULL; elem = list_elem_next(elem))
      analyzer_visit_expr(analyzer, table, (ast_node_t*)list_elem_get(elem));
  }
  else
    unreachable();
}

void analyzer_visit_expr(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_ID:
    symbol_t* id_sym = symtable_lookup(table, node->tok->id.value);
    
    if (id_sym == NULL)
      report_error_undefined_symbol(node->tok->loc);

    if (id_sym->node->kind != AST_DECL_VAR &&
        id_sym->node->kind != AST_PARAM &&
        id_sym->node->kind != AST_LOOP_VAR &&
        id_sym->node->kind != AST_ENUMERATOR &&
        id_sym->node->kind != AST_DECL_FUN &&
        id_sym->node->kind != AST_DECL_GEN)
      report_error_symbol_is_not_an_expression(node->tok->loc);
    break;
  case AST_EXPR_LIT_INT:
  case AST_EXPR_LIT_FLT:
  case AST_EXPR_LIT_STR:
  case AST_EXPR_LIT_CHAR:
  case AST_EXPR_LIT_BOOL:
  case AST_EXPR_LIT_NULL:
    break;
  case AST_EXPR_OP:
    analyzer_visit_expr_op(analyzer, table, node);
    break;
  default: unreachable();
  }
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
        id_sym->node->kind != AST_DECL_ENUM)
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
    for (list_elem_t* elem = list_front_elem(node->type_fun.params); elem != NULL; elem = list_elem_next(elem))
      list_elem_set(elem, analyzer_visit_type(analyzer, table, (ast_node_t*)list_elem_get(elem)));
    break;
  case AST_TYPE_GEN:
    node->type_gen.ret_type = analyzer_visit_type(analyzer, table, node->type_gen.ret_type);
    for (list_elem_t* elem = list_front_elem(node->type_gen.params); elem != NULL; elem = list_elem_next(elem))
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
  default: unreachable();
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

void analyzer_visit_stmt_return(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  if (node->stmt_return.expr != NULL)
    analyzer_visit_expr(analyzer, table, node->stmt_return.expr);
}

void analyzer_visit_stmt_yield(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  analyzer_visit_expr(analyzer, table, node->stmt_yield.expr);
}

void analyzer_visit_stmt_block(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symtable_t* block_table = symtable_init(table);

  for (list_elem_t* elem = list_front_elem(node->stmt_block.stmts); elem != NULL; elem = list_elem_next(elem))
    analyzer_visit_stmt(analyzer, block_table, (ast_node_t*)list_elem_get(elem));
}

void analyzer_visit_stmt_expr(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  analyzer_visit_expr(analyzer, table, node->stmt_expr.expr);
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
  symbol_t* var_sym = symbol_init(node->decl_var.id->tok->id.value, node);
  symbol_t* lookup = symtable_lookup(table, node->decl_var.id->tok->id.value);
  symbol_t* collision = symtable_insert(table, var_sym);

  if (collision != NULL && collision->node->kind == AST_DECL_VAR)
    report_error_variable_redeclaration(node->decl_var.id->tok->loc);

  if (lookup != NULL && lookup->node->kind == AST_DECL_VAR)
    report_warning_shadowed_variable(node->decl_var.id->tok->loc);

  analyzer_visit_expr(analyzer, table, node->decl_var.init);
  node->decl_var.type = analyzer_visit_type(analyzer, table, node->decl_var.type);
}

void analyzer_visit_decl_fun(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* fun_sym = symbol_init(node->decl_fun.id->tok->id.value, node);
  symbol_t* collision = symtable_insert(table, fun_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  symtable_t* fun_table = symtable_init(table);
  fun_sym->scope = fun_table;

  for (list_elem_t* elem = list_front_elem(node->decl_fun.params); elem != NULL; elem = list_elem_next(elem))
    analyzer_visit_param(analyzer, fun_table, (ast_node_t*)list_elem_get(elem));

  node->decl_fun.ret_type = analyzer_visit_type(analyzer, table, node->decl_fun.ret_type);
  analyzer_visit_stmt(analyzer, fun_table, node->decl_fun.stmt);
}

void analyzer_visit_decl_gen(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* gen_sym = symbol_init(node->decl_gen.id->tok->id.value, node);
  symbol_t* collision = symtable_insert(table, gen_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  symtable_t* gen_table = symtable_init(table);
  gen_sym->scope = gen_table;

  for (list_elem_t* elem = list_front_elem(node->decl_gen.params); elem != NULL; elem = list_elem_next(elem))
    analyzer_visit_param(analyzer, gen_table, (ast_node_t*)list_elem_get(elem));

  node->decl_gen.ret_type = analyzer_visit_type(analyzer, table, node->decl_gen.ret_type);
  analyzer_visit_stmt(analyzer, gen_table, node->decl_gen.stmt);
}

void analyzer_visit_decl_struct(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* struct_sym = symbol_init(node->decl_struct.id->tok->id.value, node);
  symbol_t* lookup = symtable_lookup(table, node->decl_struct.id->tok->id.value);
  symbol_t* collision = symtable_insert(table, struct_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* struct_table = symtable_init(table);
  struct_sym->scope = struct_table;

  for (list_elem_t* elem = list_front_elem(node->decl_struct.members); elem != NULL; elem = list_elem_next(elem))
    analyzer_visit_decl_member(analyzer, struct_table, (ast_node_t*)list_elem_get(elem));
}

void analyzer_visit_decl_union(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* union_sym = symbol_init(node->decl_union.id->tok->id.value, node);
  symbol_t* lookup = symtable_lookup(table, node->decl_union.id->tok->id.value);
  symbol_t* collision = symtable_insert(table, union_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* union_table = symtable_init(table);
  union_sym->scope = union_table;

  for (list_elem_t* elem = list_front_elem(node->decl_union.members); elem != NULL; elem = list_elem_next(elem))
    analyzer_visit_decl_member(analyzer, union_table, (ast_node_t*)list_elem_get(elem));
}

void analyzer_visit_decl_enum(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* enum_sym = symbol_init(node->decl_enum.id->tok->id.value, node);
  symbol_t* lookup = symtable_lookup(table, node->decl_enum.id->tok->id.value);
  symbol_t* collision = symtable_insert(table, enum_sym);

  if (collision != NULL)
    report_error_symbol_redeclaration(node->tok->loc);

  if (lookup != NULL)
    report_warning_shadowed_symbol(node->tok->loc);

  symtable_t* enum_table = symtable_init(table);
  enum_sym->scope = enum_table;

  for (list_elem_t* elem = list_front_elem(node->decl_enum.members); elem != NULL; elem = list_elem_next(elem))
    analyzer_visit_enumerator(analyzer, enum_table, (ast_node_t*)list_elem_get(elem));
}

void analyzer_visit_decl_mod(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  symbol_t* mod_sym = symbol_init(node->decl_enum.id->tok->id.value, node);
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

  for (list_elem_t* elem = list_front_elem(node->decl_mod.members); elem != NULL; elem = list_elem_next(elem))
    analyzer_visit_decl_member(analyzer, mod_table, (ast_node_t*)list_elem_get(elem));
}

void analyzer_visit_decl_member(analyzer_t* analyzer, symtable_t* table, ast_node_t* node)
{
  analyzer_visit_decl(analyzer, table, node->decl_member.decl);
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

  for (list_elem_t* elem = list_front_elem(node->prog.decls); elem != NULL; elem = list_elem_next(elem))
    analyzer_visit_decl(analyzer, prog_table, (ast_node_t*)list_elem_get(elem));
}

symtable_t* analyzer_analyze(analyzer_t* analyzer, ast_node_t* root)
{
  analyzer_visit_prog(analyzer, analyzer->root, root);
  return analyzer->root;
}
