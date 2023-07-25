/**
 * \file
 * 
 * Type library for type-checking.
*/

#ifndef TAU_TYPEDESC_H
#define TAU_TYPEDESC_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "typedefs.h"
#include "list.h"

/** Platform specific byte size of a pointer. */
#define TYPEDESC_PTR_SIZE sizeof(void*)

/** Platform specific alignment of a pointer. */
#define TYPEDESC_PTR_ALIGN sizeof(void*)

/** Utility macro which expands to fields that all types must have. */
#define TYPEDESC_HEADER\
  struct\
  {\
    typedesc_kind_t kind; /** Type kind. */\
    size_t size; /** Byte size of type. */\
    size_t align; /** Memory alignment of type. */\
  }\

/** Utility macro which expands to fields that all declared types must have. */
#define TYPEDESC_DECL_HEADER\
  struct\
  {\
    ast_decl_t* node; /** Declaration node. */\
  }\

/** Base type for all types. */
struct typedesc_s
{
  TYPEDESC_HEADER;
};

/** Type for mutable types. */
struct typedesc_mut_s
{
  TYPEDESC_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for compile-time types. */
struct typedesc_const_s
{
  TYPEDESC_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for pointer types. */
struct typedesc_ptr_s
{
  TYPEDESC_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for array types. */
struct typedesc_array_s
{
  TYPEDESC_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for reference types. */
struct typedesc_ref_s
{
  TYPEDESC_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for optional types. */
struct typedesc_opt_s
{
  TYPEDESC_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for function types. */
struct typedesc_fun_s
{
  TYPEDESC_HEADER;
  list_t* param_types; // Parameter types.
  typedesc_t* return_type; // Return type.
};

/** Type for generator types. */
struct typedesc_gen_s
{
  TYPEDESC_HEADER;
  list_t* param_types; // List of parameter types.
  typedesc_t* yield_type; // Yield type.
};

/** Base type for all declared types. */
struct typedesc_decl_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
};

/** Type for struct types. */
struct typedesc_struct_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
  list_t* member_types; // List of member types.
};

/** Type for union types. */
struct typedesc_union_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
  list_t* member_types; // List of member types.
};

/** Type for enum types. */
struct typedesc_enum_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
};

/** Type for module types. */
struct typedesc_mod_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
  list_t* member_types; // List of member types.
};

#undef TYPEDESC_DECL_HEADER
#undef TYPEDESC_HEADER

/**
 * \brief Initializes a type instance.
 * 
 * \param[out] desc Type instance to be initialized.
 * \param[in] kind Type kind.
 * \param[in] size Type size in bytes.
 * \param[in] align Type memory alignment.
*/
void typedesc_init(typedesc_t* type, typedesc_kind_t kind, size_t size, size_t align);

/**
 * \brief Returns a built-in type.
 * 
 * \details Built-in types don't need to be allocated every time they are used.
 * Built-in type instances can be retrieved using this function.
 * 
 * \param[in] kind Built-in type kind.
 * \returns Built-in type.
*/
typedesc_t* typedesc_builtin(typedesc_kind_t kind);

/**
 * \brief Destroys a type.
 * 
 * \param[in] type Type to be destroyed.
*/
void typedesc_free(typedesc_t* type);

/**
 * \brief Checks if a type is a modifier.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is a modifier, false otherwise.
*/
bool typedesc_is_modifier(typedesc_t* type);

/**
 * \brief Checks if a type is a built-in type.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is built-in, false otherwise.
*/
bool typedesc_is_builtin(typedesc_t* type);

/**
 * \brief Checks if a type is an integer.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is an integer, false otherwise.
*/
bool typedesc_is_integer(typedesc_t* type);

/**
 * \brief Checks if a type is a float.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is a float, false otherwise.
*/
bool typedesc_is_float(typedesc_t* type);

/**
 * \brief Checks if a type is an arithmetic type.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is arithmetic, false otherwise.
*/
bool typedesc_is_arithmetic(typedesc_t* type);

/**
 * \brief Checks if a type is a signed type.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is signed, false otherwise.
*/
bool typedesc_is_signed(typedesc_t* type);

/**
 * \brief Checks if a type is an unsigned type.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is unsigned, false otherwise.
*/
bool typedesc_is_unsigned(typedesc_t* type);

/**
 * \brief Checks if a type is an owner type.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is an owner, false otherwise.
*/
bool typedesc_is_owner(typedesc_t* type);

/**
 * \brief Checks if two types are the same.
 * 
 * \param[in] lhs Type to be checked.
 * \param[in] rhs Type to be checked.
 * \returns True if the two types are the same, false otherwise.
*/
bool typedesc_is_same(typedesc_t* lhs, typedesc_t* rhs);

/**
 * \brief Checks if a type can be assigned from another.
 * 
 * \param[in] to Type to be assigned to.
 * \param[in] from Type to be assigned from.
 * \returns True if the assignment is valid, false otherwise.
*/
bool typedesc_is_assignable(typedesc_t* to, typedesc_t* from);

/**
 * \brief Converts an unsigned type to its signed version.
 * 
 * \param[in] type Type to be converted.
 * \returns Signed version of type.
*/
typedesc_t* typedesc_make_signed(typedesc_t* type);

/**
 * \brief Adds a pointer to a type.
 * 
 * \param[in] arena Arena allocator to be used.
 * \param[in] type Type to be pointed to.
 * \returns Pointer type.
*/
typedesc_t* typedesc_make_ptr(arena_t* arena, typedesc_t* type);

/**
 * \brief Adds a reference to a type.
 * 
 * \param[in] arena Arena allocator to be used.
 * \param[in] type Type to be referenced.
 * \returns Reference type.
*/
typedesc_t* typedesc_make_ref(arena_t* arena, typedesc_t* type);

/**
 * \brief Removes a reference from a type.
 * 
 * \param[in] type Type to be modified.
 * \returns Type without reference.
*/
typedesc_t* typedesc_remove_ref(typedesc_t* type);

/**
 * \brief Takes two arithmetic types and returns a type that would be the result
 * of a binary operation (e.g. addition) between them.
 * 
 * \param[in] lhs Left-hand side arithmetic type.
 * \param[in] rhs Right-hand side arithmetic type.
 * \returns Promoted type.
*/
typedesc_t* typedesc_promote(typedesc_t* lhs, typedesc_t* rhs);

/**
 * \brief Prints a human-readable representation of a type into a stream.
 * 
 * \param[in] stream Stream to be printed to.
 * \param[in] type Type to be printed.
 * \returns Number of characters printed.
*/
size_t typedesc_print(FILE* stream, typedesc_t* type);

#endif