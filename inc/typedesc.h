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
 * \brief Utility macro which expands to fields that all types must have.
 */
#define TYPEDESC_HEADER\
  struct\
  {\
    typedesc_kind_t kind; /** Type kind. */\
    size_t size; /** Byte size of type. */\
    size_t align; /** Memory alignment of type. */\
  }\

/**
 * \brief Utility macro which expands to fields that all type modifiers must have.
 */
#define TYPEDESC_MODIFIER_HEADER\
  struct\
  {\
    typedesc_t* base_type; /** Underlying type. */\
  }\

/**
 * \brief Utility macro which expands to fields that all declared types must have.
 */
#define TYPEDESC_DECL_HEADER\
  struct\
  {\
    ast_node_t* node; /** Declaration node. */\
  }\

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
typedef struct typedesc_s
{
  TYPEDESC_HEADER;
} typedesc_t;

/**
 * \brief Type descriptor for type modifiers.
 */
typedef struct typedesc_modifier_s
{
  TYPEDESC_HEADER;
  TYPEDESC_MODIFIER_HEADER;
} typedesc_modifier_t;

/**
 * \brief Type descriptor for mutable types.
 */
typedef struct typedesc_mut_s
{
  TYPEDESC_HEADER;
  TYPEDESC_MODIFIER_HEADER;
} typedesc_mut_t;

/**
 * \brief Type descriptor for constant types.
 */
typedef struct typedesc_const_s
{
  TYPEDESC_HEADER;
  TYPEDESC_MODIFIER_HEADER;
} typedesc_const_t;

/**
 * \brief Type descriptor for pointer types.
 */
typedef struct typedesc_ptr_s
{
  TYPEDESC_HEADER;
  TYPEDESC_MODIFIER_HEADER;
} typedesc_ptr_t;

/**
 * \brief Type descriptor for array types.
 */
typedef struct typedesc_array_s
{
  TYPEDESC_HEADER;
  TYPEDESC_MODIFIER_HEADER;
} typedesc_array_t;

/**
 * \brief Type descriptor for reference types.
 */
typedef struct typedesc_ref_s
{
  TYPEDESC_HEADER;
  TYPEDESC_MODIFIER_HEADER;
} typedesc_ref_t;

/**
 * \brief Type descriptor for optional types.
 */
typedef struct typedesc_opt_s
{
  TYPEDESC_HEADER;
  TYPEDESC_MODIFIER_HEADER;
} typedesc_opt_t;

/**
 * \brief Type descriptor for functions.
 */
typedef struct typedesc_fun_s
{
  TYPEDESC_HEADER;
  list_t* param_types; // Parameter types.
  typedesc_t* return_type; // Return type.
} typedesc_fun_t;

/**
 * \brief Type descriptor for generators.
 */
typedef struct typedesc_gen_s
{
  TYPEDESC_HEADER;
  list_t* param_types; // List of parameter types.
  typedesc_t* yield_type; // Yield type.
} typedesc_gen_t;

/**
 * \brief Type descriptor for declarations.
 */
typedef struct typedesc_decl_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
} typedesc_decl_t;

/**
 * \brief Type descriptor for structures.
 */
typedef struct typedesc_struct_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
  list_t* member_types; // List of member types.
} typedesc_struct_t;

/**
 * \brief Type descriptor for unions.
 */
typedef struct typedesc_union_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
  list_t* member_types; // List of member types.
} typedesc_union_t;

/**
 * \brief Type descriptor for enumerations.
 */
typedef struct typedesc_enum_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
} typedesc_enum_t;

/**
 * \brief Type descriptor for modules.
 */
typedef struct typedesc_mod_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
  list_t* member_types; // List of member types.
} typedesc_mod_t;

#undef TYPEDESC_DECL_HEADER
#undef TYPEDESC_HEADER

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
 * \brief Checks if the given type descriptor is a declared type.
 *
 * \param[in] desc The type descriptor.
 * \returns `true` if the descriptor is a declared type, otherwise `false`.
 */
bool typedesc_is_decl(typedesc_t* desc);

/**
 * \brief Checks if two type descriptors represent the same type.
 *
 * \param[in] lhs Pointer to the first type descriptor.
 * \param[in] rhs Pointer to the second type descriptor.
 * \return `true` if the type descriptors are the same, `false` otherwise.
 */
bool typedesc_is_same(typedesc_t* first, typedesc_t* second);

#endif