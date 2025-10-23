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
#define TAU_TYPEDESC_DECL_HEADER\
  TAU_TYPEDESC_HEADER;\
  tau_ast_node_t* node /** Declaration node. */

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for declaration types.
 */
typedef struct tau_typedesc_decl_t
{
  TAU_TYPEDESC_DECL_HEADER;
} tau_typedesc_decl_t;

TAU_EXTERN_C_END

#endif
