/**
 * \file
 * 
 * \brief Pointer type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_PTR_H
#define TAU_TYPEDESC_PTR_H

#include "stages/analysis/types/typedesc/modif.h"

/**
 * \brief Type descriptor for pointer types.
 */
typedef struct typedesc_ptr_t
{
  TYPEDESC_MODIF_HEADER;
} typedesc_ptr_t;

/**
 * \brief Initializes a new pointer type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_ptr_t* typedesc_ptr_init(void);

/**
 * \brief Frees all memory allocated by an pointer type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_ptr_free(typedesc_ptr_t* desc);

/**
 * \brief Checks if a pointer type is implicitly convertible to another type.
 * 
 * \param[in] from_desc Pointer to the pointer type descriptor.
 * \param[in] to_desc Pointer to the type descriptor to check conversion into.
 * \param[in] through_ref Whether the conversion is done through any kind of reference.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_ptr_is_implicitly_convertible(typedesc_ptr_t* from_desc, typedesc_t* to_desc, bool through_ref);

/**
 * \brief Checks if a pointer type is explicitly convertible to another type.
 * 
 * \param[in] from_desc Pointer to the pointer type descriptor.
 * \param[in] to_desc Pointer to the type descriptor to check conversion into.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool typedesc_ptr_is_explicitly_convertible(typedesc_ptr_t* from_desc, typedesc_t* to_desc);

#endif
