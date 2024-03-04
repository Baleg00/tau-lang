/**
 * \file
 * 
 * \brief Constant type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_CONST_H
#define TAU_TYPEDESC_CONST_H

#include "stages/analysis/types/typedesc/modif.h"

/**
 * \brief Type descriptor for constant types.
 */
typedef struct typedesc_const_t
{
  TYPEDESC_MODIF_HEADER;
} typedesc_const_t;

/**
 * \brief Initializes a new constant type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_const_t* typedesc_const_init(void);

/**
 * \brief Frees all memory allocated by an constant type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_const_free(typedesc_const_t* desc);

/**
 * \brief Checks if a constant type is implicitly convertible to another type.
 * 
 * \param[in] desc Pointer to the constant type descriptor.
 * \param[in] target_desc Pointer to the type descriptor to check conversion into.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_const_is_implicitly_convertible(typedesc_const_t* desc, typedesc_t* target_desc);

#endif