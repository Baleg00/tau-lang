/**
 * \file
 * 
 * \brief Code generation pass.
 * 
 * \details 
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_CODEGEN_H
#define TAU_CODEGEN_H

#include "llvm.h"
#include "stages/analysis/types/types.h"

TAU_EXTERN_C_BEGIN

/**
 * \see ast/decl/fun.h
 */
typedef struct ast_decl_fun_t ast_decl_fun_t;

/**
 * \brief Code generation context.
 */
typedef struct codegen_ctx_t
{
  typebuilder_t* typebuilder;    ///< Pointer to the associated type builder.
  typetable_t* typetable;        ///< Pointer to the associated type table.
  ast_decl_fun_t* fun_node;      ///< Pointer to the current AST function declaration being visited.
  size_t param_idx;              ///< Parameter index in the current function being visited.
  size_t enum_idx;               ///< Enum constant index in the current enum being visited.

  LLVMContextRef llvm_ctx;       ///< Reference to the associated LLVM context.
  LLVMTargetDataRef llvm_layout; ///< Reference to the associated LLVM target data layout.
  LLVMModuleRef llvm_mod;        ///< Reference to the associated LLVM module.
  LLVMBuilderRef llvm_builder;   ///< Reference to the associated LLVM IR builder.
} codegen_ctx_t;

/**
 * \brief Initializes a new code generation context.
 *
 * \param[in] typebuilder The type builder to be used.
 * \param[in] typetable The type table to be used.
 * \param[in] llvm_ctx The LLVM context to be used.
 * \param[in] llvm_layout The LLVM target data layout to be used.
 * \param[in] llvm_mod The LLVM module to be used.
 * \param[in] llvm_builder The LLVM builder to be used.
 * \returns Pointer to the newly initialized code generation context.
 */
codegen_ctx_t* codegen_ctx_init(typebuilder_t* typebuilder, typetable_t* typetable, LLVMContextRef llvm_ctx, LLVMTargetDataRef llvm_layout, LLVMModuleRef llvm_mod, LLVMBuilderRef llvm_builder);

/**
 * \brief Frees all memory allocated by a code generation context.
 * 
 * \param[in] ctx Pointer to the code generation context to be freed.
 */
void codegen_ctx_free(codegen_ctx_t* ctx);

/**
 * \brief Builds an LLVM `load` instruction if the value has a reference type.
 *
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] llvm_value The LLVM value reference.
 * \param[in] desc Pointer to the type descriptor of the value.
 * \returns The result of the `load` instruction if `node` has a reference type,
 * otherwise the `llvm_value` of `node`.
 */
LLVMValueRef codegen_build_load_if_ref(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* desc);

/**
 * \brief Builds an LLVM cast instruction to perform an arithmetic cast.
 *
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] llvm_value The LLVM value reference.
 * \param[in] src_desc Pointer to the source type descriptor.
 * \param[in] dst_desc Pointer to the target type descriptor.
 * \returns The result of the cast instruction.
 */
LLVMValueRef codegen_build_arithmetic_cast(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Builds an LLVM cast instruction to perform a vector cast.
 *
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] llvm_value The LLVM value reference.
 * \param[in] src_desc Pointer to the source type descriptor.
 * \param[in] dst_desc Pointer to the target type descriptor.
 * \returns The result of the cast instruction.
 */
LLVMValueRef codegen_build_vector_cast(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Builds an LLVM cast instruction to perform a matrix cast.
 *
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] llvm_value The LLVM value reference.
 * \param[in] src_desc Pointer to the source type descriptor.
 * \param[in] dst_desc Pointer to the target type descriptor.
 * \returns The result of the cast instruction.
 */
LLVMValueRef codegen_build_matrix_cast(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Builds an LLVM instruction to wrap a value into an optional.
 *
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] llvm_value The LLVM value reference to be wrapped.
 * \param[in] src_desc Pointer to the type descriptor of value to be wrapped.
 * \param[in] dst_desc Pointer to the type descriptor of the optional.
 * \returns The wrapped LLVM value reference.
 */
LLVMValueRef codegen_build_opt_wrap(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Builds an LLVM instruction to unwrap a value from an optional without
 * checking if the optional is `null`.
 *
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] llvm_value The LLVM value reference to be unwrapped.
 * \param[in] desc Pointer to the type descriptor of the optional.
 * \returns The unwrapped LLVM value reference.
 */
LLVMValueRef codegen_build_opt_unwrap_unchecked(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* desc);

/**
 * \brief Builds an LLVM cast instruction to perform an implicit cast.
 *
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] llvm_value The LLVM value reference.
 * \param[in] src_desc Pointer to the source type descriptor.
 * \param[in] dst_desc Pointer to the target type descriptor.
 * \returns The result of the cast instruction if it is required, otherwise
 * `llvm_value`.
 */
LLVMValueRef codegen_build_implicit_cast(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* src_desc, typedesc_t* dst_desc);

/**
 * \brief Builds an LLVM instruction to create a complex floating-point number.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] llvm_value_real The LLVM value reference of the real argument.
 * \param[in] llvm_value_imag The LLVM value reference of the imaginary argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_complex(codegen_ctx_t* ctx, LLVMValueRef llvm_value_real, LLVMValueRef llvm_value_imag);

/**
 * \brief Builds an LLVM instruction to negate a complex floating-point number.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] llvm_value The LLVM value reference of the argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_complex_neg(codegen_ctx_t* ctx, LLVMValueRef llvm_value);

/**
 * \brief Builds an LLVM instruction to add two complex floating-point numbers.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] llvm_lhs The LLVM value reference of the left-hand argument.
 * \param[in] llvm_rhs The LLVM value reference of the right-hand argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_complex_add(codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs);

/**
 * \brief Builds an LLVM instruction to subtract two complex floating-point numbers.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] llvm_lhs The LLVM value reference of the left-hand argument.
 * \param[in] llvm_rhs The LLVM value reference of the right-hand argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_complex_sub(codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs);

/**
 * \brief Builds an LLVM instruction to multiply two complex floating-point numbers.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] llvm_lhs The LLVM value reference of the left-hand argument.
 * \param[in] llvm_rhs The LLVM value reference of the right-hand argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_complex_mul(codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs);

/**
 * \brief Builds an LLVM instruction to divide two complex floating-point numbers.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] llvm_lhs The LLVM value reference of the left-hand argument.
 * \param[in] llvm_rhs The LLVM value reference of the right-hand argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_complex_div(codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs);

/**
 * \brief Builds an LLVM instruction to compare two complex floating-point numbers for equality.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] llvm_lhs The LLVM value reference of the left-hand argument.
 * \param[in] llvm_rhs The LLVM value reference of the right-hand argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_complex_eq(codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs);

/**
 * \brief Builds an LLVM instruction to compare two complex floating-point numbers for inequality.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] llvm_lhs The LLVM value reference of the left-hand argument.
 * \param[in] llvm_rhs The LLVM value reference of the right-hand argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_complex_ne(codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs);

/**
 * \brief Builds an LLVM instruction to add two vectors.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] desc Pointer to the type descriptor of the vectors.
 * \param[in] llvm_lhs The LLVM value reference of the left-hand argument.
 * \param[in] llvm_rhs The LLVM value reference of the right-hand argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_vector_add(codegen_ctx_t* ctx, typedesc_t* desc, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs);

/**
 * \brief Builds an LLVM instruction to subtract two vectors.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] desc Pointer to the type descriptor of the vectors.
 * \param[in] llvm_lhs The LLVM value reference of the left-hand argument.
 * \param[in] llvm_rhs The LLVM value reference of the right-hand argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_vector_sub(codegen_ctx_t* ctx, typedesc_t* desc, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs);

/**
 * \brief Builds an LLVM instruction to multiply a vector by a scalar.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] desc Pointer to the type descriptor of the vectors.
 * \param[in] llvm_vec The LLVM value reference of the vector.
 * \param[in] llvm_scalar The LLVM value reference of the scalar.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_vector_mul(codegen_ctx_t* ctx, typedesc_t* desc, LLVMValueRef llvm_vec, LLVMValueRef llvm_scalar);

/**
 * \brief Builds an LLVM instruction to compare two vectors for equality.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] desc Pointer to the type descriptor of the vectors.
 * \param[in] llvm_lhs The LLVM value reference of the left-hand argument.
 * \param[in] llvm_rhs The LLVM value reference of the right-hand argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_vector_eq(codegen_ctx_t* ctx, typedesc_t* desc, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs);

/**
 * \brief Builds an LLVM instruction to compare two vectors for inequality.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] desc Pointer to the type descriptor of the vectors.
 * \param[in] llvm_lhs The LLVM value reference of the left-hand argument.
 * \param[in] llvm_rhs The LLVM value reference of the right-hand argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_vector_ne(codegen_ctx_t* ctx, typedesc_t* desc, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs);

/**
 * \brief Builds an LLVM instruction to add two matrices.
 *
 * \param[in] ctx Pointer to the code generation context to be used.
 * \param[in] desc Pointer to the type descriptor of the matrices.
 * \param[in] llvm_lhs The LLVM value reference of the left-hand argument.
 * \param[in] llvm_rhs The LLVM value reference of the right-hand argument.
 * \returns The result of the instruction.
 */
LLVMValueRef codegen_build_matrix_add(codegen_ctx_t* ctx, typedesc_t* desc, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs);

TAU_EXTERN_C_END

#endif
