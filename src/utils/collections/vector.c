/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/collections/vector.h"

#include <stdint.h>
#include <string.h>

#include "utils/common.h"
#include "utils/memory/memtrace.h"

/// The initial number of elements a vector should be able to hold.
#define VECTOR_INITIAL_CAPACITY ((size_t)16)

struct vector_t
{
  size_t size;     ///< The number of elements in the vector.
  size_t capacity; ///< The maximum number of elements the vector can hold.
  void** data;     ///< Pointer to the contained elements.
};

/**
 * \brief Expands the capacity of a vector.
 *
 * \param[in,out] vec Pointer to the vector to be expanded.
 * \param[in] new_capacity The new capacity of the vector.
 */
static void vector_expand(vector_t* vec, size_t new_capacity)
{
  if (vec->capacity >= new_capacity)
    return;

  vec->capacity = new_capacity;
  vec->data = (void**)realloc(vec->data, sizeof(void*) * vec->capacity);
  ASSERT(vec->data != NULL);
}

vector_t* vector_init(void)
{
  return vector_init_with_capacity(VECTOR_INITIAL_CAPACITY);
}

vector_t* vector_init_with_capacity(size_t capacity)
{
  ASSERT(capacity > 0);

  vector_t* vec = (vector_t*)malloc(sizeof(vector_t));
  ASSERT(vec != NULL);

  vec->size = 0;
  vec->capacity = capacity;
  vec->data = (void**)malloc(sizeof(void*) * capacity);
  ASSERT(vec->data != NULL);

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
  return vec->data[idx];
}

void vector_set(vector_t* vec, size_t idx, void* data)
{
  vec->data[idx] = data;
}

void* vector_front(vector_t* vec)
{
  return vec->data[0];
}

void* vector_back(vector_t* vec)
{
  return vec->data[vec->size - 1];
}

void vector_push(vector_t* vec, void* data)
{
  if (vec->size + 1 >= vec->capacity)
    vector_expand(vec, vec->capacity << 1);

  vec->data[vec->size++] = data;
}

void* vector_pop(vector_t* vec)
{
  return vec->data[--vec->size];
}

void vector_insert(vector_t* vec, size_t idx, void* data)
{
  if (vec->size + 1 >= vec->capacity)
    vector_expand(vec, vec->capacity << 1);

  memmove(vec->data + idx + 1, vec->data + idx, sizeof(void*) * (vec->size - idx));

  vec->data[idx] = data;
  vec->size++;
}

void vector_extend(vector_t* dest, vector_t* src)
{
  size_t new_capacity = dest->capacity;

  while (new_capacity < dest->size + src->size)
    new_capacity <<= 1;

  vector_expand(dest, new_capacity);

  memcpy(dest->data + dest->size, src->data, sizeof(void*) * src->size);

  dest->size += src->size;
}

void* vector_remove(vector_t* vec, size_t idx)
{
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
  ASSERT(vec->data != NULL);
}

size_t vector_find(vector_t* vec, void* data)
{
  size_t i = 0;

  for (; i < vec->size; i++)
    if (vec->data[i] == data)
      return i;

  return i;
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
  for (size_t i = 0; i < vec->size; ++i)
    func(vec->data[i]);
}

void vector_to_buffer(vector_t* vec, void* buffer)
{
  memcpy(buffer, vec->data, sizeof(void*) * vec->size);
}
