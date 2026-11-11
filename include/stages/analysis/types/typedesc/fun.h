/**
 * \file
 * 
 * \brief Function type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_FUN_H
#define TAU_TYPEDESC_FUN_H

#include "ast/callconv.h"
#include "stages/analysis/types/typedesc/base.h"
#include "utils/collections/vector.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for function types.
 */
typedef struct tau_typedesc_fun_t
{
  TAU_TYPEDESC_HEADER;
  tau_vector_t* param_types;
  tau_typedesc_t* return_type;
  bool is_vararg;
  tau_callconv_kind_t callconv;
} tau_typedesc_fun_t;

/**
 * \brief Initializes a new function type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
tau_typedesc_fun_t* tau_typedesc_fun_init(void);

/**
 * \brief Frees all memory allocated by an function type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void tau_typedesc_fun_free(tau_typedesc_fun_t* desc);

/**
 * \brief Checks if a function type is implicitly direct convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source function type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_fun_is_implicitly_direct_convertible(tau_typedesc_fun_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if a function type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source function type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_fun_is_implicitly_indirect_convertible(tau_typedesc_fun_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if a function type is explicitly convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source function type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_fun_is_explicitly_convertible(tau_typedesc_fun_t* src_desc, tau_typedesc_t* dst_desc);

TAU_EXTERN_C_END

#endif
