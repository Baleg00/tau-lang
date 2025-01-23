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
typedef struct nameres_ctx_t
{
  symtable_t* global_scope; ///< The symbol table of the global scope.
  stack_t* scopes; ///< Current scope stack.
  error_bag_t* errors; ///< Associated error bag to add errors to.
} nameres_ctx_t;

/**
 * \brief Initializes a new name resolution context.
 *
 * \param[in] symtable Pointer to the symbol table to be used.
 * \param[in] errors Pointer to the error bag to be used.
 * \returns Pointer to the newly initialized name resolution context.
 */
nameres_ctx_t* nameres_ctx_init(symtable_t* symtable, error_bag_t* errors);

/**
 * \brief Frees all memory allocated by a name resolution context.
 * 
 * \param[in] ctx Pointer to the name resolution context to be freed.
 */
void nameres_ctx_free(nameres_ctx_t* ctx);

/**
 * \brief Begins a new scope and returns it.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \returns Pointer to the new scope.
 */
symtable_t* nameres_ctx_scope_begin(nameres_ctx_t* ctx);

/**
 * \brief Ends the current scope.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 */
void nameres_ctx_scope_end(nameres_ctx_t* ctx);

/**
 * \brief Returns the current scope.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \returns Pointer to the current scope.
 */
symtable_t* nameres_ctx_scope_cur(nameres_ctx_t* ctx);

TAU_EXTERN_C_END

#endif
