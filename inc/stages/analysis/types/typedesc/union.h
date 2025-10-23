/**
 * \file
 * 
 * \brief Union type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_DECL_UNION_H
#define TAU_TYPEDESC_DECL_UNION_H

#include "stages/analysis/types/typedesc/decl.h"
#include "utils/collections/vector.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for union types.
 */
typedef struct tau_typedesc_union_t
{
  TAU_TYPEDESC_DECL_HEADER;
  tau_vector_t* field_types; // Vector of associated field types.
} tau_typedesc_union_t;

/**
 * \brief Initializes a new union type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
tau_typedesc_union_t* tau_typedesc_union_init(void);

/**
 * \brief Frees all memory allocated by an union type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void tau_typedesc_union_free(tau_typedesc_union_t* desc);

/**
 * \brief Checks if a union type is implicitly direct convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source union type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_union_is_implicitly_direct_convertible(tau_typedesc_union_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if a union type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source union type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_union_is_implicitly_indirect_convertible(tau_typedesc_union_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if a union type is explicitly convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source union type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_union_is_explicitly_convertible(tau_typedesc_union_t* src_desc, tau_typedesc_t* dst_desc);

TAU_EXTERN_C_END

#endif
