/**
 * \file
 * 
 * \brief Poison type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_POISON_H
#define TAU_TYPEDESC_POISON_H

#include "stages/analysis/types/typedesc/base.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for the poison type.
 */
typedef struct tau_typedesc_poison_t
{
  TAU_TYPEDESC_HEADER;
} tau_typedesc_poison_t;

/**
 * \brief Initializes a new poison type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
tau_typedesc_poison_t* tau_typedesc_poison_init(void);

/**
 * \brief Frees all memory allocated by an poison type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void tau_typedesc_poison_free(tau_typedesc_poison_t* desc);

/**
 * \brief Checks if an poison type is implicitly direct convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source poison type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_poison_is_implicitly_direct_convertible(tau_typedesc_poison_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if an poison type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source poison type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_poison_is_implicitly_indirect_convertible(tau_typedesc_poison_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if an poison type is explicitly convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source poison type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_poison_is_explicitly_convertible(tau_typedesc_poison_t* src_desc, tau_typedesc_t* dst_desc);

TAU_EXTERN_C_END

#endif
