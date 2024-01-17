/**
 * \file mod.h
 * 
 * \brief Abstract syntax tree module declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_MOD_H
#define TAU_AST_DECL_MOD_H

#include "stages/parser/ast/decl/decl.h"

/**
 * \brief Forward declaration of symbol table to avoid circular includes.
 * 
 * \see symtable.h
 */
typedef struct symtable_t symtable_t;

/**
 * \brief AST module declaration node.
 */
typedef struct ast_decl_mod_t
{
  AST_DECL_HEADER;
  symtable_t* scope; // The associated scope of members.
  list_t* members; // List of associated member declarations.
  bool is_pub; // Is enum public (in case it is a member).
} ast_decl_mod_t;

#endif