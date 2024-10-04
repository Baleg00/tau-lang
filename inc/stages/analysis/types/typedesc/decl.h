/**
 * \file
 * 
 * \brief Declaration type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_DECL_H
#define TAU_TYPEDESC_DECL_H

#include "stages/analysis/types/typedesc/base.h"

/**
 * \brief Header for all declaration type descriptors.
 */
#define TYPEDESC_DECL_HEADER\
  TYPEDESC_HEADER;\
  ast_node_t* node /** Declaration node. */

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for declaration types.
 */
typedef struct typedesc_decl_t
{
  TYPEDESC_DECL_HEADER;
} typedesc_decl_t;

TAU_EXTERN_C_END

#endif
