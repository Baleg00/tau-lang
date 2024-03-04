/**
 * \file
 * 
 * \brief Mutable type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_MUT_H
#define TAU_TYPEDESC_MUT_H

#include "stages/analysis/types/typedesc/modif.h"

/**
 * \brief Type descriptor for mutable types.
 */
typedef struct typedesc_mut_t
{
  TYPEDESC_MODIF_HEADER;
} typedesc_mut_t;

/**
 * \brief Initializes a new mutable type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_mut_t* typedesc_mut_init(void);

/**
 * \brief Frees all memory allocated by an mutable type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_mut_free(typedesc_mut_t* desc);

/**
 * \brief Checks if a mutable type is implicitly convertible to another type.
 * 
 * \param[in] desc Pointer to the mutable type descriptor.
 * \param[in] target_desc Pointer to the type descriptor to check conversion into.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_mut_is_implicitly_convertible(typedesc_mut_t* desc, typedesc_t* target_desc);

#endif