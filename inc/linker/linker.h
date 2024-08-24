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

#include <stdbool.h>

/**
 * \brief Linker virtual table function pointer type.
 */
typedef void(*linker_vtable_func_t)(void);

/**
 * \brief Enumeration of the abstract linker's virtual table entries.
 */
typedef enum linker_vtable_entry_kind_t
{
 LINKER_VTABLE_FREE,                         ///< \copydoc linker_free
 LINKER_VTABLE_ADD_OBJECT,                   ///< \copydoc linker_add_object
 LINKER_VTABLE_ADD_LIBRARY_DIRECTORY,        ///< \copydoc linker_add_library_directory
 LINKER_VTABLE_ADD_STATIC_LIBRARY_BY_NAME,   ///< \copydoc linker_add_static_library_by_name
 LINKER_VTABLE_ADD_STATIC_LIBRARY_BY_PATH,   ///< \copydoc linker_add_static_library_by_path
 LINKER_VTABLE_ADD_DYNAMIC_LIBRARY_BY_NAME,  ///< \copydoc linker_add_dynamic_library_by_name
 LINKER_VTABLE_ADD_DYNAMIC_LIBRARY_BY_PATH,  ///< \copydoc linker_add_dynamic_library_by_path
 LINKER_VTABLE_SET_OUTPUT_KIND,              ///< \copydoc linker_set_output_kind
 LINKER_VTABLE_SET_OUTPUT_FILE,              ///< \copydoc linker_set_output_file
 LINKER_VTABLE_SET_OPTIMIZATION_LEVEL,       ///< \copydoc linker_set_optimization_level
 LINKER_VTABLE_SET_DEBUGGING,                ///< \copydoc linker_set_debugging
 LINKER_VTABLE_SET_ENTRY_POINT,              ///< \copydoc linker_set_entry_point
 LINKER_VTABLE_SET_VISIBILITY,               ///< \copydoc linker_set_visibility
 LINKER_VTABLE_ADD_FLAG,                     ///< \copydoc linker_add_flag
 LINKER_VTABLE_LINK,                         ///< \copydoc linker_link

 LINKER_VTABLE_SIZE                          ///< Total number of vtable entries.
} linker_vtable_entry_kind_t;

/**
 * \brief Represents an abstract linker.
 */
typedef struct linker_t
{
  linker_vtable_func_t* vtable; ///< Pointer to the linker implementation's virtual table.
  void* data;                   ///< Pointer to additional implementation specific data.
} linker_t;

/**
 * \brief Enumeration of linker implementation kinds.
 */
typedef enum linker_kind_t
{
  LINKER_GCC ///< GNU Compiler Collection
} linker_kind_t;

/**
 * \brief Enumeration of linker output kinds.
 */
typedef enum linker_output_kind_t
{
  LINKER_OUTPUT_DYNAMIC_NONPIE,       ///< Generates a dynamically linked non position-independent executable file.
  LINKER_OUTPUT_DYNAMIC_PIE,          ///< Generates a dynamically linked position-independent executable file.
  LINKER_OUTPUT_STATIC_NONPIE,        ///< Generates a statically linked non position-independent executable file.
  LINKER_OUTPUT_STATIC_PIE,           ///< Generates a statically linked position-independent executable file.
  LINKER_OUTPUT_DYNAMIC_LIBRARY,      ///< Generates a dynamic library file.
  LINKER_OUTPUT_DYNAMIC_LIBRARY_LIBC, ///< Generates a dynamic library file with bundled libc.
} linker_output_kind_t;

/**
 * \brief Enumeration of optimization level kinds.
 */
typedef enum linker_optimization_level_t
{
  LINKER_OPTIMIZATION_NONE,       ///< No optimization.
  LINKER_OPTIMIZATION_LESS,       ///< Basic optimization.
  LINKER_OPTIMIZATION_DEFAULT,    ///< Moderate optimization.
  LINKER_OPTIMIZATION_AGGRESSIVE, ///< Aggressive optimization.
  LINKER_OPTIMIZATION_SIZE,       ///< Optimize for minimum binary size.
  LINKER_OPTIMIZATION_SPEED,      ///< Optimize for maximum runtime performance.
  LINKER_OPTIMIZATION_DEBUG       ///< Optimize for debugging.
} linker_optimization_level_t;

/**
 * \brief Enumeration of symbol visibility kinds.
 */
typedef enum linker_visibility_t
{
  LINKER_VISIBILITY_DEFAULT,  ///< Symbols are visible to other binaries by default.
  LINKER_VISIBILITY_HIDDEN,   ///< Symbols are not visible to other binaries.
  LINKER_VISIBILITY_PROTECTED ///< Symbols are visible to other binaries but cannot be overridden by other binaries.
} linker_visibility_t;

/**
 * \brief Initializes a new linker of a specified kind.
 *
 * \param [in] kind The linker kind.
 * \returns Pointer to the newly initialized linker.
 */
linker_t* linker_init(linker_kind_t kind);

/**
 * \brief Frees all allocated memory associated with a linker.
 *
 * \param[in] linker Pointer to the linker to be freed.
 */
void linker_free(linker_t* linker);

/**
 * \brief Adds an object file to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] path Path to the object file.
 */
void linker_add_object(linker_t* linker, const char* path);

/**
 * \brief Adds a directory to be searched for libraries to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] path Path to the directory.
 */
void linker_add_library_directory(linker_t* linker, const char* path);

/**
 * \brief Adds a static library given its name to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] name Name of the static library.
 */
void linker_add_static_library_by_name(linker_t* linker, const char* name);

/**
 * \brief Adds a static library given its path to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] path Path to the static library.
 */
void linker_add_static_library_by_path(linker_t* linker, const char* path);

/**
 * \brief Adds a dynamic library given its name to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] name Name of the dynamic library.
 */
void linker_add_dynamic_library_by_name(linker_t* linker, const char* name);

/**
 * \brief Adds a dynamic library given its path to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] path Path to the dynamic library.
 */
void linker_add_dynamic_library_by_path(linker_t* linker, const char* path);

/**
 * \brief Sets the output kind of the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] kind The output kind.
 */
void linker_set_output_kind(linker_t* linker, linker_output_kind_t kind);

/**
 * \brief Sets the output file of the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] file Path to the output file.
 */
void linker_set_output_file(linker_t* linker, const char* file);

/**
 * \brief Sets the optimization level of the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] level The optimization level.
 */
void linker_set_optimization_level(linker_t* linker, linker_optimization_level_t level);

/**
 * \brief Enables or disables the generation of debugging information for the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] debugging `true` to enable debugging, `false` to disable debugging.
 */
void linker_set_debugging(linker_t* linker, bool debugging);

/**
 * \brief Sets the entry point of the generated file for the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] entry The name of the entry point.
 */
void linker_set_entry_point(linker_t* linker, const char* entry);

/**
 * \brief Sets the symbol visibility for the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] visibility The symbol visibility kind.
 */
void linker_set_visibility(linker_t* linker, linker_visibility_t visibility);

/**
 * \brief Adds a custom flag to the linker.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \param[in] flag The custom flag.
 */
void linker_add_flag(linker_t* linker, const char* flag);

/**
 * \brief Executes the linking process.
 *
 * \param[in] linker Pointer to the linker to be used.
 * \returns `true` if linking was successful, `false` otherwise.
 */
bool linker_link(linker_t* linker);

#endif
