/**
 * \file
 * 
 * \brief Struct type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_DECL_STRUCT_H
#define TAU_TYPEDESC_DECL_STRUCT_H

#include "stages/analysis/types/typedesc/decl.h"
#include "utils/collections/vector.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for struct types.
 */
typedef struct tau_typedesc_struct_t
{
  TAU_TYPEDESC_DECL_HEADER;
  tau_vector_t* field_types; // Vector of associated field types.
} tau_typedesc_struct_t;

/**
 * \brief Initializes a new struct type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
tau_typedesc_struct_t* tau_typedesc_struct_init(void);

/**
 * \brief Frees all memory allocated by an struct type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void tau_typedesc_struct_free(tau_typedesc_struct_t* desc);

/**
 * \brief Checks if a struct type is implicitly direct convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source struct type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_struct_is_implicitly_direct_convertible(tau_typedesc_struct_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if a struct type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source struct type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_struct_is_implicitly_indirect_convertible(tau_typedesc_struct_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if a struct type is explicitly convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source struct type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_struct_is_explicitly_convertible(tau_typedesc_struct_t* src_desc, tau_typedesc_t* dst_desc);

TAU_EXTERN_C_END

#endif
