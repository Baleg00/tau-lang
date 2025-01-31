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
  bool is_row;           ///< `true` if the vector is a row vector, `false` otherwise.
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
 * \brief Checks if a vector type is implicitly direct convertible to another type.
 *
 * \param[in] src_desc Pointer to the source vector type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_vec_is_implicitly_direct_convertible(typedesc_vec_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Checks if a vector type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source vector type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_vec_is_implicitly_indirect_convertible(typedesc_vec_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Checks if a vector type is explicitly convertible to another type.
 *
 * \param[in] src_desc Pointer to the vector type descriptor.
 * \param[in] dst_desc Pointer to the type descriptor to check conversion into.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool typedesc_vec_is_explicitly_convertible(typedesc_vec_t* src_desc, typedesc_t* dst_desc);

TAU_EXTERN_C_END

#endif
