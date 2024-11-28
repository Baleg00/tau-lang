/**
 * \file
 * 
 * \brief Enum type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_DECL_ENUM_H
#define TAU_TYPEDESC_DECL_ENUM_H

#include "stages/analysis/types/typedesc/decl.h"
#include "utils/collections/vector.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for enum types.
 */
typedef struct typedesc_enum_t
{
  TYPEDESC_DECL_HEADER;
} typedesc_enum_t;

/**
 * \brief Initializes a new enum type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_enum_t* typedesc_enum_init(void);

/**
 * \brief Frees all memory allocated by an enum type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_enum_free(typedesc_enum_t* desc);

/**
 * \brief Checks if an enum type is implicitly direct convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source enum type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_enum_is_implicitly_direct_convertible(typedesc_enum_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Checks if an enum type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source enum type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_enum_is_implicitly_indirect_convertible(typedesc_enum_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Checks if an enum type is explicitly convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source enum type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool typedesc_enum_is_explicitly_convertible(typedesc_enum_t* src_desc, typedesc_t* dst_desc);

TAU_EXTERN_C_END

#endif
