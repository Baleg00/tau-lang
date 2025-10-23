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
#define TAU_AST_DECL_HEADER\
  TAU_AST_NODE_HEADER;\
  tau_ast_node_t* id; /** The associated identifier. */\
  bool is_pub /** Is declaration public (in case it is a member). */

TAU_EXTERN_C_BEGIN

/**
 * \brief AST declaration node.
 */
typedef struct tau_ast_decl_t
{
  TAU_AST_DECL_HEADER;
} tau_ast_decl_t;

TAU_EXTERN_C_END

#endif
