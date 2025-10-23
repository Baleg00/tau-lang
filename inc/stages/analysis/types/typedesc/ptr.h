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

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for pointer types.
 */
typedef struct tau_typedesc_ptr_t
{
  TAU_TYPEDESC_MODIF_HEADER;
} tau_typedesc_ptr_t;

/**
 * \brief Initializes a new pointer type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
tau_typedesc_ptr_t* tau_typedesc_ptr_init(void);

/**
 * \brief Frees all memory allocated by an pointer type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void tau_typedesc_ptr_free(tau_typedesc_ptr_t* desc);

/**
 * \brief Checks if a pointer type is implicitly direct convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source pointer type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_ptr_is_implicitly_direct_convertible(tau_typedesc_ptr_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if a pointer type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source pointer type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_ptr_is_implicitly_indirect_convertible(tau_typedesc_ptr_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if a pointer type is explicitly convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source pointer type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_ptr_is_explicitly_convertible(tau_typedesc_ptr_t* src_desc, tau_typedesc_t* dst_desc);

TAU_EXTERN_C_END

#endif
