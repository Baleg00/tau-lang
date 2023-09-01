/**
 * \file typedesc.h
 * 
 * \brief Type descriptor library interface.
 * 
 * \details Type descriptors store information about the characteristics and
 * properties of data types. They provide essential details about a type, such
 * as its size, alignment, modifiers (like const and mut), and its relationship
 * to other types (e.g., pointers, arrays, functions). Type descriptors
 * facilitate type checking, memory allocation, code generation, and other
 * compiler tasks by enabling the compiler to understand and manage the
 * semantics of different data types.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_H
#define TAU_TYPEDESC_H

#include <stdbool.h>
#include <stdint.h>

#include "ast.h"
#include "list.h"

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
  TYPEDESC_BOOL, // Built-in type bool
  TYPEDESC_UNIT, // Built-in type unit
  TYPEDESC_NULL, // Null type
  TYPEDESC_TYPE, // Type of type
  
  TYPEDESC_FUN, // Function type
  TYPEDESC_GEN, // Generator type
  TYPEDESC_STRUCT, // Struct type
  TYPEDESC_UNION, // Union type
  TYPEDESC_ENUM, // Enum type
  TYPEDESC_MOD, // Module type
} typedesc_kind_t;

/**
 * \brief Type descriptor.
 */
typedef struct typedesc_s typedesc_t;

/**
 * \brief Initializes a new type descriptor with the specified kind.
 * 
 * \param[in] kind The kind of the type descriptor.
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_t* typedesc_init(typedesc_kind_t kind);

/**
 * \brief Frees all memory allocated by type descriptors.
*/
void typedesc_cleanup(void);

/**
 * \brief Retrieves the kind of the type descriptor.
 *
 * \param[in] desc Pointer to the type descriptor.
 * \returns The kind of the type descriptor.
 */
typedesc_kind_t typedesc_get_kind(typedesc_t* desc);

/**
 * \brief Sets the kind of the type descriptor.
 *
 * \param[in,out] desc Pointer to the type descriptor.
 * \param[in] kind The kind to set.
 */
void typedesc_set_kind(typedesc_t* desc, typedesc_kind_t kind);

/**
 * \brief Retrieves the size of the type described.
 *
 * \param[in] desc Pointer to the type descriptor.
 * \returns The size of the type.
 */
size_t typedesc_get_size(typedesc_t* desc);

/**
 * \brief Sets the size of the type described.
 *
 * \param[in,out] desc Pointer to the type descriptor.
 * \param[in] size The size to set.
 */
void typedesc_set_size(typedesc_t* desc, size_t size);

/**
 * \brief Retrieves the alignment of the type described.
 *
 * \param[in] desc Pointer to the type descriptor.
 * \returns The alignment of the type.
 */
size_t typedesc_get_align(typedesc_t* desc);

/**
 * \brief Sets the alignment of the type described.
 *
 * \param[in,out] desc Pointer to the type descriptor.
 * \param[in] align The alignment to set.
 */
void typedesc_set_align(typedesc_t* desc, size_t align);

/**
 * \brief Retrieves the associated AST node of the type descriptor.
 *
 * \param[in] desc Pointer to the type descriptor.
 * \returns The associated AST node.
 */
ast_node_t* typedesc_get_node(typedesc_t* desc);

/**
 * \brief Sets the associated AST node of the type descriptor.
 *
 * \param[in,out] desc Pointer to the type descriptor.
 * \param[in] node The AST node to set.
 */
void typedesc_set_node(typedesc_t* desc, ast_node_t* node);

/**
 * \brief Retrieves the base type descriptor of the type described.
 *
 * \param[in] desc Pointer to the type descriptor.
 * \returns The base type descriptor.
 */
typedesc_t* typedesc_get_base(typedesc_t* desc);

/**
 * \brief Sets the base type descriptor of the type described.
 *
 * \param[in,out] desc Pointer to the type descriptor.
 * \param[in] base_type The base type descriptor to set.
 */
void typedesc_set_base(typedesc_t* desc, typedesc_t* base_type);

/**
 * \brief Retrieves the return type descriptor of the type described.
 *
 * \param[in] desc Pointer to the type descriptor.
 * \returns The return type descriptor.
 */
typedesc_t* typedesc_get_return(typedesc_t* desc);

/**
 * \brief Sets the return type descriptor of the type described.
 *
 * \param[in,out] desc Pointer to the type descriptor.
 * \param[in] return_type The return type descriptor to set.
 */
void typedesc_set_return(typedesc_t* desc, typedesc_t* return_type);

/**
 * \brief Retrieves the yield type descriptor of the type described.
 *
 * \param[in] desc Pointer to the type descriptor.
 * \returns The yield type descriptor.
 */
typedesc_t* typedesc_get_yield(typedesc_t* desc);

/**
 * \brief Sets the yield type descriptor of the type described.
 *
 * \param[in,out] desc Pointer to the type descriptor.
 * \param[in] yield_type The yield type descriptor to set.
 */
void typedesc_set_yield(typedesc_t* desc, typedesc_t* yield_type);

/**
 * \brief Retrieves the list of parameter type descriptors of the type described.
 *
 * \param[in] desc Pointer to the type descriptor.
 * \returns The list of parameter type descriptors.
 */
list_t* typedesc_get_params(typedesc_t* desc);

/**
 * \brief Sets the list of parameter type descriptors of the type described.
 *
 * \param[in,out] desc Pointer to the type descriptor.
 * \param[in] param_types The list of parameter type descriptors to set.
 */
void typedesc_set_params(typedesc_t* desc, list_t* param_types);

/**
 * \brief Retrieves the list of member type descriptors of the type described.
 *
 * \param[in] desc Pointer to the type descriptor.
 * \returns The list of member type descriptors.
 */
list_t* typedesc_get_members(typedesc_t* desc);

/**
 * \brief Sets the list of member type descriptors of the type described.
 *
 * \param[in,out] desc Pointer to the type descriptor.
 * \param[in] member_types The list of member type descriptors to set.
 */
void typedesc_set_members(typedesc_t* desc, list_t* member_types);

/**
 * \brief Creates a new mutable type descriptor based on the provided base type
 * descriptor.
 *
 * \param[in] base_type The base type descriptor.
 * \returns A newly created mutable type descriptor.
 */
typedesc_t* typedesc_add_mut(typedesc_t* base_type);

/**
 * \brief Creates a new constant type descriptor based on the provided base type
 * descriptor.
 *
 * \param[in] base_type The base type descriptor.
 * \returns A newly created constant type descriptor.
 */
typedesc_t* typedesc_add_const(typedesc_t* base_type);

/**
 * \brief Creates a new pointer type descriptor based on the provided base type
 * descriptor.
 *
 * \param[in] base_type The base type descriptor.
 * \returns A newly created pointer type descriptor.
 */
typedesc_t* typedesc_add_ptr(typedesc_t* base_type);

/**
 * \brief Creates a new array type descriptor based on the provided base type
 * descriptor.
 *
 * \param[in] base_type The base type descriptor.
 * \returns A newly created array type descriptor.
 */
typedesc_t* typedesc_add_array(typedesc_t* base_type);

/**
 * \brief Creates a new reference type descriptor based on the provided base
 * type descriptor.
 *
 * \param[in] base_type The base type descriptor.
 * \returns A newly created reference type descriptor.
 */
typedesc_t* typedesc_add_ref(typedesc_t* base_type);

/**
 * \brief Creates a new optional type descriptor based on the provided base type
 * descriptor.
 *
 * \param[in] base_type The base type descriptor.
 * \returns A newly created optional type descriptor.
 */
typedesc_t* typedesc_add_opt(typedesc_t* base_type);

/**
 * \brief Removed the topmost mutable qualifier from the provided type
 * descriptor, if present.
 *
 * \param[in] desc The type descriptor.
 * \returns A type descriptor with the mutable qualifier removed.
 */
typedesc_t* typedesc_remove_mut(typedesc_t* desc);

/**
 * \brief Removed the topmost constant qualifier from the provided type
 * descriptor, if present.
 *
 * \param[in] desc The type descriptor.
 * \returns A type descriptor with the constant qualifier removed.
 */
typedesc_t* typedesc_remove_const(typedesc_t* desc);

/**
 * \brief Removed the topmost pointer from the provided type descriptor, if
 * present.
 *
 * \param[in] desc The type descriptor.
 * \returns A type descriptor with the pointer removed.
 */
typedesc_t* typedesc_remove_ptr(typedesc_t* desc);

/**
 * \brief Removed the topmost array specifier from the provided type descriptor,
 * if present.
 *
 * \param[in] desc The type descriptor.
 * \returns A type descriptor with the array specifier removed.
 */
typedesc_t* typedesc_remove_array(typedesc_t* desc);

/**
 * \brief Removed the topmost reference specifier from the provided type
 * descriptor, if present.
 *
 * \param[in] desc The type descriptor.
 * \returns A type descriptor with the reference specifier removed.
 */
typedesc_t* typedesc_remove_ref(typedesc_t* desc);

/**
 * \brief Removed the topmost optional specifier from the provided type
 * descriptor, if present.
 *
 * \param[in] desc The type descriptor.
 * \returns A type descriptor with the optional specifier removed.
 */
typedesc_t* typedesc_remove_opt(typedesc_t* desc);

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
 * \brief Checks if the given type descriptor is a typename.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a typename, otherwise `false`.
 */
bool typedesc_is_type(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a mutable type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a mutable type, otherwise `false`.
 */
bool typedesc_is_mut(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a constant type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a constant type, otherwise `false`.
 */
bool typedesc_is_const(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a pointer type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a pointer type, otherwise `false`.
 */
bool typedesc_is_ptr(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an array type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an array type, otherwise `false`.
 */
bool typedesc_is_array(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a reference type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a reference type, otherwise `false`.
 */
bool typedesc_is_ref(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an optional type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an optional type, otherwise `false`.
 */
bool typedesc_is_opt(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `i8` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `i8` type, otherwise
 * `false`.
 */
bool typedesc_is_i8(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `i16` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `i16` type, otherwise
 * `false`.
 */
bool typedesc_is_i16(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `i32` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `i32` type, otherwise
 * `false`.
 */
bool typedesc_is_i32(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `i64` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `i64` type, otherwise
 * `false`.
 */
bool typedesc_is_i64(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `isize` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `isize` type, otherwise
 * `false`.
 */
bool typedesc_is_isize(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `u8` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `u8` type, otherwise
 * `false`.
 */
bool typedesc_is_u8(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `u16` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `u16` type, otherwise
 * `false`.
 */
bool typedesc_is_u16(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `u32` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `u32` type, otherwise
 * `false`.
 */
bool typedesc_is_u32(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `u64` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `u64` type, otherwise
 * `false`.
 */
bool typedesc_is_u64(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `usize` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `usize` type, otherwise
 * `false`.
 */
bool typedesc_is_usize(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `f32` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `f32` type, otherwise
 * `false`.
 */
bool typedesc_is_f32(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `f64` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `f64` type, otherwise
 * `false`.
 */
bool typedesc_is_f64(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `bool` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `bool` type, otherwise
 * `false`.
 */
bool typedesc_is_bool(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `unit` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `unit` type, otherwise
 * `false`.
 */
bool typedesc_is_unit(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is the built-in `null` type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is the built-in `null` type, otherwise
 * `false`.
 */
bool typedesc_is_null(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a function type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a function type, otherwise `false`.
 */
bool typedesc_is_fun(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a generator type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a generator type, otherwise `false`.
 */
bool typedesc_is_gen(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a structure type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a structure type, otherwise `false`.
 */
bool typedesc_is_struct(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a union type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a union type, otherwise `false`.
 */
bool typedesc_is_union(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is an enumeration type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is an enumeration type, otherwise `false`.
 */
bool typedesc_is_enum(typedesc_t* desc);

/**
 * \brief Checks if the given type descriptor is a module type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a module type, otherwise `false`.
 */
bool typedesc_is_mod(typedesc_t* desc);

/**
 * \brief Checks if two type descriptors represent the same type.
 *
 * \param[in] lhs Pointer to the first type descriptor.
 * \param[in] rhs Pointer to the second type descriptor.
 * \return `true` if the type descriptors are the same, `false` otherwise.
 */
bool typedesc_is_same(typedesc_t* first, typedesc_t* second);

#endif