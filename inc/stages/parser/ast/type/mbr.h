/**
 * \file mbr.h
 * 
 * \brief Abstract syntax tree member type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_MBR_H
#define TAU_AST_TYPE_MBR_H

#include "stages/parser/ast/type/type.h"

/**
 * \brief AST member type node.
 */
typedef struct ast_type_mbr_t
{
  AST_TYPE_HEADER;
  ast_node_t* parent;
  ast_node_t* member;
} ast_type_mbr_t;

#endif