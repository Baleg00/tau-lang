/**
 * \file
 *
 * \brief Abstract linker interface.
 *
 * \details The linker provides an abstract interface for managing the linking process, abstracting away the differences
 * between various linkers such as GCC, LLD, and MSVC. It allows for the combination of object files into executables or
 * libraries, manages dependencies on external libraries, and handles common tasks like setting optimization levels,
 * specifying output types etc. By using this abstraction, the compiler can seamlessly switch between different
 * underlying linkers without altering the core linking logic.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_LINKER_H
#define TAU_LINKER_H

#include "utils/common.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Enumeration of linker implementation kinds.
 */
typedef enum tau_linker_kind_t
{
  TAU_LINKER_GCC, ///< GNU Compiler Collection
  TAU_LINKER_MSVC ///< MSVC Linker
} tau_linker_kind_t;

/**
 * \brief Represents an abstract linker.
 */
typedef struct tau_linker_t
{
  tau_linker_kind_t kind; ///< The linker's implementation kind.
  void* data;         ///< Pointer to additional implementation specific data.
} tau_linker_t;

/**
 * \brief Enumeration of linker output kinds.
 */
typedef enum tau_linker_output_kind_t
{
  TAU_LINKER_OUTPUT_DYNAMIC_NONPIE,       ///< Generates a dynamically linked non position-independent executable file.
  TAU_LINKER_OUTPUT_DYNAMIC_PIE,          ///< Generates a dynamically linked position-independent executable file.
  TAU_LINKER_OUTPUT_STATIC_NONPIE,        ///< Generates a statically linked non position-independent executable file.
  TAU_LINKER_OUTPUT_STATIC_PIE,           ///< Generates a statically linked position-independent executable file.
  TAU_LINKER_OUTPUT_DYNAMIC_LIBRARY,      ///< Generates a dynamic library file.
  TAU_LINKER_OUTPUT_DYNAMIC_LIBRARY_LIBC, ///< Generates a dynamic library file with bundled libc.
} tau_linker_output_kind_t;

/**
 * \brief Enumeration of optimization level kinds.
 */
typedef enum tau_linker_optimization_level_t
{
  TAU_LINKER_OPTIMIZATION_NONE,       ///< No optimization.
  TAU_LINKER_OPTIMIZATION_LESS,       ///< Basic optimization.
  TAU_LINKER_OPTIMIZATION_DEFAULT,    ///< Moderate optimization.
  TAU_LINKER_OPTIMIZATION_AGGRESSIVE, ///< Aggressive optimization.
  TAU_LINKER_OPTIMIZATION_SIZE,       ///< Optimize for minimum binary size.
  TAU_LINKER_OPTIMIZATION_SPEED,      ///< Optimize for maximum runtime performance.
  TAU_LINKER_OPTIMIZATION_DEBUG       ///< Optimize for debugging.
} tau_linker_optimization_level_t;

/**
 * \brief Enumeration of symbol visibility kinds.
 */
typedef enum tau_linker_visibility_t
{
  TAU_LINKER_VISIBILITY_DEFAULT,  ///< Symbols are visible to other binaries by default.
  TAU_LINKER_VISIBILITY_HIDDEN,   ///< Symbols are not visible to other binaries.
  TAU_LINKER_VISIBILITY_PROTECTED ///< Symbols are visible to other binaries but cannot be overridden by other binaries.
} tau_linker_visibility_t;

/**
 * \brief Initializes a new linker of a specified kind.
 *
 * \param [in] kind The linker kind.
 * \returns Pointer to the newly initialized linker.
 */
tau_linker_t* tau_linker_init(tau_linker_kind_t kind);

/**
 * \brief Frees all allocated memory associated with a linker.
 *
 * \param[in] linker Pointer to the linker to be freed.
 */
void tau_linker_free(tau_linker_t* linker);

/**
 * \brief Adds an object file to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] path Path to the object file.
 */
void tau_linker_add_object(tau_linker_t* linker, const char* path);

/**
 * \brief Adds a directory to be searched for libraries to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] path Path to the directory.
 */
void tau_linker_add_library_directory(tau_linker_t* linker, const char* path);

/**
 * \brief Adds a static library given its name to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] name Name of the static library.
 */
void tau_linker_add_static_library_by_name(tau_linker_t* linker, const char* name);

/**
 * \brief Adds a static library given its path to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] path Path to the static library.
 */
void tau_linker_add_static_library_by_path(tau_linker_t* linker, const char* path);

/**
 * \brief Adds a dynamic library given its name to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] name Name of the dynamic library.
 */
void tau_linker_add_dynamic_library_by_name(tau_linker_t* linker, const char* name);

/**
 * \brief Adds a dynamic library given its path to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] path Path to the dynamic library.
 */
void tau_linker_add_dynamic_library_by_path(tau_linker_t* linker, const char* path);

/**
 * \brief Sets the output kind of the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] kind The output kind.
 */
void tau_linker_set_output_kind(tau_linker_t* linker, tau_linker_output_kind_t kind);

/**
 * \brief Sets the output file of the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] file Path to the output file.
 */
void tau_linker_set_output_file(tau_linker_t* linker, const char* file);

/**
 * \brief Sets the optimization level of the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] level The optimization level.
 */
void tau_linker_set_optimization_level(tau_linker_t* linker, tau_linker_optimization_level_t level);

/**
 * \brief Enables or disables the generation of debugging information for the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] debugging `true` to enable debugging, `false` to disable debugging.
 */
void tau_linker_set_debugging(tau_linker_t* linker, bool debugging);

/**
 * \brief Sets the entry point of the generated file for the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] entry The name of the entry point.
 */
void tau_linker_set_entry_point(tau_linker_t* linker, const char* entry);

/**
 * \brief Sets the symbol visibility for the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] visibility The symbol visibility kind.
 */
void tau_linker_set_visibility(tau_linker_t* linker, tau_linker_visibility_t visibility);

/**
 * \brief Adds a custom flag to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] flag The custom flag.
 */
void tau_linker_add_flag(tau_linker_t* linker, const char* flag);

/**
 * \brief Executes the linking process.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \returns `true` if linking was successful, `false` otherwise.
 */
bool tau_linker_link(tau_linker_t* linker);

TAU_EXTERN_C_END

#endif
