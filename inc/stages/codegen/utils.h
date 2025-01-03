/**
 * \file
 * 
 * \brief Code generation utility functions.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_CODEGEN_UTILS_H
#define TAU_CODEGEN_UTILS_H

#include "ast/ast.h"
#include "llvm.h"
#include "stages/codegen/codegen.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Builds an LLVM `load` instruction if `node` has a reference type.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the expression node.
 * \returns The result of the `load` instruction if `node` has a reference type,
 * otherwise the `llvm_value` of `node`.
 */
LLVMValueRef codegen_build_load_if_ref(codegen_ctx_t* ctx, ast_expr_t* node);

/**
 * \brief Builds an LLVM cast instruction to perform an arithmetic cast.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] llvm_value The LLVM value reference.
 * \param[in] from_desc Pointer to the source type descriptor.
 * \param[in] to_desc Pointer to the target type descriptor.
 * \returns The result of the cast instruction if it is required, otherwise
 * `llvm_value`.
 */
LLVMValueRef codegen_build_arithmetic_cast(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc);

/**
 * \brief Builds an LLVM instruction to wrap a value into an optional.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] llvm_value The LLVM value reference to be wrapped.
 * \param[in] from_desc Pointer to the type descriptor of value to be wrapped.
 * \param[in] to_desc Pointer to the type descriptor of the optional.
 * \returns The wrapped LLVM value reference.
 */
LLVMValueRef codegen_build_opt_wrap(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_opt_t* to_desc);

/**
 * \brief Builds an LLVM instruction to unwrap a value from an optional without
 * checking if the optional is `null`.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] llvm_value The LLVM value reference to be unwrapped.
 * \param[in] desc Pointer to the type descriptor of the optional.
 * \returns The unwrapped LLVM value reference.
 */
LLVMValueRef codegen_build_opt_unwrap_unchecked(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_opt_t* desc);

/**
 * \brief Builds an LLVM cast instruction to perform an implicit cast.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] llvm_value The LLVM value reference.
 * \param[in] from_desc Pointer to the source type descriptor.
 * \param[in] to_desc Pointer to the target type descriptor.
 * \returns The result of the cast instruction if it is required, otherwise
 * `llvm_value`.
 */
LLVMValueRef codegen_build_implicit_cast(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc);

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

TAU_EXTERN_C_END

#endif
