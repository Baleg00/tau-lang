/**
 * \file
 * 
 * \brief Type check pass.
 * 
 * \details The type check pass is a stage during the compilation process where
 * the types of expressions, variables, and other language constructs are validated
 * for consistency and adherence to the language's type system. This pass ensures
 * that the program follows the rules of the language with respect to type
 * compatibility, assignment, and function invocation, preventing type-related
 * errors at runtime.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPECHECK_H
#define TAU_TYPECHECK_H

#include "stages/analysis/types/typebuilder.h"
#include "stages/analysis/types/typetable.h"
#include "utils/error.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Type check context.
 */
typedef struct typecheck_ctx_t
{
  typebuilder_t* typebuilder;
  typetable_t* typetable;

  typedesc_fun_t* fun_desc; ///< Type descriptor of the containing function declaration.
  typedesc_enum_t* enum_desc; ///< Type descriptor of the containing enum declaration.

  error_bag_t* errors; ///< Associated error bag to add errors to.
} typecheck_ctx_t;

/**
 * \brief Initializes a new type check context.
 * 
 * \param[in,out] typebuilder The type builder to be used.
 * \param[in,out] typetable The type table target data layout to be used.
 * \param[in] errors Pointer to the error bag to be used.
 * \returns Pointer to the newly initialized type check context.
 */
typecheck_ctx_t* typecheck_ctx_init(typebuilder_t* typebuilder, typetable_t* typetable, error_bag_t* errors);

/**
 * \brief Frees all memory allocated by a type check context.
 * 
 * \param[in] ctx Pointer to the type check context to be freed.
 */
void typecheck_ctx_free(typecheck_ctx_t* ctx);

/**
 * \brief Poisons an AST node in the typetable.
 *
 * \param[in] ctx Pointer to the type check context to be used.
 * \param[in] node Pointer to the AST node to be poisoned.
 */
void typecheck_poison(typecheck_ctx_t* ctx, ast_node_t* node);

TAU_EXTERN_C_END

#endif
