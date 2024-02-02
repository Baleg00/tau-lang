/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/collections/vector.h"

#include <string.h>

#include "utils/common.h"
#include "utils/memory/memtrace.h"

#define VECTOR_INITIAL_CAPACITY ((size_t)16)

struct vector_t
{
  size_t size;
  size_t capacity;
  void** data;
};

static void vector_expand(vector_t* vec)
{
  vec->capacity <<= 1;
  vec->data = (void**)realloc(vec->data, sizeof(void*) * vec->capacity);
  assert(vec->data != NULL);
}

vector_t* vector_init(void)
{
  return vector_init_with_capacity(VECTOR_INITIAL_CAPACITY);
}

vector_t* vector_init_with_capacity(size_t capacity)
{
  assert(capacity > 0);

  vector_t* vec = (vector_t*)malloc(sizeof(vector_t));
  assert(vec != NULL);

  vec->size = 0;
  vec->capacity = capacity;
  vec->data = (void**)malloc(sizeof(void*) * capacity);

  return vec;
}

vector_t* vector_init_from_buffer(void* buffer, size_t length)
{
  if (length == 0)
    return vector_init();

  vector_t* vec = vector_init_with_capacity(length);

  memcpy(vec->data, buffer, sizeof(void*) * length);
  vec->size = length;

  return vec;
}

void vector_free(vector_t* vec)
{
  if (vec == NULL)
    return;

  free(vec->data);
  free(vec);
}

vector_t* vector_copy(vector_t* vec)
{
  vector_t* new_vec = vector_init_with_capacity(vec->size);

  new_vec->size = vec->size;
  memcpy(new_vec->data, vec->data, sizeof(void*) * vec->size);

  return new_vec;
}

void* vector_get(vector_t* vec, size_t idx)
{
  assert(idx < vec->size);
  return vec->data[idx];
}

void vector_set(vector_t* vec, size_t idx, void* data)
{
  assert(idx < vec->size);
  vec->data[idx] = data;
}

void* vector_front(vector_t* vec)
{
  assert(vec->size > 0);
  return vec->data[0];
}

void* vector_back(vector_t* vec)
{
  assert(vec->size > 0);
  return vec->data[vec->size - 1];
}

void vector_push(vector_t* vec, void* data)
{
  if (vec->size == vec->capacity)
    vector_expand(vec);

  vec->data[vec->size++] = data;
}

void* vector_pop(vector_t* vec)
{
  assert(vec->size > 0);
  return vec->data[--vec->size];
}

void vector_insert(vector_t* vec, size_t idx, void* data)
{
  if (vec->size == vec->capacity)
    vector_expand(vec);

  memmove(vec->data + idx + 1, vec->data + idx, sizeof(void*) * (vec->size - idx));
  
  vec->data[idx] = data;
  vec->size++;
}

void* vector_remove(vector_t* vec, size_t idx)
{
  assert(idx < vec->size);

  void* temp = vec->data[idx];

  memmove(vec->data + idx, vec->data + idx + 1, sizeof(void*) * (vec->size - idx - 1));

  vec->size--;

  return temp;
}

void vector_clear(vector_t* vec)
{
  vec->size = 0;
  vec->capacity = VECTOR_INITIAL_CAPACITY;

  vec->data = (void**)realloc(vec->data, sizeof(void*) * VECTOR_INITIAL_CAPACITY);
  assert(vec->data != NULL);
}

size_t vector_size(vector_t* vec)
{
  return vec->size;
}

bool vector_empty(vector_t* vec)
{
  return vec->size == 0;
}

void vector_for_each(vector_t* vec, vector_for_each_func_t func)
{
  VECTOR_FOR_LOOP(i, vec)
    func(vector_get(vec, i));
}

void vector_to_buffer(vector_t* vec, void* buffer)
{
  memcpy(buffer, vec->data, sizeof(void*) * vec->size);
}
