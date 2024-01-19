/**
 * \file nameres.h
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

#include "stages/analyzer/symtable.h"

/**
 * \brief Name resolution context type.
 */
typedef struct nameres_ctx_t
{
  symtable_t* scope; // The parent scope of the node being visited.
} nameres_ctx_t;

/**
 * \brief Initializes a new name resolution context.
 * 
 * \returns Pointer to the newly initialized name resolution context.
 */
nameres_ctx_t* nameres_ctx_init(void);

/**
 * \brief Frees all memory allocated by a name resolution context.
 * 
 * \param[in] ctx Pointer to the name resolution context to be freed.
 */
void nameres_ctx_free(nameres_ctx_t* ctx);

#endif