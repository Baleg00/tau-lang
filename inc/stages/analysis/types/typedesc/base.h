/**
 * \file
 * 
 * \brief Base type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_BASE_H
#define TAU_TYPEDESC_BASE_H

#include <stdbool.h>
#include <stdint.h>

#include "llvm.h"

/**
 * \brief Header for all type descriptors.
 */
#define TYPEDESC_HEADER\
  typedesc_kind_t kind; /** Type kind. */\
  LLVMTypeRef llvm_type /** LLVM type. */\

/**
 * \see ast/node.h 
 */
typedef struct ast_node_t ast_node_t;

/**
 * \brief Enumeration of type descriptor kinds.
 */
typedef enum typedesc_kind_e
{
  TYPEDESC_MUT, // Mutable type
  TYPEDESC_CONST, // Compile-time type
  TYPEDESC_PTR, // Pointer type
  TYPEDESC_ARRAY, // Array type
  TYPEDESC_REF, // Reference type
  TYPEDESC_OPT, // Optional type
  TYPEDESC_I8, // Built-in type i8
  TYPEDESC_I16, // Built-in type i16
  TYPEDESC_I32, // Built-in type i32
  TYPEDESC_I64, // Built-in type i64
  TYPEDESC_ISIZE, // Built-in type isize
  TYPEDESC_U8, // Built-in type u8
  TYPEDESC_U16, // Built-in type u16
  TYPEDESC_U32, // Built-in type u32
  TYPEDESC_U64, // Built-in type u64
  TYPEDESC_USIZE, // Built-in type usize
  TYPEDESC_F32, // Built-in type f32
  TYPEDESC_F64, // Built-in type f64
  TYPEDESC_CHAR, // Built-in type char
  TYPEDESC_BOOL, // Built-in type bool
  TYPEDESC_UNIT, // Built-in type unit
  TYPEDESC_FUN, // Function type
  TYPEDESC_STRUCT, // Struct type
  TYPEDESC_UNION, // Union type
  TYPEDESC_ENUM, // Enum type
} typedesc_kind_t;

/**
 * \brief Type descriptor.
 */
typedef struct typedesc_t
{
  TYPEDESC_HEADER;
} typedesc_t;

/**
 * \brief Frees all resources associated with a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_free(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a modifier.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a modifier, otherwise `false`.
 */
bool typedesc_is_modifier(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a built-in type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a built-in type, otherwise `false`.
 */
bool typedesc_is_builtin(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an integer type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an integer type, otherwise `false`.
 */
bool typedesc_is_integer(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a floating-point type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a floating-point type, otherwise
 * `false`.
 */
bool typedesc_is_float(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an arithmetic type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an arithmetic type, otherwise `false`.
 */
bool typedesc_is_arithmetic(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a signed arithmetic type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a signed arithmetic type, otherwise
 * `false`.
 */
bool typedesc_is_signed(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an unsigned arithmetic type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an unsigned arithmetic type, otherwise
 * `false`.
 */
bool typedesc_is_unsigned(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an invokable type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an invokable type, otherwise `false`.
 */
bool typedesc_is_invokable(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a composite type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a composite type, otherwise `false`.
 */
bool typedesc_is_composite(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a declared type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a declared type, otherwise `false`.
 */
bool typedesc_is_decl(typedesc_t* desc);

/**
 * \brief Removes the topmost mutable modifier from a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the mutable modifier.
 */
typedesc_t* typedesc_remove_mut(typedesc_t* desc);

/**
 * \brief Removes the topmost constant modifier from a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the constant modifier.
 */
typedesc_t* typedesc_remove_const(typedesc_t* desc);

/**
 * \brief Removes the topmost pointer modifier from a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the pointer modifier.
 */
typedesc_t* typedesc_remove_ptr(typedesc_t* desc);

/**
 * \brief Removes the topmost array modifier from a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the array modifier.
 */
typedesc_t* typedesc_remove_array(typedesc_t* desc);

/**
 * \brief Removes the topmost reference modifier from a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the reference modifier.
 */
typedesc_t* typedesc_remove_ref(typedesc_t* desc);

/**
 * \brief Removes the topmost optional modifier from a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the optional modifier.
 */
typedesc_t* typedesc_remove_opt(typedesc_t* desc);

/**
 * \brief Removes the topmost constant and mutable modifiers from a type
 * descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the constant and mutable
 * modifiers.
 */
typedesc_t* typedesc_remove_const_mut(typedesc_t* desc);

/**
 * \brief Removes the topmost constant and reference modifiers from a type
 * descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the constant and reference
 * modifiers.
 */
typedesc_t* typedesc_remove_const_ref(typedesc_t* desc);

/**
 * \brief Removes the topmost constant, reference and mutable modifiers from a
 * type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the constant, reference and
 * mutable modifiers.
 */
typedesc_t* typedesc_remove_const_ref_mut(typedesc_t* desc);

/**
 * \brief Checks if a modifier can be added to a type descriptor.
 * 
 * \param[in] kind The modifier kind.
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if the modifier can be added, `false` otherwise.
 */
bool typedesc_can_add_modifier(typedesc_kind_t kind, typedesc_t* desc);

/**
 * \brief Checks if a mutable modifier can be added to a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if a mutable modifier can be added, `false` otherwise.
 */
bool typedesc_can_add_mut(typedesc_t* desc);

/**
 * \brief Checks if a constant modifier can be added to a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if a constant modifier can be added, `false` otherwise.
 */
bool typedesc_can_add_const(typedesc_t* desc);

/**
 * \brief Checks if a pointer modifier can be added to a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if a pointer modifier can be added, `false` otherwise.
 */
bool typedesc_can_add_ptr(typedesc_t* desc);

/**
 * \brief Checks if an array modifier can be added to a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if an array modifier can be added, `false` otherwise.
 */
bool typedesc_can_add_array(typedesc_t* desc);

/**
 * \brief Checks if a reference modifier can be added to a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if a reference modifier can be added, `false` otherwise.
 */
bool typedesc_can_add_ref(typedesc_t* desc);

/**
 * \brief Checks if an optional modifier can be added to a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if an optional modifier can be added, `false` otherwise.
 */
bool typedesc_can_add_opt(typedesc_t* desc);

/**
 * \brief Checks if a type descriptor is implicitly convertible to another.
 * 
 * \param[in] from_desc Pointer to the type descriptor to convert from.
 * \param[in] to_desc Pointer to the type descriptor to convert into.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool typedesc_is_implicitly_convertible(typedesc_t* from_desc, typedesc_t* to_desc);

/**
 * \brief Retrieves the bit-width of an integer type descriptor.
 * 
 * \param desc Pointer to the type descriptor.
 * \returns The bit-width of the type.
 */
size_t typedesc_integer_bits(typedesc_t* desc);

/**
 * \brief Performs type promotion using the specified arithmetic types.
 * 
 * \param lhs_desc Pointer to the type descriptor.
 * \param rhs_desc Pointer to the type descriptor.
 * \returns Pointer to the promoted type.
 */
typedesc_t* typedesc_arithmetic_promote(typedesc_t* lhs_desc, typedesc_t* rhs_desc);

/**
 * \brief Checks if the given type descriptor is a callable type.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if the type descriptor is a callable, `false` otherwise.
 */
bool typedesc_is_callable(typedesc_t* desc);

/**
 * \brief Removes all modifiers and returns the underlying callable type of the
 * given type descriptor.
 * 
 * \param desc Pointer to callable type descriptor.
 * \returns Pointer to the underlying callable type descriptor.
 */
typedesc_t* typedesc_underlying_callable(typedesc_t* desc);

#endif