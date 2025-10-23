/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/collections/vector.h"

/// The initial number of elements a vector should be able to hold.
#define TAU_VECTOR_INITIAL_CAPACITY ((size_t)16)

struct tau_vector_t
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
static void tau_vector_expand(tau_vector_t* vec, size_t new_capacity)
{
  if (vec->capacity >= new_capacity)
    return;

  vec->capacity = new_capacity;
  vec->data = (void**)realloc(vec->data, sizeof(void*) * vec->capacity);
  TAU_ASSERT(vec->data != NULL);
}

tau_vector_t* tau_vector_init(void)
{
  return tau_vector_init_with_capacity(TAU_VECTOR_INITIAL_CAPACITY);
}

tau_vector_t* tau_vector_init_with_capacity(size_t capacity)
{
  TAU_ASSERT(capacity > 0);

  tau_vector_t* vec = (tau_vector_t*)malloc(sizeof(tau_vector_t));
  TAU_ASSERT(vec != NULL);

  vec->size = 0;
  vec->capacity = capacity;
  vec->data = (void**)malloc(sizeof(void*) * capacity);
  TAU_ASSERT(vec->data != NULL);

  return vec;
}

tau_vector_t* tau_vector_init_from_buffer(void* buffer, size_t length)
{
  if (length == 0)
    return tau_vector_init();

  tau_vector_t* vec = tau_vector_init_with_capacity(length);

  memcpy(vec->data, buffer, sizeof(void*) * length);
  vec->size = length;

  return vec;
}

void tau_vector_free(tau_vector_t* vec)
{
  free(vec->data);
  free(vec);
}

tau_vector_t* tau_vector_copy(const tau_vector_t* vec)
{
  tau_vector_t* new_vec = tau_vector_init_with_capacity(vec->size);

  new_vec->size = vec->size;
  memcpy(new_vec->data, vec->data, sizeof(void*) * vec->size);

  return new_vec;
}

void* tau_vector_get(const tau_vector_t* vec, size_t idx)
{
  return vec->data[idx];
}

void tau_vector_set(const tau_vector_t* restrict vec, size_t idx, void* restrict data)
{
  vec->data[idx] = data;
}

void* tau_vector_front(const tau_vector_t* vec)
{
  return vec->data[0];
}

void* tau_vector_back(const tau_vector_t* vec)
{
  return vec->data[vec->size - 1];
}

void tau_vector_push(tau_vector_t* restrict vec, void* restrict data)
{
  if (vec->size + 1 >= vec->capacity)
    tau_vector_expand(vec, vec->capacity << 1);

  vec->data[vec->size++] = data;
}

void* tau_vector_pop(tau_vector_t* vec)
{
  return vec->data[--vec->size];
}

void tau_vector_insert(tau_vector_t* restrict vec, size_t idx, void* restrict data)
{
  if (vec->size + 1 >= vec->capacity)
    tau_vector_expand(vec, vec->capacity << 1);

  memmove(vec->data + idx + 1, vec->data + idx, sizeof(void*) * (vec->size - idx));

  vec->data[idx] = data;
  vec->size++;
}

void tau_vector_extend(tau_vector_t* restrict dest, const tau_vector_t* restrict src)
{
  size_t new_capacity = dest->capacity;

  while (new_capacity < dest->size + src->size)
    new_capacity <<= 1;

  tau_vector_expand(dest, new_capacity);

  memcpy(dest->data + dest->size, src->data, sizeof(void*) * src->size);

  dest->size += src->size;
}

void* tau_vector_remove(tau_vector_t* vec, size_t idx)
{
  void* temp = vec->data[idx];

  memmove(vec->data + idx, vec->data + idx + 1, sizeof(void*) * (vec->size - idx - 1));

  vec->size--;

  return temp;
}

void tau_vector_clear(tau_vector_t* vec)
{
  vec->size = 0;
  vec->capacity = TAU_VECTOR_INITIAL_CAPACITY;

  vec->data = (void**)realloc(vec->data, sizeof(void*) * TAU_VECTOR_INITIAL_CAPACITY);
  TAU_ASSERT(vec->data != NULL);
}

size_t tau_vector_find(const tau_vector_t* restrict vec, void* restrict data)
{
  size_t i = 0;

  for (; i < vec->size; i++)
    if (vec->data[i] == data)
      return i;

  return i;
}

size_t tau_vector_size(const tau_vector_t* vec)
{
  return vec->size;
}

bool tau_vector_empty(const tau_vector_t* vec)
{
  return vec->size == 0;
}

void tau_vector_for_each(const tau_vector_t* vec, tau_vector_for_each_func_t func)
{
  for (size_t i = 0; i < vec->size; ++i)
    func(vec->data[i]);
}

void tau_vector_to_buffer(const tau_vector_t* restrict vec, void* restrict buffer)
{
  memcpy(buffer, vec->data, sizeof(void*) * vec->size);
}
