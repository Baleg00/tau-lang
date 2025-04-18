/**
 * \file
 * 
 * \brief Compiler context.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_COMPILER_H
#define TAU_COMPILER_H

#include "compiler/environment.h"
#include "utils/extern_c.h"
#include "utils/io/path.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Represents a compiler context.
 */
typedef struct compiler_t compiler_t;

/**
 * \brief Initializes a new compiler context.
 * 
 * \returns Pointer to the newly initialized compiler context.
 */
compiler_t* compiler_init(void);

/**
 * \brief Frees all resources associated with a compiler context.
 * 
 * \param[in] compiler Pointer to the compiler context to be freed.
 */
void compiler_free(compiler_t* compiler);

/**
 * \brief Executes a compilation process using a compiler context and commandline
 * arguments.
 * 
 * \param[in] compiler Pointer to the compiler context to be used.
 * \param[in] argc Commandline argument count.
 * \param[in] argv Commandline argument array.
 * \returns `0` on success, otherwise a non-zero value.
 */
int compiler_main(compiler_t* compiler, int argc, const char* argv[]);

/**
 * \brief Processes a source file.
 *
 * \param[in,out] compiler Pointer to the compiler context to be used.
 * \param[in] path Pointer to the path of the source file.
 */
environment_t* compiler_process_file(compiler_t* compiler, path_t* path);

TAU_EXTERN_C_END

#endif
