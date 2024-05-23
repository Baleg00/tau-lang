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

/**
 * \brief Type descriptor for struct types.
 */
typedef struct typedesc_struct_t
{
  TYPEDESC_DECL_HEADER;
  vector_t* field_types; // Vector of associated field types.
} typedesc_struct_t;

/**
 * \brief Initializes a new struct type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_struct_t* typedesc_struct_init(void);

/**
 * \brief Frees all memory allocated by an struct type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_struct_free(typedesc_struct_t* desc);

/**
 * \brief Checks if a struct type is implicitly convertible to another type.
 * 
 * \param[in] from_desc Pointer to the struct type descriptor.
 * \param[in] to_desc Pointer to the type descriptor to check conversion into.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_struct_is_implicitly_convertible(typedesc_struct_t* from_desc, typedesc_t* to_desc);

#endif