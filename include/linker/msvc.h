/**
 * \file
 *
 * \brief MSVC linker implementation.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_LINKER_MSVC_H
#define TAU_LINKER_MSVC_H

#include "linker/linker.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Initializes a new MSVC linker.
 *
 * \returns Pointer to the newly initialized MSVC linker.
 */
tau_linker_t* tau_linker_msvc_init(void);

/**
 * \brief Frees all allocated memory associated with a linker.
 *
 * \param[in] linker Pointer to the linker to be freed.
 */
void tau_linker_msvc_free(tau_linker_t* linker);

/**
 * \brief Adds an object file to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] path Path to the object file.
 */
void tau_linker_msvc_add_object(tau_linker_t* linker, const char* path);

/**
 * \brief Adds a directory to be searched for libraries to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] path Path to the directory.
 */
void tau_linker_msvc_add_library_directory(tau_linker_t* linker, const char* path);

/**
 * \brief Adds a static library given its name to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] name Name of the static library.
 */
void tau_linker_msvc_add_static_library_by_name(tau_linker_t* linker, const char* name);

/**
 * \brief Adds a static library given its path to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] path Path to the static library.
 */
void tau_linker_msvc_add_static_library_by_path(tau_linker_t* linker, const char* path);

/**
 * \brief Adds a dynamic library given its name to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] name Name of the dynamic library.
 */
void tau_linker_msvc_add_dynamic_library_by_name(tau_linker_t* linker, const char* name);

/**
 * \brief Adds a dynamic library given its path to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] path Path to the dynamic library.
 */
void tau_linker_msvc_add_dynamic_library_by_path(tau_linker_t* linker, const char* path);

/**
 * \brief Sets the output kind of the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] kind The output kind.
 */
void tau_linker_msvc_set_output_kind(tau_linker_t* linker, tau_linker_output_kind_t kind);

/**
 * \brief Sets the output file of the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] file Path to the output file.
 */
void tau_linker_msvc_set_output_file(tau_linker_t* linker, const char* file);

/**
 * \brief Sets the optimization level of the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] level The optimization level.
 */
void tau_linker_msvc_set_optimization_level(tau_linker_t* linker, tau_linker_optimization_level_t level);

/**
 * \brief Enables or disables the generation of debugging information for the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] debugging `true` to enable debugging, `false` to disable debugging.
 */
void tau_linker_msvc_set_debugging(tau_linker_t* linker, bool debugging);

/**
 * \brief Sets the entry point of the generated file for the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] entry The name of the entry point.
 */
void tau_linker_msvc_set_entry_point(tau_linker_t* linker, const char* entry);

/**
 * \brief Sets the symbol visibility for the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] visibility The symbol visibility kind.
 */
void tau_linker_msvc_set_visibility(tau_linker_t* linker, tau_linker_visibility_t visibility);

/**
 * \brief Adds a custom flag to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] flag The custom flag.
 */
void tau_linker_msvc_add_flag(tau_linker_t* linker, const char* flag);

/**
 * \brief Executes the linking process.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \returns `true` if linking was successful, `false` otherwise.
 */
bool tau_linker_msvc_link(tau_linker_t* linker);

TAU_EXTERN_C_END

#endif
