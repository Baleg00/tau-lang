/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/crumb.h"

#include "utils/esc_seq.h"

static FILE* g_crumb_stream = NULL;

static void tau_crumb_snippet_print(tau_crumb_snippet_t* snippet)
{
  fprintf(g_crumb_stream, TAU_ESC_FG_BRIGHT_BLACK "[%s:%zu:%zu]\n" TAU_ESC_RESET, snippet->loc.path, snippet->loc.row + 1, snippet->loc.col);

  const char* row_begin = snippet->loc.ptr;

  while (row_begin > snippet->loc.src && *(row_begin - 1) != '\n')
    row_begin--;

  const char* row_end = snippet->loc.ptr;

  while (*row_end != '\0' && *row_end != '\n')
    row_end++;

  fprintf(g_crumb_stream, " %4zu | ", snippet->loc.row + 1);
  fprintf(g_crumb_stream, "%.*s", (int)(snippet->loc.ptr - row_begin), row_begin);
  fprintf(g_crumb_stream, TAU_ESC_FG_RED "%.*s" TAU_ESC_RESET, (int)snippet->loc.len, snippet->loc.ptr);
  fprintf(g_crumb_stream, "%.*s\n", (int)(row_end - snippet->loc.ptr - snippet->loc.len), snippet->loc.ptr + snippet->loc.len);

  int marker_offset = (int)(snippet->loc.ptr - row_begin + 8);

  fprintf(g_crumb_stream, TAU_ESC_FG_RED "%*c", marker_offset + 1, '^');

  for (size_t i = 1; i < snippet->loc.len; i++)
    fputc('~', g_crumb_stream);

  fputs(TAU_ESC_RESET, g_crumb_stream);

  if (tau_vector_empty(snippet->labels))
    fputc('\n', g_crumb_stream);
  else
  {
    fprintf(g_crumb_stream, " %s\n", (const char*)tau_vector_front(snippet->labels));

    for (size_t i = 1; i < tau_vector_size(snippet->labels); i++)
    {
      fprintf(g_crumb_stream, "%*c\n", marker_offset + 1, '|');
      fprintf(g_crumb_stream, "%*s %s\n", marker_offset + 2, "+-", (const char*)tau_vector_get(snippet->labels, i));
    }
  }

  TAU_VECTOR_FOR_LOOP(i, snippet->subsnippets)
    tau_crumb_snippet_print((tau_crumb_snippet_t*)tau_vector_get(snippet->subsnippets, i));
}

static void tau_crumb_note_print(tau_crumb_note_t* note)
{
  fprintf(g_crumb_stream, "# %s\n", note->text);

  TAU_VECTOR_FOR_LOOP(i, note->snippets)
    tau_crumb_snippet_print((tau_crumb_snippet_t*)tau_vector_get(note->snippets, i));
}

tau_crumb_snippet_t* tau_crumb_snippet_init(tau_location_t loc)
{
  tau_crumb_snippet_t* snippet = (tau_crumb_snippet_t*)malloc(sizeof(tau_crumb_snippet_t));
  TAU_ASSERT(snippet != NULL);

  snippet->loc = loc;
  snippet->labels = tau_vector_init();
  snippet->subsnippets = tau_vector_init();

  return snippet;
}

void tau_crumb_snippet_free(tau_crumb_snippet_t* snippet)
{
  TAU_VECTOR_FOR_LOOP(i, snippet->labels)
    free(tau_vector_get(snippet->labels, i));

  tau_vector_free(snippet->labels);

  TAU_VECTOR_FOR_LOOP(i, snippet->subsnippets)
    tau_crumb_snippet_free((tau_crumb_snippet_t*)tau_vector_get(snippet->subsnippets, i));

  tau_vector_free(snippet->subsnippets);

  free(snippet);
}

void tau_crumb_snippet_label(tau_crumb_snippet_t* snippet, const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  size_t size = (size_t)vsnprintf(NULL, 0, fmt, args) + 1;

  char* label = (char*)malloc(sizeof(char) * size);
  TAU_ASSERT(label != NULL);

  vsnprintf(label, size, fmt, args);

  va_end(args);

  tau_vector_push(snippet->labels, label);
}

void tau_crumb_snippet_subsnippet(tau_crumb_snippet_t* snippet, tau_crumb_snippet_t* subsnippet)
{
  tau_vector_push(snippet->subsnippets, subsnippet);
}

tau_crumb_note_t* tau_crumb_note_init(const char* fmt, ...)
{
  tau_crumb_note_t* note = (tau_crumb_note_t*)malloc(sizeof(tau_crumb_note_t));
  TAU_ASSERT(note != NULL);

  va_list args;
  va_start(args, fmt);

  size_t size = (size_t)vsnprintf(NULL, 0, fmt, args) + 1;

  char* text = (char*)malloc(sizeof(char) * size);
  TAU_ASSERT(text != NULL);

  vsnprintf(text, size, fmt, args);

  va_end(args);

  note->text = text;
  note->snippets = tau_vector_init();

  return note;
}

void tau_crumb_note_free(tau_crumb_note_t* note)
{
  free(note->text);

  TAU_VECTOR_FOR_LOOP(i, note->snippets)
    tau_crumb_snippet_free((tau_crumb_snippet_t*)tau_vector_get(note->snippets, i));

  tau_vector_free(note->snippets);

  free(note);
}

void tau_crumb_note_snippet(tau_crumb_note_t* note, tau_crumb_snippet_t* snippet)
{
  tau_vector_push(note->snippets, snippet);
}

tau_crumb_error_t* tau_crumb_error_init(size_t code, const char* fmt, ...)
{
  tau_crumb_error_t* error = (tau_crumb_error_t*)malloc(sizeof(tau_crumb_error_t));
  TAU_ASSERT(error != NULL);

  error->code = code;

  va_list args;
  va_start(args, fmt);

  size_t size = (size_t)vsnprintf(NULL, 0, fmt, args) + 1;

  char* title = (char*)malloc(sizeof(char) * size);
  TAU_ASSERT(title != NULL);

  vsnprintf(title, size, fmt, args);

  va_end(args);

  error->title = title;
  error->snippets = tau_vector_init();
  error->notes = tau_vector_init();

  return error;
}

void tau_crumb_error_free(tau_crumb_error_t* error)
{
  free(error->title);

  TAU_VECTOR_FOR_LOOP(i, error->snippets)
    tau_crumb_snippet_free((tau_crumb_snippet_t*)tau_vector_get(error->snippets, i));

  tau_vector_free(error->snippets);

  TAU_VECTOR_FOR_LOOP(i, error->notes)
    tau_crumb_note_free((tau_crumb_note_t*)tau_vector_get(error->notes, i));

  tau_vector_free(error->notes);

  free(error);
}

void tau_crumb_error_snippet(tau_crumb_error_t* error, tau_crumb_snippet_t* snippet)
{
  tau_vector_push(error->snippets, snippet);
}

void tau_crumb_error_note(tau_crumb_error_t* error, tau_crumb_note_t* note)
{
  tau_vector_push(error->notes, note);
}

void tau_crumb_error_print(tau_crumb_error_t* error)
{
  fprintf(g_crumb_stream, TAU_ESC_FG_RED "Error (E%04zu): %s\n" TAU_ESC_RESET, error->code, error->title);

  TAU_VECTOR_FOR_LOOP(i, error->snippets)
    tau_crumb_snippet_print((tau_crumb_snippet_t*)tau_vector_get(error->snippets, i));

  TAU_VECTOR_FOR_LOOP(i, error->notes)
    tau_crumb_note_print((tau_crumb_note_t*)tau_vector_get(error->notes, i));
}

void tau_crumb_set_stream(FILE* stream)
{
  g_crumb_stream = stream;
}

FILE* tau_crumb_get_stream(void)
{
  return g_crumb_stream;
}
