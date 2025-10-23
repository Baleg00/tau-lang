/**
 * \file
 * 
 * \brief Abstract syntax tree statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_H
#define TAU_AST_STMT_H

#include "ast/node.h"

/**
 * \brief Header for all AST statement nodes.
 */
#define TAU_AST_STMT_HEADER\
  TAU_AST_NODE_HEADER

TAU_EXTERN_C_BEGIN

/**
 * \brief AST statement node.
 */
typedef struct tau_ast_stmt_t
{
  TAU_AST_STMT_HEADER;
} tau_ast_stmt_t;

TAU_EXTERN_C_END

#endif
