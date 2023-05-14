/**
 * \file
 * 
 * Type library for type-checking.
*/

#ifndef TAU_TYPEDESC_H
#define TAU_TYPEDESC_H

#include <stdbool.h>
#include <stdio.h>

#include "typedefs.h"

/** Utility macro which expands to fields that all types must have. */
#define TYPE_HEADER\
  struct\
  {\
    typedesc_kind_t kind; /** Type kind. */\
  }\

/** Utility macro which expands to fields that all declared types must have. */
#define TYPE_DECL_HEADER\
  struct\
  {\
    ast_decl_t* node; /** Declaration node. */\
  }\

/** Base type for all types. */
struct typedesc_s
{
  TYPE_HEADER;
};

/** Type for mutable types. */
struct typedesc_mut_s
{
  TYPE_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for compile-time types. */
struct typedesc_const_s
{
  TYPE_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for pointer types. */
struct typedesc_ptr_s
{
  TYPE_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for array types. */
struct typedesc_array_s
{
  TYPE_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for reference types. */
struct typedesc_ref_s
{
  TYPE_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for optional types. */
struct typedesc_opt_s
{
  TYPE_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for function types. */
struct typedesc_fun_s
{
  TYPE_HEADER;
  list_t* param_types; // Parameter types.
  typedesc_t* return_type; // Return type.
};

/** Type for generator types. */
struct typedesc_gen_s
{
  TYPE_HEADER;
  list_t* param_types; // List of parameter types.
  typedesc_t* yield_type; // Yield type.
};

/** Base type for all declared types. */
struct typedesc_decl_s
{
  TYPE_HEADER;
  TYPE_DECL_HEADER;
};

/** Type for struct types. */
struct typedesc_struct_s
{
  TYPE_HEADER;
  TYPE_DECL_HEADER;
};

/** Type for union types. */
struct typedesc_union_s
{
  TYPE_HEADER;
  TYPE_DECL_HEADER;
};

/** Type for enum types. */
struct typedesc_enum_s
{
  TYPE_HEADER;
  TYPE_DECL_HEADER;
};

/** Type for module types. */
struct typedesc_mod_s
{
  TYPE_HEADER;
  TYPE_DECL_HEADER;
};

#undef TYPE_DECL_HEADER
#undef TYPE_HEADER

/**
 * \brief Initializes a new type.
 * 
 * \param[in] kind Type kind.
 * \param[in] size Size of type in bytes.
 * \returns New type.
*/
typedesc_t* typedesc_init(typedesc_kind_t kind, size_t size);

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

#define typedesc_mut_init()      ((typedesc_mut_t*)   typedesc_init(TYPEDESC_MUT,    sizeof(typedesc_mut_t)))
#define typedesc_const_init()    ((typedesc_const_t*) typedesc_init(TYPEDESC_CONST,  sizeof(typedesc_const_t)))
#define typedesc_ptr_init()      ((typedesc_ptr_t*)   typedesc_init(TYPEDESC_PTR,    sizeof(typedesc_ptr_t)))
#define typedesc_array_init()    ((typedesc_array_t*) typedesc_init(TYPEDESC_ARRAY,  sizeof(typedesc_array_t)))
#define typedesc_ref_init()      ((typedesc_ref_t*)   typedesc_init(TYPEDESC_REF,    sizeof(typedesc_ref_t)))
#define typedesc_opt_init()      ((typedesc_opt_t*)   typedesc_init(TYPEDESC_OPT,    sizeof(typedesc_opt_t)))
#define typedesc_fun_init()      ((typedesc_fun_t*)   typedesc_init(TYPEDESC_FUN,    sizeof(typedesc_fun_t)))
#define typedesc_gen_init()      ((typedesc_gen_t*)   typedesc_init(TYPEDESC_GEN,    sizeof(typedesc_gen_t)))
#define typedesc_type_init()     ((typedesc_t*)       typedesc_init(TYPEDESC_TYPE,   sizeof(typedesc_t)))
#define typedesc_i8_init()       ((typedesc_t*)       typedesc_init(TYPEDESC_I8,     sizeof(typedesc_t)))
#define typedesc_i16_init()      ((typedesc_t*)       typedesc_init(TYPEDESC_I16,    sizeof(typedesc_t)))
#define typedesc_i32_init()      ((typedesc_t*)       typedesc_init(TYPEDESC_I32,    sizeof(typedesc_t)))
#define typedesc_i64_init()      ((typedesc_t*)       typedesc_init(TYPEDESC_I64,    sizeof(typedesc_t)))
#define typedesc_isize_init()    ((typedesc_t*)       typedesc_init(TYPEDESC_ISIZE,  sizeof(typedesc_t)))
#define typedesc_u8_init()       ((typedesc_t*)       typedesc_init(TYPEDESC_U8,     sizeof(typedesc_t)))
#define typedesc_u16_init()      ((typedesc_t*)       typedesc_init(TYPEDESC_U16,    sizeof(typedesc_t)))
#define typedesc_u32_init()      ((typedesc_t*)       typedesc_init(TYPEDESC_U32,    sizeof(typedesc_t)))
#define typedesc_u64_init()      ((typedesc_t*)       typedesc_init(TYPEDESC_U64,    sizeof(typedesc_t)))
#define typedesc_usize_init()    ((typedesc_t*)       typedesc_init(TYPEDESC_USIZE,  sizeof(typedesc_t)))
#define typedesc_f32_init()      ((typedesc_t*)       typedesc_init(TYPEDESC_F32,    sizeof(typedesc_t)))
#define typedesc_f64_init()      ((typedesc_t*)       typedesc_init(TYPEDESC_F64,    sizeof(typedesc_t)))
#define typedesc_bool_init()     ((typedesc_t*)       typedesc_init(TYPEDESC_BOOL,   sizeof(typedesc_t)))
#define typedesc_unit_init()     ((typedesc_t*)       typedesc_init(TYPEDESC_UNIT,   sizeof(typedesc_t)))
#define typedesc_null_init()     ((typedesc_t*)       typedesc_init(TYPEDESC_NULL,   sizeof(typedesc_t)))
#define typedesc_struct_init()   ((typedesc_struct_t*)typedesc_init(TYPEDESC_STRUCT, sizeof(typedesc_struct_t)))
#define typedesc_union_init()    ((typedesc_union_t*) typedesc_init(TYPEDESC_UNION,  sizeof(typedesc_union_t)))
#define typedesc_enum_init()     ((typedesc_enum_t*)  typedesc_init(TYPEDESC_ENUM,   sizeof(typedesc_enum_t)))
#define typedesc_mod_init()      ((typedesc_mod_t*)   typedesc_init(TYPEDESC_MOD,    sizeof(typedesc_mod_t)))

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
 * \param[in] type Type to be pointed to.
 * \returns Pointer type.
*/
typedesc_t* typedesc_make_ptr(typedesc_t* type);

/**
 * \brief Adds a reference to a type.
 * 
 * \param[in] type Type to be referenced.
 * \returns Reference type.
*/
typedesc_t* typedesc_make_ref(typedesc_t* type);

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
 * \brief Returns the type of an AST node.
 * 
 * \param[in] node Node whose type is to be created.
 * \returns Type of AST node.
*/
typedesc_t* typedesc_of(ast_node_t* node);

/**
 * \brief Prints a human-readable representation of a type into a stream.
 * 
 * \param[in] stream Stream to be printed to.
 * \param[in] type Type to be printed.
 * \returns Number of characters printed.
*/
size_t typedesc_print(FILE* stream, typedesc_t* type);

#endif