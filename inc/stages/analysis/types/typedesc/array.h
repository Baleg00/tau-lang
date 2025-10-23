/**
 * \file
 * 
 * \brief Array type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_ARRAY_H
#define TAU_TYPEDESC_ARRAY_H

#include "stages/analysis/types/typedesc/modif.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for array types.
 */
typedef struct tau_typedesc_array_t
{
  TAU_TYPEDESC_MODIF_HEADER;
  size_t length; // Number of elements in the array.
} tau_typedesc_array_t;

/**
 * \brief Initializes a new array type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
tau_typedesc_array_t* tau_typedesc_array_init(void);

/**
 * \brief Frees all memory allocated by an array type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void tau_typedesc_array_free(tau_typedesc_array_t* desc);

/**
 * \brief Checks if an array type is implicitly direct convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source array type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_array_is_implicitly_direct_convertible(tau_typedesc_array_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if an array type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source array type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_array_is_implicitly_indirect_convertible(tau_typedesc_array_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if an array type is explicitly convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source array type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_array_is_explicitly_convertible(tau_typedesc_array_t* src_desc, tau_typedesc_t* dst_desc);

TAU_EXTERN_C_END

#endif
