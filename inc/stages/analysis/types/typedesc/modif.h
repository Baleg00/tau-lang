/**
 * \file
 * 
 * \brief Modifier type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_MODIF_H
#define TAU_TYPEDESC_MODIF_H

#include "stages/analysis/types/typedesc/base.h"

/**
 * \brief Header for all modifier type descriptors.
 */
#define TAU_TYPEDESC_MODIF_HEADER\
  TAU_TYPEDESC_HEADER;\
  tau_typedesc_t* base_type /** Underlying type. */

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for type modifiers.
 */
typedef struct tau_typedesc_modif_t
{
  TAU_TYPEDESC_MODIF_HEADER;
} tau_typedesc_modif_t;

TAU_EXTERN_C_END

#endif
