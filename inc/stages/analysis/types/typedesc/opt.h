/**
 * \file
 * 
 * \brief Optional type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_OPT_H
#define TAU_TYPEDESC_OPT_H

#include "stages/analysis/types/typedesc/modif.h"

/**
 * \brief Type descriptor for optional types.
 */
typedef struct typedesc_opt_t
{
  TYPEDESC_MODIF_HEADER;
} typedesc_opt_t;

/**
 * \brief Initializes a new optional type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_opt_t* typedesc_opt_init(void);

/**
 * \brief Frees all memory allocated by an optional type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_opt_free(typedesc_opt_t* desc);

/**
 * \brief Checks if an optional type is implicitly convertible to another type.
 * 
 * \param[in] from_desc Pointer to the optional type descriptor.
 * \param[in] to_desc Pointer to the type descriptor to check conversion into.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_opt_is_implicitly_convertible(typedesc_opt_t* from_desc, typedesc_t* to_desc);

#endif