/**
 * \file fun.h
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

#endif