/**
 * \file typeset.h
 * 
 * \brief Type set library interface.
 * 
 * \details A type set is a data structure designed to manage a collection of
 * types. It ensures that the contained types are unique, preventing duplicates,
 * and manages the memory and resources associated with these types, ensuring
 * proper deallocation when they are no longer needed. Type sets are crucial to
 * maintain a clean and efficient type system, preventing unnecessary duplication
 * of type descriptors and ensuring that resources are handled correctly. This
 * helps reduce memory overhead and enhance type consistency.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPESET_H
#define TAU_TYPESET_H

#include "ast.h"
#include "list.h"
#include "typedesc.h"

/**
 * \brief Represents a type set.
 * 
 * \note The type set is an owning data structure. When a type is added, the set
 * becomes responsible for managing the type's memory, including freeing it when
 * the set is destroyed. Avoid manually freeing types that are part of a type set
 * to prevent double-free errors.
 */
typedef struct typeset_t typeset_t;

/**
 * \brief Initializes a new type set.
 *
 * \returns A pointer to the newly initialized type set.
 */
typeset_t* typeset_init(void);

/**
 * \brief Frees the resources associated with a type set.
 *
 * \param[in] typeset Pointer to the type set to be freed.
 */
void typeset_free(typeset_t* typeset);

/**
 * \brief Looks up and retrieves a mutable version of a type in a type set.
 *
 * \details This function checks if a mutable type with the specified base type
 * is already present in the set. If found, it returns a pointer to the existing
 * type; otherwise, it adds the new type to the set andreturns it.
 *
 * \param[in] typeset Pointer to the type set.
 * \param[in] base_type The base type to look up or add.
 * \returns A pointer to the type in the type set.
 */
typedesc_t* typeset_lookup_mut(typeset_t* typeset, typedesc_t* base_type);

/**
 * \brief Looks up and retrieves a constant version of a type in a type set.
 *
 * \details This function checks if a constant type with the specified base type
 * is already present in the set. If found, it returns a pointer to the existing
 * type; otherwise, it adds the new type to the set andreturns it.
 *
 * \param[in] typeset Pointer to the type set.
 * \param[in] base_type The base type to look up or add.
 * \returns A pointer to the type in the type set.
 */
typedesc_t* typeset_lookup_const(typeset_t* typeset, typedesc_t* base_type);

/**
 * \brief Looks up and retrieves a pointer version of a type in a type set.
 *
 * \details This function checks if a pointer type with the specified base type
 * is already present in the set. If found, it returns a pointer to the existing
 * type; otherwise, it adds the new type to the set andreturns it.
 *
 * \param[in] typeset Pointer to the type set.
 * \param[in] base_type The base type to look up or add.
 * \returns A pointer to the type in the type set.
 */
typedesc_t* typeset_lookup_ptr(typeset_t* typeset, typedesc_t* base_type);

/**
 * \brief Looks up and retrieves an array version of a type in a type set.
 *
 * \details This function checks if an array type with the specified length and
 * base type is already present in the set. If found, it returns a pointer to
 * the existing type; otherwise, it adds the new type to the set andreturns it.
 *
 * \param[in] typeset Pointer to the type set.
 * \param[in] base_type The base type to look up or add.
 * \returns A pointer to the type in the type set.
 */
typedesc_t* typeset_lookup_array(typeset_t* typeset, size_t length, typedesc_t* base_type);

/**
 * \brief Looks up and retrieves a reference version of a type in a type set.
 *
 * \details This function checks if a reference type with the specified base type
 * is already present in the set. If found, it returns a pointer to the existing
 * type; otherwise, it adds the new type to the set andreturns it.
 *
 * \param[in] typeset Pointer to the type set.
 * \param[in] base_type The base type to look up or add.
 * \returns A pointer to the type in the type set.
 */
typedesc_t* typeset_lookup_ref(typeset_t* typeset, typedesc_t* base_type);

/**
 * \brief Looks up and retrieves an optional version of a type in a type set.
 *
 * \details This function checks if an optional type with the specified base
 * type is already present in the set. If found, it returns a pointer to the
 * existing type; otherwise, it adds the new type to the set and returns it.
 *
 * \param[in] typeset Pointer to the type set.
 * \param[in] base_type The base type to look up or add.
 * \returns A pointer to the type in the type set.
 */
typedesc_t* typeset_lookup_opt(typeset_t* typeset, typedesc_t* base_type);

/**
 * \brief Retrieves a type descriptor for a signed 8-bit integer.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for a signed 8-bit integer.
 */
typedesc_t* typeset_lookup_i8(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for a signed 16-bit integer.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for a signed 16-bit integer.
 */
typedesc_t* typeset_lookup_i16(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for a signed 32-bit integer.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for a signed 32-bit integer.
 */
typedesc_t* typeset_lookup_i32(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for a signed 64-bit integer.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for a signed 64-bit integer.
 */
typedesc_t* typeset_lookup_i64(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for a signed size integer.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for a signed size integer.
 */
typedesc_t* typeset_lookup_isize(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for an unsigned 8-bit integer.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for an unsigned 8-bit integer.
 */
typedesc_t* typeset_lookup_u8(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for an unsigned 16-bit integer.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for an unsigned 16-bit integer.
 */
typedesc_t* typeset_lookup_u16(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for an unsigned 32-bit integer.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for an unsigned 32-bit integer.
 */
typedesc_t* typeset_lookup_u32(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for an unsigned 64-bit integer.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for an unsigned 64-bit integer.
 */
typedesc_t* typeset_lookup_u64(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for an unsigned size integer.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for an unsigned size integer.
 */
typedesc_t* typeset_lookup_usize(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for a 32-bit floating-point number.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for a 32-bit floating-point number.
 */
typedesc_t* typeset_lookup_f32(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for a 64-bit floating-point number.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for a 64-bit floating-point number.
 */
typedesc_t* typeset_lookup_f64(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for a boolean type.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for a boolean type.
 */
typedesc_t* typeset_lookup_bool(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for a unit type.
 *
 * \param[in] typeset The type set.
 * \returns A type descriptor for a unit type.
 */
typedesc_t* typeset_lookup_unit(typeset_t* typeset);

/**
 * \brief Retrieves a type descriptor for a function type.
 *
 * \details This function checks if a function type with the specified return
 * type and parameter types is already present in the set. If found, it returns
 * a pointer to the existing type; otherwise, it adds the new type to the set
 * and returns it.
 * 
 * \param[in] typeset The type set.
 * \param[in] return_type The return type of the function.
 * \param[in] param_types The parameter types of the function.
 * \param[in] param_count The number of parameters.
 * \param[in] is_vararg `true` if the function is variadic, `false` otherwise.
 * \param[in] abi The function ABI.
 * \return A type descriptor for the function type.
 */
typedesc_t* typeset_lookup_fun(typeset_t* typeset, typedesc_t* return_type, typedesc_t* param_types[], size_t param_count, bool is_vararg, abi_kind_t abi);

/**
 * \brief Retrieves a type descriptor for a generator type.
 *
 * \details This function checks if a generator type with the specified return
 * type and parameter types is already present in the set. If found, it returns
 * a pointer to the existing type; otherwise, it adds the new type to the set
 * and returns it.
 * 
 * \param[in] typeset The type set.
 * \param[in] yield_type The yield type of the generator.
 * \param[in] param_types The parameter types of the generator.
 * \param[in] param_count The number of parameters.
 * \return A type descriptor for the generator type.
 */
typedesc_t* typeset_lookup_gen(typeset_t* typeset, typedesc_t* yield_type, typedesc_t* param_types[], size_t param_count);

/**
 * \brief Retrieves a type descriptor for a struct type.
 *
 * \details This function checks if a struct type with the specified node and
 * field types is already present in the set. If found, it returns a pointer to
 * the existing type; otherwise, it adds the new type to the set and returns it.
 * 
 * \param[in] typeset The type set.
 * \param[in] node The AST node of the struct type.
 * \param[in] field_types The types of the struct fields.
 * \param[in] field_count The number of fields.
 * \return A type descriptor for the struct type.
 */
typedesc_t* typeset_lookup_struct(typeset_t* typeset, ast_node_t* node, typedesc_t* field_types[], size_t param_count);

/**
 * \brief Retrieves a type descriptor for a union type.
 *
 * \details This function checks if a union type with the specified node and
 * field types is already present in the set. If found, it returns a pointer to
 * the existing type; otherwise, it adds the new type to the set and returns it.
 * 
 * \param[in] typeset The type set.
 * \param[in] node The AST node of the union type.
 * \param[in] field_types The types of the union fields.
 * \param[in] field_count The number of fields.
 * \return A type descriptor for the union type.
 */
typedesc_t* typeset_lookup_union(typeset_t* typeset, ast_node_t* node, typedesc_t* field_types[], size_t param_count);

/**
 * \brief Retrieves a type descriptor for an enum type.
 *
 * \details This function checks if an enum type with the specified node is
 * already present in the set. If found, it returns a pointer to the existing
 * type; otherwise, it adds the new type to the set and returns it.
 * 
 * \param[in] typeset The type set.
 * \param[in] node The AST node of the enum type.
 * \return A type descriptor for the enum type.
 */
typedesc_t* typeset_lookup_enum(typeset_t* typeset, ast_node_t* node);

#endif
