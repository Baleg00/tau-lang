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

/**
 * \brief Type variable.
 */
typedef struct typedesc_var_t
{
  TYPEDESC_HEADER;
  uint64_t id; // The associated type variable identifier.
  typedesc_t* subst; // The substituted type descriptor or `NULL`.
} typedesc_var_t;

/**
 * \brief Initializes a new type variable.
 * 
 * \returns Pointer to the newly initialized type variable.
*/
typedesc_var_t* typedesc_var_init(void);

/**
 * \brief Frees all memory allocated by an type variable.
 * 
 * \param[in] desc Pointer to the type variable to be freed.
 */
void typedesc_var_free(typedesc_var_t* desc);

#endif
