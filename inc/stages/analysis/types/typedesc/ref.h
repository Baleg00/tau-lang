/**
 * \file
 * 
 * \brief Reference type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_REF_H
#define TAU_TYPEDESC_REF_H

#include "stages/analysis/types/typedesc/modif.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for reference types.
 */
typedef struct typedesc_ref_t
{
  TYPEDESC_MODIF_HEADER;
} typedesc_ref_t;

/**
 * \brief Initializes a new reference type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_ref_t* typedesc_ref_init(void);

/**
 * \brief Frees all memory allocated by an reference type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_ref_free(typedesc_ref_t* desc);

/**
 * \brief Checks if a reference type is implicitly direct convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source reference type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor to check conversion into.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_ref_is_implicitly_direct_convertible(typedesc_ref_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Checks if a reference type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source reference type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor to check conversion into.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_ref_is_implicitly_indirect_convertible(typedesc_ref_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Checks if a reference type is explicitly convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source reference type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool typedesc_ref_is_explicitly_convertible(typedesc_ref_t* src_desc, typedesc_t* dst_desc);

TAU_EXTERN_C_END

#endif
