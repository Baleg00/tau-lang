/**
 * \file typebuilder.h
 * 
 * \brief Type builder interface.
 * 
 * \details A type builder is a utility that simplifies the process of defining
 * complex data types. It abstracts away the low-level details of type creation,
 * allowing developers to define and manipulate types in a more high-level and
 * intuitive manner. It enables the specification of the structure, fields,
 * methods, and relationships of custom types in a concise and readable way,
 * improving code maintainability and reducing the risk of type-related errors.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEBUILDER_H
#define TAU_TYPEBUILDER_H

#include <llvm-c/Types.h>
#include <llvm-c/Target.h>

#include "stages/analyzer/typedesc.h"
#include "utils/str.h"

/**
 * \brief Represents a type builder.
 * 
 * \note The type builder takes ownership of the types it constructs. When a type
 * is built, the builder becomes responsible for managing the type's memory,
 * including freeing it when the builder is destroyed. Avoid manually freeing
 * types that are returned by a type builder to prevent double-free errors.
 */
typedef struct typebuilder_t typebuilder_t;

/**
 * \brief Initializes a new type builder.
 *
 * \param[in] llvm_context The LLVM context to be used.
 * \param[in] llvm_layout The LLVM target data layout to be used.
 * \returns A pointer to the newly initialized type builder.
 */
typebuilder_t* typebuilder_init(LLVMContextRef llvm_context, LLVMTargetDataRef llvm_layout);

/**
 * \brief Frees the resources associated with the type builder.
 *
 * \param[in] builder Pointer to the type builder to be freed.
 */
void typebuilder_free(typebuilder_t* builder);

/**
 * \brief Builds a mutable version of the provided base type.
 *
 * \param[in] builder Pointer to the type builder.
 * \param[in] base_type The base type to be modified.
 * \returns A pointer to the newly created mutable type descriptor.
 */
typedesc_t* typebuilder_build_mut(typebuilder_t* builder, typedesc_t* base_type);

/**
 * \brief Builds a constant version of the provided base type.
 *
 * \param[in] builder Pointer to the type builder.
 * \param[in] base_type The base type to be modified.
 * \returns A pointer to the newly created constant type descriptor.
 */
typedesc_t* typebuilder_build_const(typebuilder_t* builder, typedesc_t* base_type);

/**
 * \brief Builds a pointer type with the provided base type.
 *
 * \param[in] builder Pointer to the type builder.
 * \param[in] base_type The base type for the pointer.
 * \returns A pointer to the newly created pointer type descriptor.
 */
typedesc_t* typebuilder_build_ptr(typebuilder_t* builder, typedesc_t* base_type);

/**
 * \brief Builds an array type with the provided base type.
 *
 * \param[in] builder Pointer to the type builder.
 * \param[in] length The number of elements in the array.
 * \param[in] base_type The base type for the array.
 * \returns A pointer to the newly created array type descriptor.
 */
typedesc_t* typebuilder_build_array(typebuilder_t* builder, size_t length, typedesc_t* base_type);

/**
 * \brief Builds a reference type with the provided base type.
 *
 * \param[in] builder Pointer to the type builder.
 * \param[in] base_type The base type for the reference.
 * \returns A pointer to the newly created reference type descriptor.
 */
typedesc_t* typebuilder_build_ref(typebuilder_t* builder, typedesc_t* base_type);

/**
 * \brief Builds an optional type with the provided base type.
 *
 * \param[in] builder Pointer to the type builder.
 * \param[in] base_type The base type for the optional.
 * \returns A pointer to the newly created optional type descriptor.
 */
typedesc_t* typebuilder_build_opt(typebuilder_t* builder, typedesc_t* base_type);

/**
 * \brief Builds a signed 8-bit integer type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created 8-bit integer type descriptor.
 */
typedesc_t* typebuilder_build_i8(typebuilder_t* builder);

/**
 * \brief Builds a signed 16-bit integer type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created 16-bit integer type descriptor.
 */
typedesc_t* typebuilder_build_i16(typebuilder_t* builder);

/**
 * \brief Builds a signed 32-bit integer type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created 32-bit integer type descriptor.
 */
typedesc_t* typebuilder_build_i32(typebuilder_t* builder);

/**
 * \brief Builds a signed 64-bit integer type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created 64-bit integer type descriptor.
 */
typedesc_t* typebuilder_build_i64(typebuilder_t* builder);

/**
 * \brief Builds a signed size type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created size type descriptor.
 */
typedesc_t* typebuilder_build_isize(typebuilder_t* builder);

/**
 * \brief Builds a unsigned 8-bit integer type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created 8-bit integer type descriptor.
 */
typedesc_t* typebuilder_build_u8(typebuilder_t* builder);

/**
 * \brief Builds a unsigned 16-bit integer type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created 16-bit integer type descriptor.
 */
typedesc_t* typebuilder_build_u16(typebuilder_t* builder);

/**
 * \brief Builds a unsigned 32-bit integer type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created 32-bit integer type descriptor.
 */
typedesc_t* typebuilder_build_u32(typebuilder_t* builder);

/**
 * \brief Builds a unsigned 64-bit integer type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created 64-bit integer type descriptor.
 */
typedesc_t* typebuilder_build_u64(typebuilder_t* builder);

/**
 * \brief Builds a unsigned size type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created size type descriptor.
 */
typedesc_t* typebuilder_build_usize(typebuilder_t* builder);

/**
 * \brief Builds a 32-bit floating-point type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created 32-bit floating-point type descriptor.
 */
typedesc_t* typebuilder_build_f32(typebuilder_t* builder);

/**
 * \brief Builds a 64-bit floating-point type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created 64-bit floating-point type descriptor.
 */
typedesc_t* typebuilder_build_f64(typebuilder_t* builder);

/**
 * \brief Builds a boolean type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created boolean type descriptor.
 */
typedesc_t* typebuilder_build_bool(typebuilder_t* builder);

/**
 * \brief Builds a unit type.
 *
 * \param[in] builder Pointer to the type builder.
 * \returns A pointer to the newly created unit type descriptor.
 */
typedesc_t* typebuilder_build_unit(typebuilder_t* builder);

/**
 * \brief Builds a function type with the provided return type and parameter types.
 *
 * \param[in] builder Pointer to the type builder.
 * \param[in] return_type The return type of the function.
 * \param[in] param_types An array of parameter types for the function.
 * \param[in] param_count The number of parameters.
 * \param[in] is_vararg `true` if the function is variadic, `false` otherwise.
 * \param[in] callconv The function calling convention.
 * \returns A pointer to the newly created function type descriptor.
 */
typedesc_t* typebuilder_build_fun(typebuilder_t* builder, typedesc_t* return_type, typedesc_t* param_types[], size_t param_count, bool is_vararg, callconv_kind_t callconv);

/**
 * \brief Builds a struct type with the provided identifier and field types.
 *
 * \param[in] builder Pointer to the type builder.
 * \param[in] node The AST node of the struct type.
 * \param[in] field_types An array of field types for the struct.
 * \param[in] field_count The number of fields.
 * \returns A pointer to the newly created struct type descriptor.
 */
typedesc_t* typebuilder_build_struct(typebuilder_t* builder, ast_node_t* node, typedesc_t* field_types[], size_t field_count);

/**
 * \brief Builds a union type with the provided identifier and field types.
 *
 * \param[in] builder Pointer to the type builder.
 * \param[in] node The AST node of the union type.
 * \param[in] field_types An array of field types for the union.
 * \param[in] field_count The number of fields.
 * \returns A pointer to the newly created union type descriptor.
 */
typedesc_t* typebuilder_build_union(typebuilder_t* builder, ast_node_t* node, typedesc_t* field_types[], size_t field_count);

/**
 * \brief Builds an enum type with the provided identifier.
 *
 * \param[in] builder Pointer to the type builder.
 * \param[in] node The AST node of the enum type.
 * \returns A pointer to the newly created enum type descriptor.
 */
typedesc_t* typebuilder_build_enum(typebuilder_t* builder, ast_node_t* node);

#endif