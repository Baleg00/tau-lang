/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include "utils/crumb.h"
#include "utils/common.h"
#include "utils/esc_seq.h"
#include "utils/memory/memtrace.h"

static FILE* g_crumb_stream = NULL;

static void crumb_snippet_print(crumb_snippet_t* snippet)
{
  fprintf(g_crumb_stream, ESC_FG_BRIGHT_BLACK "[%s:%zu:%zu]\n" ESC_RESET, snippet->loc.path, snippet->loc.row + 1, snippet->loc.col);

  const char* row_begin = snippet->loc.ptr;

  while (row_begin > snippet->loc.src && *(row_begin - 1) != '\n')
    row_begin--;

  const char* row_end = snippet->loc.ptr;

  while (*row_end != '\0' && *row_end != '\n')
    row_end++;

  fprintf(g_crumb_stream, " %4zu | ", snippet->loc.row + 1);
  fprintf(g_crumb_stream, "%.*s", (int)(snippet->loc.ptr - row_begin), row_begin);
  fprintf(g_crumb_stream, ESC_FG_RED "%.*s" ESC_RESET, (int)snippet->loc.len, snippet->loc.ptr);
  fprintf(g_crumb_stream, "%.*s\n", (int)(row_end - snippet->loc.ptr - snippet->loc.len), snippet->loc.ptr + snippet->loc.len);

  int marker_offset = (int)(snippet->loc.ptr - row_begin + 8);

  fprintf(g_crumb_stream, ESC_FG_RED "%*c", marker_offset + 1, '^');

  for (size_t i = 1; i < snippet->loc.len; i++)
    fputc('~', g_crumb_stream);

  fputs(ESC_RESET, g_crumb_stream);

  if (vector_empty(snippet->labels))
    fputc('\n', g_crumb_stream);
  else
  {
    fprintf(g_crumb_stream, " %s\n", (const char*)vector_front(snippet->labels));

    for (size_t i = 1; i < vector_size(snippet->labels); i++)
    {
      fprintf(g_crumb_stream, "%*c\n", marker_offset + 1, '|');
      fprintf(g_crumb_stream, "%*s %s\n", marker_offset + 2, "+-", (const char*)vector_get(snippet->labels, i));
    }
  }

  VECTOR_FOR_LOOP(i, snippet->subsnippets)
    crumb_snippet_print((crumb_snippet_t*)vector_get(snippet->subsnippets, i));
}

static void crumb_note_print(crumb_note_t* note)
{
  fprintf(g_crumb_stream, "# %s\n", note->text);

  VECTOR_FOR_LOOP(i, note->snippets)
    crumb_snippet_print((crumb_snippet_t*)vector_get(note->snippets, i));
}

crumb_snippet_t* crumb_snippet_init(location_t loc)
{
  crumb_snippet_t* snippet = (crumb_snippet_t*)malloc(sizeof(crumb_snippet_t));
  ASSERT(snippet != NULL);

  snippet->loc = loc;
  snippet->labels = vector_init();
  snippet->subsnippets = vector_init();

  return snippet;
}

void crumb_snippet_free(crumb_snippet_t* snippet)
{
  VECTOR_FOR_LOOP(i, snippet->labels)
    free(vector_get(snippet->labels, i));

  vector_free(snippet->labels);

  VECTOR_FOR_LOOP(i, snippet->subsnippets)
    crumb_snippet_free((crumb_snippet_t*)vector_get(snippet->subsnippets, i));

  vector_free(snippet->subsnippets);

  free(snippet);
}

void crumb_snippet_label(crumb_snippet_t* snippet, const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  size_t size = (size_t)vsnprintf(NULL, 0, fmt, args) + 1;

  char* label = (char*)malloc(sizeof(char) * size);
  ASSERT(label != NULL);

  vsnprintf(label, size, fmt, args);

  va_end(args);

  vector_push(snippet->labels, label);
}

void crumb_snippet_subsnippet(crumb_snippet_t* snippet, crumb_snippet_t* subsnippet)
{
  vector_push(snippet->subsnippets, subsnippet);
}

crumb_note_t* crumb_note_init(const char* fmt, ...)
{
  crumb_note_t* note = (crumb_note_t*)malloc(sizeof(crumb_note_t));
  ASSERT(note != NULL);

  va_list args;
  va_start(args, fmt);

  size_t size = (size_t)vsnprintf(NULL, 0, fmt, args) + 1;

  char* text = (char*)malloc(sizeof(char) * size);
  ASSERT(text != NULL);

  vsnprintf(text, size, fmt, args);

  va_end(args);

  note->text = text;
  note->snippets = vector_init();

  return note;
}

void crumb_note_free(crumb_note_t* note)
{
  free(note->text);

  VECTOR_FOR_LOOP(i, note->snippets)
    crumb_snippet_free((crumb_snippet_t*)vector_get(note->snippets, i));

  vector_free(note->snippets);

  free(note);
}

void crumb_note_snippet(crumb_note_t* note, crumb_snippet_t* snippet)
{
  vector_push(note->snippets, snippet);
}

crumb_error_t* crumb_error_init(size_t code, const char* fmt, ...)
{
  crumb_error_t* error = (crumb_error_t*)malloc(sizeof(crumb_error_t));
  ASSERT(error != NULL);

  error->code = code;

  va_list args;
  va_start(args, fmt);

  size_t size = (size_t)vsnprintf(NULL, 0, fmt, args) + 1;

  char* title = (char*)malloc(sizeof(char) * size);
  ASSERT(title != NULL);

  vsnprintf(title, size, fmt, args);

  va_end(args);

  error->title = title;
  error->snippets = vector_init();
  error->notes = vector_init();

  return error;
}

void crumb_error_free(crumb_error_t* error)
{
  free(error->title);

  VECTOR_FOR_LOOP(i, error->snippets)
    crumb_snippet_free((crumb_snippet_t*)vector_get(error->snippets, i));

  vector_free(error->snippets);

  VECTOR_FOR_LOOP(i, error->notes)
    crumb_note_free((crumb_note_t*)vector_get(error->notes, i));

  vector_free(error->notes);

  free(error);
}

void crumb_error_snippet(crumb_error_t* error, crumb_snippet_t* snippet)
{
  vector_push(error->snippets, snippet);
}

void crumb_error_note(crumb_error_t* error, crumb_note_t* note)
{
  vector_push(error->notes, note);
}

void crumb_error_print(crumb_error_t* error)
{
  fprintf(g_crumb_stream, ESC_FG_RED "Error (E%04zu): %s\n" ESC_RESET, error->code, error->title);

  VECTOR_FOR_LOOP(i, error->snippets)
    crumb_snippet_print((crumb_snippet_t*)vector_get(error->snippets, i));

  VECTOR_FOR_LOOP(i, error->notes)
    crumb_note_print((crumb_note_t*)vector_get(error->notes, i));
}

void crumb_set_stream(FILE* stream)
{
  g_crumb_stream = stream;
}

FILE* crumb_get_stream(void)
{
  return g_crumb_stream;
}
