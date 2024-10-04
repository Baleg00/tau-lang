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
typedef struct typedesc_fun_t
{
  TYPEDESC_HEADER;
  vector_t* param_types;
  typedesc_t* return_type;
  bool is_vararg;
  callconv_kind_t callconv;
} typedesc_fun_t;

/**
 * \brief Initializes a new function type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_fun_t* typedesc_fun_init(void);

/**
 * \brief Frees all memory allocated by an function type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_fun_free(typedesc_fun_t* desc);

/**
 * \brief Checks if a function type is implicitly convertible to another type.
 * 
 * \param[in] from_desc Pointer to the function type descriptor.
 * \param[in] to_desc Pointer to the type descriptor to check conversion into.
 * \param[in] through_ref Whether the conversion is done through any kind of reference.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_fun_is_implicitly_convertible(typedesc_fun_t* from_desc, typedesc_t* to_desc, bool through_ref);

/**
 * \brief Checks if a function type is explicitly convertible to another type.
 * 
 * \param[in] from_desc Pointer to the function type descriptor.
 * \param[in] to_desc Pointer to the type descriptor to check conversion into.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool typedesc_fun_is_explicitly_convertible(typedesc_fun_t* from_desc, typedesc_t* to_desc);

TAU_EXTERN_C_END

#endif
