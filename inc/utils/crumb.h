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

#include <stdio.h>

#include "stages/lexer/location.h"
#include "utils/collections/vector.h"

/**
 * \brief Represents a code snippet in a crumb message.
 */
typedef struct crumb_snippet_t
{
  location_t loc; // Snippet location in the source code.
  vector_t* labels; // Labels associated with the snippet.
  vector_t* subsnippets; // Subsnippets associated with the snippet.
} crumb_snippet_t;

/**
 * \brief Represents a note in a crumb message.
 */
typedef struct crumb_note_t
{
  char* text; // The text of the note.
  vector_t* snippets; // Snippets associated with the note.
} crumb_note_t;

/**
 * \brief Represents a crumb error message.
 */
typedef struct crumb_error_t
{
  size_t code; // Error code.
  char* title; // Title of the error.
  vector_t* snippets; // Snippets associated with the error.
  vector_t* notes; // Notes associated with the error.
} crumb_error_t;

/**
 * \brief Initializes a new snippet.
 *
 * \param[in] loc Source code location of the snippet.
 * \returns Pointer to the newly initialized snippet.
 */
crumb_snippet_t* crumb_snippet_init(location_t loc);

/**
 * \brief Frees all memory associated with a snippet.
 *
 * \param[in] snippet Pointer to the snippet to be freed.
 */
void crumb_snippet_free(crumb_snippet_t* snippet);

/**
 * \brief Adds a label to a snippet.
 *
 * \param[in] snippet Pointer to the snippet to add a label to.
 * \param[in] fmt Format string of the label.
 * \param[in] ... Variadic arguments for the label.
 */
void crumb_snippet_label(crumb_snippet_t* snippet, const char* fmt, ...);

/**
 * \brief Adds a subsnippet to a snippet.
 *
 * \param[in] snippet Pointer to the snippet to add a subsnippet to.
 * \param[in] subsnippet Pointer to the subsnippet to be added.
 */
void crumb_snippet_subsnippet(crumb_snippet_t* snippet, crumb_snippet_t* subsnippet);

/**
 * \brief Initializes a new note.
 *
 * \param[in] fmt Format string of the note.
 * \param[in] ... Variadic arguments for the note.
 * \returns Pointer to the newly initialized note.
 */
crumb_note_t* crumb_note_init(const char* fmt, ...);

/**
 * \brief Frees all memory associated with a note.
 *
 * \param[in] note Pointer to the note to be freed.
 */
void crumb_note_free(crumb_note_t* note);

/**
 * \brief Adds a snippet to a note.
 *
 * \param[in] note Pointer to the note to add a snippet to.
 * \param[in] snippet Pointer to the snippet to be added.
 */
void crumb_note_snippet(crumb_note_t* note, crumb_snippet_t* snippet);

/**
 * \brief Initializes a new crumb error.
 *
 * \param[in] code Error code.
 * \param[in] fmt Format string of the error title.
 * \param[in] ... Variadic arguments for the error title.
 * \returns Pointer to the newly initialized error.
 */
crumb_error_t* crumb_error_init(size_t code, const char* fmt, ...);

/**
 * \brief Frees all memory associated with an error.
 *
 * \param[in] error Pointer to the error to be freed.
 */
void crumb_error_free(crumb_error_t* error);

/**
 * \brief Adds a snippet to an error.
 *
 * \param[in] error Pointer to the error to add a snippet to.
 * \param[in] snippet Pointer to the snippet to be added.
 */
void crumb_error_snippet(crumb_error_t* error, crumb_snippet_t* snippet);

/**
 * \brief Adds a note to an error.
 *
 * \param[in] error Pointer to the error to add a note to.
 * \param[in] note Pointer to the note to be added.
 */
void crumb_error_note(crumb_error_t* error, crumb_note_t* note);

/**
 * \brief Prints an error to the global crumb output stream.
 *
 * \param[in] error Pointer to the error to be printed.
 */
void crumb_error_print(crumb_error_t* error);

/**
 * \brief Sets the crumb output stream.
 * 
 * \param[in] stream Pointer to the output stream.
 */
void crumb_set_stream(FILE* stream);

/**
 * \brief Gets the crumb output stream.
 * 
 * \returns Pointer to the output stream.
 */
FILE* crumb_get_stream(void);

#endif