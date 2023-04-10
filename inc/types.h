/**
 * \file
 * 
 * Type library for type-checking.
*/

#ifndef TAU_TYPES_H
#define TAU_TYPES_H

#include <stdbool.h>
#include <stdio.h>

#include "list.h"
#include "ast.h"
#include "op.h"

/** Indicates the category of a type. */
typedef enum type_flag_e
{
  TYPE_FLAG_MODIFIER   = 0x010000, // Modifier (pointer, reference etc.)
  TYPE_FLAG_BUILTIN    = 0x020000, // Built-in (i32, unit etc.)
  TYPE_FLAG_INTEGER    = 0x040000, // Integer (i32, u32 etc.)
  TYPE_FLAG_FLOAT      = 0x080000, // Float (f32, f64)
  TYPE_FLAG_ARITHMETIC = 0x100000, // Arithmetic (i32, f32 etc.)
  TYPE_FLAG_SIGNED     = 0x200000, // Signed (i8, i16 etc.)
  TYPE_FLAG_UNSIGNED   = 0x400000, // Unsigned (u8, u16 etc.)
  TYPE_FLAG_OWNER      = 0x800000, // Owner (struct, union etc.)
} type_flag_t;

/** Indicates the kind of a type. */
typedef enum type_kind_e
{
  TYPE_FUN, // Function type
  TYPE_GEN, // Generator type
  TYPE_TYPE, // Type of type
  TYPE_MUT = TYPE_FLAG_MODIFIER, // Mutable type
  TYPE_CONST, // Compile-time type
  TYPE_PTR, // Pointer type
  TYPE_ARRAY, // Array type
  TYPE_REF, // Reference type
  TYPE_OPT, // Optional type
  TYPE_I8 = TYPE_FLAG_INTEGER | TYPE_FLAG_ARITHMETIC | TYPE_FLAG_SIGNED | TYPE_FLAG_BUILTIN, // Built-in type i8
  TYPE_I16, // Built-in type i16
  TYPE_I32, // Built-in type i32
  TYPE_I64, // Built-in type i64
  TYPE_ISIZE, // Built-in type isize
  TYPE_U8 = TYPE_FLAG_INTEGER | TYPE_FLAG_ARITHMETIC | TYPE_FLAG_UNSIGNED | TYPE_FLAG_BUILTIN, // Built-in type u8
  TYPE_U16, // Built-in type u16
  TYPE_U32, // Built-in type u32
  TYPE_U64, // Built-in type u64
  TYPE_USIZE, // Built-in type usize
  TYPE_F32 = TYPE_FLAG_FLOAT | TYPE_FLAG_ARITHMETIC | TYPE_FLAG_SIGNED | TYPE_FLAG_BUILTIN, // Built-in type f32
  TYPE_F64, // Built-in type f64
  TYPE_BOOL = TYPE_FLAG_BUILTIN, // Built-in type bool
  TYPE_UNIT, // Built-in type unit
  TYPE_NULL, // Null type
  TYPE_STRUCT = TYPE_FLAG_OWNER, // Struct type
  TYPE_UNION, // Union type
  TYPE_ENUM, // Enum type
  TYPE_MOD, // Module type
} type_kind_t;

/** Utility macro which expands to fields that all types must have. */
#define TYPE_HEADER\
  struct\
  {\
    type_kind_t kind; /** Type kind. */\
  }\

/** Utility macro which expands to fields that all declared types must have. */
#define TYPE_DECL_HEADER\
  struct\
  {\
    ast_decl_t* node; /** Declaration node. */\
  }\

/** Base type for all types. */
typedef struct type_s
{
  TYPE_HEADER;
} type_t;

/** Type for mutable types. */
typedef struct type_mut_s
{
  TYPE_HEADER;
  type_t* base_type; // Underlying type.
} type_mut_t;

/** Type for compile-time types. */
typedef struct type_const_s
{
  TYPE_HEADER;
  type_t* base_type; // Underlying type.
} type_const_t;

/** Type for pointer types. */
typedef struct type_ptr_s
{
  TYPE_HEADER;
  type_t* base_type; // Underlying type.
} type_ptr_t;

/** Type for array types. */
typedef struct type_array_s
{
  TYPE_HEADER;
  type_t* base_type; // Underlying type.
} type_array_t;

/** Type for reference types. */
typedef struct type_ref_s
{
  TYPE_HEADER;
  type_t* base_type; // Underlying type.
} type_ref_t;

/** Type for optional types. */
typedef struct type_opt_s
{
  TYPE_HEADER;
  type_t* base_type; // Underlying type.
} type_opt_t;

/** Type for function types. */
typedef struct type_fun_s
{
  TYPE_HEADER;
  list_t* param_types; // Parameter types.
  type_t* return_type; // Return type.
} type_fun_t;

/** Type for generator types. */
typedef struct type_gen_s
{
  TYPE_HEADER;
  list_t* param_types; // List of parameter types.
  type_t* yield_type; // Yield type.
} type_gen_t;

/** Base type for all declared types. */
typedef struct type_decl_s
{
  TYPE_HEADER;
  TYPE_DECL_HEADER;
} type_decl_t;

/** Type for struct types. */
typedef struct type_struct_s
{
  TYPE_HEADER;
  TYPE_DECL_HEADER;
} type_struct_t;

/** Type for union types. */
typedef struct type_union_s
{
  TYPE_HEADER;
  TYPE_DECL_HEADER;
} type_union_t;

/** Type for enum types. */
typedef struct type_enum_s
{
  TYPE_HEADER;
  TYPE_DECL_HEADER;
} type_enum_t;

/** Type for module types. */
typedef struct type_mod_s
{
  TYPE_HEADER;
  TYPE_DECL_HEADER;
} type_mod_t;

#undef TYPE_DECL_HEADER
#undef TYPE_HEADER

/**
 * \brief Initializes a new type.
 * 
 * \param[in] kind Type kind.
 * \param[in] size Size of type in bytes.
 * \returns New type.
*/
type_t* type_init(type_kind_t kind, size_t size);

/**
 * \brief Returns a built-in type.
 * 
 * \details Built-in types don't need to be allocated every time they are used.
 * Built-in type instances can be retrieved using this function.
 * 
 * \param[in] kind Built-in type kind.
 * \returns Built-in type.
*/
type_t* type_builtin(type_kind_t kind);

#define type_mut_init()    ((type_mut_t*)   type_init(TYPE_MUT,    sizeof(type_mut_t)))
#define type_const_init()  ((type_const_t*) type_init(TYPE_CONST,  sizeof(type_const_t)))
#define type_ptr_init()    ((type_ptr_t*)   type_init(TYPE_PTR,    sizeof(type_ptr_t)))
#define type_array_init()  ((type_array_t*) type_init(TYPE_ARRAY,  sizeof(type_array_t)))
#define type_ref_init()    ((type_ref_t*)   type_init(TYPE_REF,    sizeof(type_ref_t)))
#define type_opt_init()    ((type_opt_t*)   type_init(TYPE_OPT,    sizeof(type_opt_t)))
#define type_fun_init()    ((type_fun_t*)   type_init(TYPE_FUN,    sizeof(type_fun_t)))
#define type_gen_init()    ((type_gen_t*)   type_init(TYPE_GEN,    sizeof(type_gen_t)))
#define type_type_init()   ((type_type_t*)  type_init(TYPE_TYPE,   sizeof(type_type_t)))
#define type_i8_init()     ((type_t*)       type_init(TYPE_I8,     sizeof(type_t)))
#define type_i16_init()    ((type_t*)       type_init(TYPE_I16,    sizeof(type_t)))
#define type_i32_init()    ((type_t*)       type_init(TYPE_I32,    sizeof(type_t)))
#define type_i64_init()    ((type_t*)       type_init(TYPE_I64,    sizeof(type_t)))
#define type_isize_init()  ((type_t*)       type_init(TYPE_ISIZE,  sizeof(type_t)))
#define type_u8_init()     ((type_t*)       type_init(TYPE_U8,     sizeof(type_t)))
#define type_u16_init()    ((type_t*)       type_init(TYPE_U16,    sizeof(type_t)))
#define type_u32_init()    ((type_t*)       type_init(TYPE_U32,    sizeof(type_t)))
#define type_u64_init()    ((type_t*)       type_init(TYPE_U64,    sizeof(type_t)))
#define type_usize_init()  ((type_t*)       type_init(TYPE_USIZE,  sizeof(type_t)))
#define type_f32_init()    ((type_t*)       type_init(TYPE_F32,    sizeof(type_t)))
#define type_f64_init()    ((type_t*)       type_init(TYPE_F64,    sizeof(type_t)))
#define type_bool_init()   ((type_t*)       type_init(TYPE_BOOL,   sizeof(type_t)))
#define type_unit_init()   ((type_t*)       type_init(TYPE_UNIT,   sizeof(type_t)))
#define type_null_init()   ((type_t*)       type_init(TYPE_NULL,   sizeof(type_t)))
#define type_struct_init() ((type_struct_t*)type_init(TYPE_STRUCT, sizeof(type_struct_t)))
#define type_union_init()  ((type_union_t*) type_init(TYPE_UNION,  sizeof(type_union_t)))
#define type_enum_init()   ((type_enum_t*)  type_init(TYPE_ENUM,   sizeof(type_enum_t)))
#define type_mod_init()    ((type_mod_t*)   type_init(TYPE_MOD,    sizeof(type_mod_t)))

/**
 * \brief Destroys a type.
 * 
 * \param[in] type Type to be destroyed.
*/
void type_free(type_t* type);

/**
 * \brief Checks if a type is a modifier.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is a modifier, false otherwise.
*/
bool type_is_modifier(type_t* type);

/**
 * \brief Checks if a type is a built-in type.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is built-in, false otherwise.
*/
bool type_is_builtin(type_t* type);

/**
 * \brief Checks if a type is an integer.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is an integer, false otherwise.
*/
bool type_is_integer(type_t* type);

/**
 * \brief Checks if a type is a float.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is a float, false otherwise.
*/
bool type_is_float(type_t* type);

/**
 * \brief Checks if a type is an arithmetic type.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is arithmetic, false otherwise.
*/
bool type_is_arithmetic(type_t* type);

/**
 * \brief Checks if a type is a signed type.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is signed, false otherwise.
*/
bool type_is_signed(type_t* type);

/**
 * \brief Checks if a type is an unsigned type.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is unsigned, false otherwise.
*/
bool type_is_unsigned(type_t* type);

/**
 * \brief Checks if a type is an owner type.
 * 
 * \param[in] type Type to be checked.
 * \returns True if type is an owner, false otherwise.
*/
bool type_is_owner(type_t* type);

/**
 * \brief Checks if two types are the same.
 * 
 * \param[in] lhs Type to be checked.
 * \param[in] rhs Type to be checked.
 * \returns True if the two types are the same, false otherwise.
*/
bool type_is_same(type_t* lhs, type_t* rhs);

/**
 * \brief Checks if a type can be assigned from another.
 * 
 * \param[in] to Type to be assigned to.
 * \param[in] from Type to be assigned from.
 * \returns True if the assignment is valid, false otherwise.
*/
bool type_is_assignable(type_t* to, type_t* from);

/**
 * \brief Converts an unsigned type to its signed version.
 * 
 * \param[in] type Type to be converted.
 * \returns Signed version of type.
*/
type_t* type_make_signed(type_t* type);

/**
 * \brief Adds a pointer to a type.
 * 
 * \param[in] type Type to be pointed to.
 * \returns Pointer type.
*/
type_t* type_make_ptr(type_t* type);

/**
 * \brief Adds a reference to a type.
 * 
 * \param[in] type Type to be referenced.
 * \returns Reference type.
*/
type_t* type_make_ref(type_t* type);

/**
 * \brief Removes a reference from a type.
 * 
 * \param[in] type Type to be modified.
 * \returns Type without reference.
*/
type_t* type_remove_ref(type_t* type);

/**
 * \brief Takes two arithmetic types and returns a type that would be the result
 * of a binary operation (e.g. addition) between them.
 * 
 * \param[in] lhs Left-hand side arithmetic type.
 * \param[in] rhs Right-hand side arithmetic type.
 * \returns Promoted type.
*/
type_t* type_promote(type_t* lhs, type_t* rhs);

/**
 * \brief Returns the type of an AST node.
 * 
 * \param[in] node Node whose type is to be created.
 * \returns Type of AST node.
*/
type_t* type_of(ast_node_t* node);

/**
 * \brief Prints a human-readable representation of a type into a stream.
 * 
 * \param[in] stream Stream to be printed to.
 * \param[in] type Type to be printed.
 * \returns Number of characters printed.
*/
size_t type_print(FILE* stream, type_t* type);

#endif