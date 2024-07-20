/**
* \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/lexer/token/registry.h"

#include "utils/common.h"
#include "utils/hash.h"
#include "utils/collections/vector.h"
#include "utils/memory/arena.h"
#include "utils/memory/memtrace.h"

/**
 * \brief Represents an entry corresponding to a source file in the token registry.
 */
typedef struct token_registry_entry_t
{
  uint64_t key; // The hash of the file path.
  const char* path; // The path to the source file.
  const char* src; // The contents of the source file.
  arena_t* arena; // The arena allocator for the tokens associated with this file.
} token_registry_entry_t;

/**
 * \brief Vector of token registry entries.
 */
static vector_t* g_token_registry = NULL;

void token_registry_register_file(const char* path, const char* src)
{
  if (g_token_registry == NULL)
    g_token_registry = vector_init();

  uint64_t key = hash_digest(path, strlen(path));

  VECTOR_FOR_LOOP(i, g_token_registry)
  {
    token_registry_entry_t* entry = (token_registry_entry_t*)vector_get(g_token_registry, i);

    if (entry->key == key)
      return;
  }

  token_registry_entry_t* entry = (token_registry_entry_t*)malloc(sizeof(token_registry_entry_t));
  ASSERT(entry != NULL);

  entry->key = key;
  entry->path = path;
  entry->src = src;
  entry->arena = arena_init();

  vector_push(g_token_registry, entry);
}

token_t* token_registry_token_init(const char* path, token_kind_t kind, size_t pos)
{
  if (g_token_registry == NULL)
    return NULL;

  uint64_t key = hash_digest(path, strlen(path));

  VECTOR_FOR_LOOP(i, g_token_registry)
  {
    token_registry_entry_t* entry = (token_registry_entry_t*)vector_get(g_token_registry, i);

    if (entry->key == key)
    {
      token_t* tok = (token_t*)arena_alloc(entry->arena, sizeof(token_t));

      tok->kind = kind;
      tok->pos = pos;

      return tok;
    }
  }

  return NULL;
}

void token_registry_path_and_src(token_t* tok, const char** path, const char** src)
{
  if (g_token_registry == NULL)
    return;

  VECTOR_FOR_LOOP(i, g_token_registry)
  {
    token_registry_entry_t* entry = (token_registry_entry_t*)vector_get(g_token_registry, i);

    if (arena_owns(entry->arena, tok))
    {
      *path = entry->path;
      *src = entry->src;
      return;
    }
  }
}

void token_registry_free(void)
{
  if (g_token_registry == NULL)
    return;

  VECTOR_FOR_LOOP(i, g_token_registry)
  {
    token_registry_entry_t* entry = (token_registry_entry_t*)vector_get(g_token_registry, i);

    arena_free(entry->arena);
    free(entry);
  }

  vector_free(g_token_registry);
  g_token_registry = NULL;
}
