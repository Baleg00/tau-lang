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
typedef struct typedesc_poison_t
{
  TYPEDESC_HEADER;
} typedesc_poison_t;

/**
 * \brief Initializes a new poison type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_poison_t* typedesc_poison_init(void);

/**
 * \brief Frees all memory allocated by an poison type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_poison_free(typedesc_poison_t* desc);

/**
 * \brief Checks if an poison type is implicitly direct convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source poison type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_poison_is_implicitly_direct_convertible(typedesc_poison_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Checks if an poison type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source poison type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_poison_is_implicitly_indirect_convertible(typedesc_poison_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Checks if an poison type is explicitly convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source poison type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool typedesc_poison_is_explicitly_convertible(typedesc_poison_t* src_desc, typedesc_t* dst_desc);

TAU_EXTERN_C_END

#endif
