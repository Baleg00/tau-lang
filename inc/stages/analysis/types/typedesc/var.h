/**
 * \file
 * 
 * \brief Type variable interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_VAR_H
#define TAU_TYPEDESC_VAR_H

#include "stages/analysis/types/typedesc/base.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Type variable.
 */
typedef struct tau_typedesc_var_t
{
  TAU_TYPEDESC_HEADER;
  uint64_t id; // The associated type variable identifier.
  tau_typedesc_t* subst; // The substituted type descriptor or `NULL`.
} tau_typedesc_var_t;

/**
 * \brief Initializes a new type variable.
 * 
 * \returns Pointer to the newly initialized type variable.
*/
tau_typedesc_var_t* tau_typedesc_var_init(void);

/**
 * \brief Frees all memory allocated by an type variable.
 * 
 * \param[in] desc Pointer to the type variable to be freed.
 */
void tau_typedesc_var_free(tau_typedesc_var_t* desc);

TAU_EXTERN_C_END

#endif
