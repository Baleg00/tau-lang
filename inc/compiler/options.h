/**
 * \file
 *
 * \brief Compiler command-line options interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_OPTIONS_H
#define TAU_OPTIONS_H

#include "linker/linker.h"
#include "utils/collections/vector.h"
#include "utils/io/log.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Represents a compiler option context.
 */
typedef struct options_ctx_t options_ctx_t;

/**
 * \brief Initializes a new compiler option context.
 *
 * \returns Pointer to the newly initialized compiler option context.
 */
options_ctx_t* options_ctx_init(void);

/**
 * \brief Frees all allocated memory associated with a compiler option context.
 *
 * \param[in] ctx Pointer to the compiler option context to be freed.
 */
void options_ctx_free(options_ctx_t* ctx);

/**
 * \brief Parses command-line arguments and stores options on a compiler option context.
 *
 * \param[out] ctx Pointer to the compiler option context to be used.
 * \param[in] argc Number of command-line arguments.
 * \param[in] argv Array of command-line arguments.
 */
void options_parse(options_ctx_t* ctx, int argc, const char* argv[]);

/**
 * \brief Retrieves the output file name.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns The output file name.
 */
const char* options_get_output_file(options_ctx_t* ctx);

/**
 * \brief Retrieves the output kind.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns The output kind.
 */
linker_output_kind_t options_get_output_kind(options_ctx_t* ctx);

/**
 * \brief Retrieves the log level.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns The log level.
 */
log_level_t options_get_log_level(options_ctx_t* ctx);

/**
 * \brief Retrieves the libraries to link against.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns Vector of library names and paths.
 */
vector_t* options_get_link_libraries(options_ctx_t* ctx);

/**
 * \brief Retrieves the library search directories.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns Vector of search directories.
 */
vector_t* options_get_search_directories(options_ctx_t* ctx);

/**
 * \brief Retrieves the input files.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns Vector of input files.
 */
vector_t* options_get_input_files(options_ctx_t* ctx);

/**
 * \brief Retrieves wether to use verbose output or not.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns `true` if verbose output should be used, `false` otherwise.
 */
bool options_get_is_verbose(options_ctx_t* ctx);

/**
 * \brief Retrieves wether to dump tokens or not.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns `true` if tokens should be dumped, `false` otherwise.
 */
bool options_get_dump_tokens(options_ctx_t* ctx);

/**
 * \brief Retrieves wether to dump AST or not.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns `true` if AST should be dumped, `false` otherwise.
 */
bool options_get_dump_ast(options_ctx_t* ctx);

/**
 * \brief Retrieves wether to dump LLVM IR or not.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns `true` if LLVM IR should be dumped, `false` otherwise.
 */
bool options_get_dump_ll(options_ctx_t* ctx);

/**
 * \brief Retrieves wether to dump LLVM bitcode or not.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns `true` if LLVM bitcode should be dumped, `false` otherwise.
 */
bool options_get_dump_bc(options_ctx_t* ctx);

/**
 * \brief Retrieves wether to dump assembly code or not.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns `true` if assembly code should be dumped, `false` otherwise.
 */
bool options_get_dump_asm(options_ctx_t* ctx);

/**
 * \brief Retrieves wether the compiler should exit safely after parsing command-line arguments.
 *
 * \param[in] ctx Pointer to the compiler option context to be used.
 * \returns `true` if the compiler should exit safely, `false` otherwise.
 */
bool options_get_should_exit(options_ctx_t* ctx);

TAU_EXTERN_C_END

#endif
