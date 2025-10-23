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

#include "llvm.h"
#include "utils/common.h"

/**
 * \brief Header for all type descriptors.
 */
#define TAU_TYPEDESC_HEADER\
  tau_typedesc_kind_t kind; /** Type kind. */\
  LLVMTypeRef llvm_type /** LLVM type. */\

TAU_EXTERN_C_BEGIN

/**
 * \see ast/node.h 
 */
typedef struct tau_ast_node_t tau_ast_node_t;

/**
 * \brief Enumeration of type descriptor kinds.
 */
typedef enum tau_typedesc_kind_e
{
  TAU_TYPEDESC_MUT,    ///< Mutable type.
  TAU_TYPEDESC_PTR,    ///< Pointer type.
  TAU_TYPEDESC_ARRAY,  ///< Array type.
  TAU_TYPEDESC_REF,    ///< Reference type.
  TAU_TYPEDESC_OPT,    ///< Optional type.
  TAU_TYPEDESC_VEC,    ///< Vector type.
  TAU_TYPEDESC_MAT,    ///< Matrix type.
  TAU_TYPEDESC_I8,     ///< Built-in type `i8`.
  TAU_TYPEDESC_I16,    ///< Built-in type `i16`.
  TAU_TYPEDESC_I32,    ///< Built-in type `i32`.
  TAU_TYPEDESC_I64,    ///< Built-in type `i64`.
  TAU_TYPEDESC_ISIZE,  ///< Built-in type `isize`.
  TAU_TYPEDESC_U8,     ///< Built-in type `u8`.
  TAU_TYPEDESC_U16,    ///< Built-in type `u16`.
  TAU_TYPEDESC_U32,    ///< Built-in type `u32`.
  TAU_TYPEDESC_U64,    ///< Built-in type `u64`.
  TAU_TYPEDESC_USIZE,  ///< Built-in type `usize`.
  TAU_TYPEDESC_F32,    ///< Built-in type `f32`.
  TAU_TYPEDESC_F64,    ///< Built-in type `f64`.
  TAU_TYPEDESC_C64,    ///< Built-in type `c64`.
  TAU_TYPEDESC_C128,   ///< Built-in type `c128`.
  TAU_TYPEDESC_CHAR,   ///< Built-in type `char`.
  TAU_TYPEDESC_BOOL,   ///< Built-in type `bool`.
  TAU_TYPEDESC_UNIT,   ///< Built-in type `unit`.
  TAU_TYPEDESC_FUN,    ///< Function type.
  TAU_TYPEDESC_STRUCT, ///< Struct type.
  TAU_TYPEDESC_UNION,  ///< Union type.
  TAU_TYPEDESC_ENUM,   ///< Enum type.
  TAU_TYPEDESC_VAR,    ///< Type variable.
  TAU_TYPEDESC_POISON, ///< Poison type.
} tau_typedesc_kind_t;

/**
 * \brief Type descriptor.
 */
typedef struct tau_typedesc_t
{
  TAU_TYPEDESC_HEADER;
} tau_typedesc_t;

/**
 * \brief Frees all resources associated with a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void tau_typedesc_free(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a modifier.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a modifier, otherwise `false`.
 */
bool tau_typedesc_is_modifier(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a built-in type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a built-in type, otherwise `false`.
 */
bool tau_typedesc_is_builtin(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an integer type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an integer type, otherwise `false`.
 */
bool tau_typedesc_is_integer(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a floating-point type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a floating-point type, otherwise
 * `false`.
 */
bool tau_typedesc_is_float(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a complex floating-point type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a complex floating-point type, otherwise
 * `false`.
 */
bool tau_typedesc_is_complex(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a vector type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a vector type, otherwise `false`.
 */
bool tau_typedesc_is_vector(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a matrix type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a matrix type, otherwise `false`.
 */
bool tau_typedesc_is_matrix(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an arithmetic type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an arithmetic type, otherwise `false`.
 */
bool tau_typedesc_is_arithmetic(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a signed arithmetic type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a signed arithmetic type, otherwise
 * `false`.
 */
bool tau_typedesc_is_signed(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an unsigned arithmetic type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an unsigned arithmetic type, otherwise
 * `false`.
 */
bool tau_typedesc_is_unsigned(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an invokable type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an invokable type, otherwise `false`.
 */
bool tau_typedesc_is_invokable(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a composite type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a composite type, otherwise `false`.
 */
bool tau_typedesc_is_composite(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a declared type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a declared type, otherwise `false`.
 */
bool tau_typedesc_is_decl(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a mutable type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a mutable type, otherwise `false`.
 */
bool tau_typedesc_is_mut(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a pointer type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a pointer type, otherwise `false`.
 */
bool tau_typedesc_is_ptr(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an array type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an array type, otherwise `false`.
 */
bool tau_typedesc_is_array(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a reference type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a reference type, otherwise `false`.
 */
bool tau_typedesc_is_ref(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an optional type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an optional type, otherwise `false`.
 */
bool tau_typedesc_is_opt(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a poison type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a poison type, otherwise `false`.
 */
bool tau_typedesc_is_poison(tau_typedesc_t* desc);

/**
 * \brief Removes the topmost mutable modifier from a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the mutable modifier.
 */
tau_typedesc_t* tau_typedesc_remove_mut(tau_typedesc_t* desc);

/**
 * \brief Removes the topmost pointer modifier from a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the pointer modifier.
 */
tau_typedesc_t* tau_typedesc_remove_ptr(tau_typedesc_t* desc);

/**
 * \brief Removes the topmost array modifier from a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the array modifier.
 */
tau_typedesc_t* tau_typedesc_remove_array(tau_typedesc_t* desc);

/**
 * \brief Removes the topmost reference modifier from a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the reference modifier.
 */
tau_typedesc_t* tau_typedesc_remove_ref(tau_typedesc_t* desc);

/**
 * \brief Removes the topmost reference and mutable modifiers from a type
 * descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the reference and mutable
 * modifier.
 */
tau_typedesc_t* tau_typedesc_remove_ref_mut(tau_typedesc_t* desc);

/**
 * \brief Removes the topmost optional modifier from a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns Pointer to the type descriptor without the optional modifier.
 */
tau_typedesc_t* tau_typedesc_remove_opt(tau_typedesc_t* desc);

/**
 * \brief Checks if a modifier can be added to a type descriptor.
 * 
 * \param[in] kind The modifier kind.
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if the modifier can be added, `false` otherwise.
 */
bool tau_typedesc_can_add_modifier(tau_typedesc_kind_t kind, tau_typedesc_t* desc);

/**
 * \brief Checks if a mutable modifier can be added to a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if a mutable modifier can be added, `false` otherwise.
 */
bool tau_typedesc_can_add_mut(tau_typedesc_t* desc);

/**
 * \brief Checks if a pointer modifier can be added to a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if a pointer modifier can be added, `false` otherwise.
 */
bool tau_typedesc_can_add_ptr(tau_typedesc_t* desc);

/**
 * \brief Checks if an array modifier can be added to a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if an array modifier can be added, `false` otherwise.
 */
bool tau_typedesc_can_add_array(tau_typedesc_t* desc);

/**
 * \brief Checks if a reference modifier can be added to a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if a reference modifier can be added, `false` otherwise.
 */
bool tau_typedesc_can_add_ref(tau_typedesc_t* desc);

/**
 * \brief Checks if an optional modifier can be added to a type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if an optional modifier can be added, `false` otherwise.
 */
bool tau_typedesc_can_add_opt(tau_typedesc_t* desc);

/**
 * \brief Checks if a type is implicitly direct convertible to another type.
 *
 * \param[in] src_desc Pointer to the source type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_is_implicitly_direct_convertible(tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if a type is implicitly indirect convertible to another type.
 *
 * \param[in] src_desc Pointer to the source type descriptor.
 * \param[in] dst_desc Pointer to the destination type descriptor.
 * \returns `true` if the implicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_is_implicitly_indirect_convertible(tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Checks if a type is explicitly convertible to another type.
 * 
 * \param[in] src_desc Pointer to the source type descriptor.
 * \param[in] dst_desc Pointer to the type descriptor to check conversion into.
 * \returns `true` if the explicit conversion is possible, `false` otherwise.
 */
bool tau_typedesc_is_explicitly_convertible(tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc);

/**
 * \brief Retrieves the bit-width of an integer type descriptor.
 * 
 * \param desc Pointer to the type descriptor.
 * \returns The bit-width of the type.
 */
size_t tau_typedesc_integer_bits(tau_typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a callable type.
 * 
 * \param[in] desc Pointer to the type descriptor.
 * \returns `true` if the type descriptor is a callable, `false` otherwise.
 */
bool tau_typedesc_is_callable(tau_typedesc_t* desc);

/**
 * \brief Removes all modifiers and returns the underlying callable type of the
 * given type descriptor.
 * 
 * \param desc Pointer to callable type descriptor.
 * \returns Pointer to the underlying callable type descriptor.
 */
tau_typedesc_t* tau_typedesc_underlying_callable(tau_typedesc_t* desc);

TAU_EXTERN_C_END

#endif
