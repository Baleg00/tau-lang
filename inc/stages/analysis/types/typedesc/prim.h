/**
 * \file
 * 
 * \brief Primitive type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_PRIM_H
#define TAU_TYPEDESC_PRIM_H

#include "stages/analysis/types/typedesc/base.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Type descriptor for primitive types.
 */
typedef struct typedesc_prim_t
{
  TYPEDESC_HEADER;
} typedesc_prim_t;

/**
 * \brief Initializes a new primitive `i8` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_i8_init(void);

/**
 * \brief Initializes a new primitive `i16` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_i16_init(void);

/**
 * \brief Initializes a new primitive `i32` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_i32_init(void);

/**
 * \brief Initializes a new primitive `i64` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_i64_init(void);

/**
 * \brief Initializes a new primitive `isize` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_isize_init(void);

/**
 * \brief Initializes a new primitive `u8` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_u8_init(void);

/**
 * \brief Initializes a new primitive `u16` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_u16_init(void);

/**
 * \brief Initializes a new primitive `u32` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_u32_init(void);

/**
 * \brief Initializes a new primitive `u64` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_u64_init(void);

/**
 * \brief Initializes a new primitive `usize` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_usize_init(void);

/**
 * \brief Initializes a new primitive `f32` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_f32_init(void);

/**
 * \brief Initializes a new primitive `f64` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_f64_init(void);

/**
 * \brief Initializes a new primitive `c64` type descriptor.
 *
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_c64_init(void);

/**
 * \brief Initializes a new primitive `c128` type descriptor.
 *
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_c128_init(void);

/**
 * \brief Initializes a new primitive `char` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_char_init(void);

/**
 * \brief Initializes a new primitive `bool` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_bool_init(void);

/**
 * \brief Initializes a new primitive `unit` type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
 */
typedesc_prim_t* typedesc_prim_unit_init(void);

/**
 * \brief Frees all memory allocated by an primitive type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_prim_free(typedesc_prim_t* desc);

/**
 * \brief Checks if a primitive type is implicitly direct convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source primitive type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_prim_is_implicitly_direct_convertible(typedesc_prim_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Checks if a primitive type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source primitive type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_prim_is_implicitly_indirect_convertible(typedesc_prim_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Checks if a primitive type is explicitly convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source primitive type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool typedesc_prim_is_explicitly_convertible(typedesc_prim_t* src_desc, typedesc_t* dst_desc);

TAU_EXTERN_C_END

#endif
