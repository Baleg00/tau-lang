/**
 * \file
 *
 * \brief Vector type descriptor interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_VEC_H
#define TAU_TYPEDESC_VEC_H

#include "stages/analysis/types/typedesc/base.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for vector types.
 */
typedef struct typedesc_vec_t
{
  TYPEDESC_HEADER;
  size_t size;           ///< The size of the vector.
  typedesc_t* base_type; ///< The base type of the vector.
} typedesc_vec_t;

/**
 * \brief Initializes a new vector type descriptor.
 *
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_vec_t* typedesc_vec_init(void);

/**
 * \brief Frees all memory allocated by a vector type descriptor.
 *
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_vec_free(typedesc_vec_t* desc);

/**
 * \brief Checks if a vector type is implicitly convertible to another type.
 *
 * \param[in] from_desc Pointer to the vector type descriptor.
 * \param[in] to_desc Pointer to the type descriptor to check conversion into.
 * \param[in] through_ref Whether the conversion is done through any kind of reference.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_vec_is_implicitly_convertible(typedesc_vec_t* from_desc, typedesc_t* to_desc, bool through_ref);

/**
 * \brief Checks if a vector type is explicitly convertible to another type.
 *
 * \param[in] from_desc Pointer to the vector type descriptor.
 * \param[in] to_desc Pointer to the type descriptor to check conversion into.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool typedesc_vec_is_explicitly_convertible(typedesc_vec_t* from_desc, typedesc_t* to_desc);

TAU_EXTERN_C_END

#endif
