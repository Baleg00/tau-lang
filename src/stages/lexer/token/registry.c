/**
* \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/lexer/token/registry.h"

#include "utils/hash.h"
#include "utils/collections/vector.h"
#include "utils/memory/arena.h"

/**
 * \brief Represents an entry corresponding to a source file in the token registry.
 */
typedef struct tau_token_registry_entry_t
{
  uint64_t key; // The hash of the file path.
  const char* path; // The path to the source file.
  const char* src; // The contents of the source file.
  tau_arena_t* arena; // The arena allocator for the tokens associated with this file.
} tau_token_registry_entry_t;

/**
 * \brief Vector of token registry entries.
 */
static tau_vector_t* g_token_registry = NULL;

void tau_token_registry_register_file(const char* path, const char* src)
{
  if (g_token_registry == NULL)
    g_token_registry = tau_vector_init();

  uint64_t key = tau_hash_digest(path, strlen(path));

  TAU_VECTOR_FOR_LOOP(i, g_token_registry)
  {
    tau_token_registry_entry_t* entry = (tau_token_registry_entry_t*)tau_vector_get(g_token_registry, i);

    if (entry->key == key)
      return;
  }

  tau_token_registry_entry_t* entry = (tau_token_registry_entry_t*)malloc(sizeof(tau_token_registry_entry_t));
  TAU_ASSERT(entry != NULL);

  entry->key = key;
  entry->path = path;
  entry->src = src;
  entry->arena = tau_arena_init();

  tau_vector_push(g_token_registry, entry);
}

tau_token_t* tau_token_registry_token_init(const char* path, tau_token_kind_t kind, size_t pos)
{
  if (g_token_registry == NULL)
    return NULL;

  uint64_t key = tau_hash_digest(path, strlen(path));

  TAU_VECTOR_FOR_LOOP(i, g_token_registry)
  {
    tau_token_registry_entry_t* entry = (tau_token_registry_entry_t*)tau_vector_get(g_token_registry, i);

    if (entry->key == key)
    {
      tau_token_t* tok = (tau_token_t*)tau_arena_alloc(entry->arena, sizeof(tau_token_t));

      tok->kind = kind;
      tok->pos = pos;

      return tok;
    }
  }

  return NULL;
}

void tau_token_registry_path_and_src(tau_token_t* tok, const char** path, const char** src)
{
  if (g_token_registry == NULL)
    return;

  TAU_VECTOR_FOR_LOOP(i, g_token_registry)
  {
    tau_token_registry_entry_t* entry = (tau_token_registry_entry_t*)tau_vector_get(g_token_registry, i);

    if (tau_arena_owns(entry->arena, tok))
    {
      *path = entry->path;
      *src = entry->src;
      return;
    }
  }
}

void tau_token_registry_free(void)
{
  if (g_token_registry == NULL)
    return;

  TAU_VECTOR_FOR_LOOP(i, g_token_registry)
  {
    tau_token_registry_entry_t* entry = (tau_token_registry_entry_t*)tau_vector_get(g_token_registry, i);

    tau_arena_free(entry->arena);
    free(entry);
  }

  tau_vector_free(g_token_registry);
  g_token_registry = NULL;
}
