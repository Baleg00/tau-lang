/**
 * \file
 * 
 * \brief Compiler diagnostic message library interface.
 * 
 * \details The crumb library provides functions to display detailed messages
 * referencing locations in the source code making it possible to create
 * informative error and warning messages for the user during compilation.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_CRUMB_H
#define TAU_CRUMB_H

#include "stages/lexer/location.h"
#include "utils/common.h"
#include "utils/collections/vector.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Represents a code snippet in a crumb message.
 */
typedef struct tau_crumb_snippet_t
{
  tau_location_t loc; // Snippet location in the source code.
  tau_vector_t* labels; // Labels associated with the snippet.
  tau_vector_t* subsnippets; // Subsnippets associated with the snippet.
} tau_crumb_snippet_t;

/**
 * \brief Represents a note in a crumb message.
 */
typedef struct tau_crumb_note_t
{
  char* text; // The text of the note.
  tau_vector_t* snippets; // Snippets associated with the note.
} tau_crumb_note_t;

/**
 * \brief Represents a crumb error message.
 */
typedef struct tau_crumb_error_t
{
  size_t code; // Error code.
  char* title; // Title of the error.
  tau_vector_t* snippets; // Snippets associated with the error.
  tau_vector_t* notes; // Notes associated with the error.
} tau_crumb_error_t;

/**
 * \brief Initializes a new snippet.
 *
 * \param[in] loc Source code location of the snippet.
 * \returns Pointer to the newly initialized snippet.
 */
tau_crumb_snippet_t* tau_crumb_snippet_init(tau_location_t loc);

/**
 * \brief Frees all memory associated with a snippet.
 *
 * \param[in] snippet Pointer to the snippet to be freed.
 */
void tau_crumb_snippet_free(tau_crumb_snippet_t* snippet);

/**
 * \brief Adds a label to a snippet.
 *
 * \param[in] snippet Pointer to the snippet to add a label to.
 * \param[in] fmt Format string of the label.
 * \param[in] ... Variadic arguments for the label.
 */
void tau_crumb_snippet_label(tau_crumb_snippet_t* snippet, const char* fmt, ...);

/**
 * \brief Adds a subsnippet to a snippet.
 *
 * \param[in] snippet Pointer to the snippet to add a subsnippet to.
 * \param[in] subsnippet Pointer to the subsnippet to be added.
 */
void tau_crumb_snippet_subsnippet(tau_crumb_snippet_t* snippet, tau_crumb_snippet_t* subsnippet);

/**
 * \brief Initializes a new note.
 *
 * \param[in] fmt Format string of the note.
 * \param[in] ... Variadic arguments for the note.
 * \returns Pointer to the newly initialized note.
 */
tau_crumb_note_t* tau_crumb_note_init(const char* fmt, ...);

/**
 * \brief Frees all memory associated with a note.
 *
 * \param[in] note Pointer to the note to be freed.
 */
void tau_crumb_note_free(tau_crumb_note_t* note);

/**
 * \brief Adds a snippet to a note.
 *
 * \param[in] note Pointer to the note to add a snippet to.
 * \param[in] snippet Pointer to the snippet to be added.
 */
void tau_crumb_note_snippet(tau_crumb_note_t* note, tau_crumb_snippet_t* snippet);

/**
 * \brief Initializes a new crumb error.
 *
 * \param[in] code Error code.
 * \param[in] fmt Format string of the error title.
 * \param[in] ... Variadic arguments for the error title.
 * \returns Pointer to the newly initialized error.
 */
tau_crumb_error_t* tau_crumb_error_init(size_t code, const char* fmt, ...);

/**
 * \brief Frees all memory associated with an error.
 *
 * \param[in] error Pointer to the error to be freed.
 */
void tau_crumb_error_free(tau_crumb_error_t* error);

/**
 * \brief Adds a snippet to an error.
 *
 * \param[in] error Pointer to the error to add a snippet to.
 * \param[in] snippet Pointer to the snippet to be added.
 */
void tau_crumb_error_snippet(tau_crumb_error_t* error, tau_crumb_snippet_t* snippet);

/**
 * \brief Adds a note to an error.
 *
 * \param[in] error Pointer to the error to add a note to.
 * \param[in] note Pointer to the note to be added.
 */
void tau_crumb_error_note(tau_crumb_error_t* error, tau_crumb_note_t* note);

/**
 * \brief Prints an error to the global crumb output stream.
 *
 * \param[in] error Pointer to the error to be printed.
 */
void tau_crumb_error_print(tau_crumb_error_t* error);

/**
 * \brief Sets the crumb output stream.
 * 
 * \param[in] stream Pointer to the output stream.
 */
void tau_crumb_set_stream(FILE* stream);

/**
 * \brief Gets the crumb output stream.
 * 
 * \returns Pointer to the output stream.
 */
FILE* tau_crumb_get_stream(void);

TAU_EXTERN_C_END

#endif
