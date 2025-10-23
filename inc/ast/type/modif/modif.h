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
#define TAU_AST_TYPE_MODIFIER_HEADER\
  TAU_AST_TYPE_HEADER;\
  tau_ast_node_t* base_type

TAU_EXTERN_C_BEGIN

/**
 * \brief AST type modifier node.
 */
typedef struct tau_ast_type_modif_t
{
  TAU_AST_TYPE_MODIFIER_HEADER;
} tau_ast_type_modif_t;

TAU_EXTERN_C_END

#endif
