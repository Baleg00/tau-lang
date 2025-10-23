/**
 * \file
 * 
 * \brief Name resolution pass.
 * 
 * \details The name resolution pass is a stage in the compilation process where
 * identifiers (names) within the source code are associated with their corresponding
 * declarations or definitions. This pass resolves references to variables,
 * functions, or other symbols, linking each usage to the appropriate declaration
 * in the program scope. Name resolution lays the foundation for subsequent
 * compilation phases, enabling type checking, semantic analysis, and code
 * generation.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_NAMERES_H
#define TAU_NAMERES_H

#include <utils/error.h>

#include "stages/analysis/symtable.h"
#include "utils/common.h"
#include "utils/collections/stack.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Name resolution context.
 */
typedef struct tau_nameres_ctx_t
{
  tau_symtable_t* global_scope; ///< The symbol table of the global scope.
  tau_stack_t* scopes; ///< Current scope stack.
  tau_error_bag_t* errors; ///< Associated error bag to add errors to.
} tau_nameres_ctx_t;

/**
 * \brief Initializes a new name resolution context.
 *
 * \param[in] symtable Pointer to the symbol table to be used.
 * \param[in] errors Pointer to the error bag to be used.
 * \returns Pointer to the newly initialized name resolution context.
 */
tau_nameres_ctx_t* tau_nameres_ctx_init(tau_symtable_t* symtable, tau_error_bag_t* errors);

/**
 * \brief Frees all memory allocated by a name resolution context.
 * 
 * \param[in] ctx Pointer to the name resolution context to be freed.
 */
void tau_nameres_ctx_free(tau_nameres_ctx_t* ctx);

/**
 * \brief Begins a new scope and returns it.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \returns Pointer to the new scope.
 */
tau_symtable_t* tau_nameres_ctx_scope_begin(tau_nameres_ctx_t* ctx);

/**
 * \brief Ends the current scope.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 */
void tau_nameres_ctx_scope_end(tau_nameres_ctx_t* ctx);

/**
 * \brief Returns the current scope.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \returns Pointer to the current scope.
 */
tau_symtable_t* tau_nameres_ctx_scope_cur(tau_nameres_ctx_t* ctx);

TAU_EXTERN_C_END

#endif
