/**
 * \file
 *
 * \brief Matrix type descriptor interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_MAT_H
#define TAU_TYPEDESC_MAT_H

#include "stages/analysis/types/typedesc/base.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for matrix types.
 */
typedef struct tau_typedesc_mat_t
{
  TAU_TYPEDESC_HEADER;
  size_t rows;           ///< The number of rows in the matrix.
  size_t cols;           ///< The number of columns in the matrix.
  tau_typedesc_t* base_type; ///< The base type of the matrix.
} tau_typedesc_mat_t;

/**
 * \brief Initializes a new matrix type descriptor.
 *
 * \returns Pointer to the newly initialized type descriptor.
*/
tau_typedesc_mat_t* tau_typedesc_mat_init(void);

/**
 * \brief Frees all memory allocated by a matrix type descriptor.
 *
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void tau_typedesc_mat_free(tau_typedesc_mat_t* desc);

/**
 * \brief Checks if a matrix type is implicitly direct convertible to another type.
 *
 * \param[in] src_desc Pointer to the source matrix type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_mat_is_implicitly_direct_convertible(tau_typedesc_mat_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if a matrix type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source matrix type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_mat_is_implicitly_indirect_convertible(tau_typedesc_mat_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if a matrix type is explicitly convertible to another type.
 *
 * \param[in] src_desc Pointer to the matrix type descriptor.
 * \param[in] dst_desc Pointer to the type descriptor to check conversion into.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_mat_is_explicitly_convertible(tau_typedesc_mat_t* src_desc, tau_typedesc_t* dst_desc);

TAU_EXTERN_C_END

#endif
