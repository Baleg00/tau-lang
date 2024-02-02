/**
 * \file
 * 
 * \brief Abstract syntax tree type modifier node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_MODIFIER_H
#define TAU_AST_TYPE_MODIFIER_H

#include "ast/type/type.h"

/**
 * \brief Header for all AST type modifier nodes.
 */
#define AST_TYPE_MODIFIER_HEADER\
  AST_TYPE_HEADER;\
  ast_node_t* base_type

/**
 * \brief AST type modifier node.
 */
typedef struct ast_type_modif_t
{
  AST_TYPE_MODIFIER_HEADER;
} ast_type_modif_t;

#endif