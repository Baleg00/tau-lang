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

/**
 * \brief Type descriptor for union types.
 */
typedef struct typedesc_union_t
{
  TYPEDESC_DECL_HEADER;
  vector_t* field_types; // Vector of associated field types.
} typedesc_union_t;

/**
 * \brief Initializes a new union type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_union_t* typedesc_union_init(void);

/**
 * \brief Frees all memory allocated by an union type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_union_free(typedesc_union_t* desc);

#endif