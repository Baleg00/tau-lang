/**
 * \file
 * 
 * \brief Abstract syntax tree declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_H
#define TAU_AST_DECL_H

#include "ast/node.h"

/**
 * \brief Header for all AST declaration nodes.
 */
#define AST_DECL_HEADER\
  AST_NODE_HEADER;\
  ast_node_t* id; /** The associated identifier. */\
  bool is_pub /** Is declaration public (in case it is a member). */

/**
 * \brief AST declaration node.
 */
typedef struct ast_decl_t
{
  AST_DECL_HEADER;
} ast_decl_t;

#endif