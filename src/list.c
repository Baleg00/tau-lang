/**
 * \file list.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "list.h"

#include <stdlib.h>

#include "util.h"

#include "memtrace.h"

struct list_node_t
{
  void* data; // Pointer to the stored data.
  list_t* owner; // Pointer to the list which the node belongs to.
  list_node_t* prev; // Pointer to the previous node or NULL.
  list_node_t* next; // Pointer to the next node or NULL.
};

struct list_t
{
  list_node_t* head; // Pointer to the first node in the list or NULL.
  list_node_t* tail; // Pointer to the last node in the list or NULL.
  size_t len; // Number of nodes in the list.
};

list_node_t* list_node_init(void* data)
{
  list_node_t* node = (list_node_t*)malloc(sizeof(list_node_t));
  assert(node != NULL);

  node->data = data;
  node->owner = NULL;
  node->prev = NULL;
  node->next = NULL;

  return node;
}

void list_node_free(list_node_t* node)
{
  free(node);
}

list_node_t* list_node_copy(list_node_t* node)
{
  assert(node != NULL);
  return list_node_init(node->data);
}

list_node_t* list_node_prev(list_node_t* node)
{
  assert(node != NULL);
  return node->prev;
}

list_node_t* list_node_next(list_node_t* node)
{
  assert(node != NULL);
  return node->next;
}

void* list_node_get(list_node_t* node)
{
  assert(node != NULL);
  return node->data;
}

void list_node_set(list_node_t* node, void* data)
{
  assert(node != NULL);
  node->data = data;
}

list_t* list_init(void)
{
  list_t* list = (list_t*)malloc(sizeof(list_t));
  assert(list != NULL);

  list->len = 0;
  list->head = NULL;
  list->tail = NULL;

  return list;
}

list_t* list_init_from_buffer(void* buffer, size_t length)
{
  list_t* list = list_init();

  for (size_t i = 0; i < length; i++)
    list_push_back(list, ((void**)buffer)[i]);

  return list;
}

void list_free(list_t* list)
{
  if (list == NULL)
    return;

  for (list_node_t *node = list_front_node(list), *next; node != NULL; node = next)
  {
    next = list_node_next(node);
    list_node_free(node);
  }

  free(list);
}

list_t* list_copy(list_t* list)
{
  list_t* new_list = list_init();

  LIST_FOR_LOOP(node, list)
    list_push_back(new_list, list_node_get(node));

  return new_list;
}

void* list_front(list_t* list)
{
  assert(!list_empty(list));
  return list_node_get(list_front_node(list));
}

void* list_back(list_t* list)
{
  assert(!list_empty(list));
  return list_node_get(list_back_node(list));
}

list_node_t* list_front_node(list_t* list)
{
  assert(list != NULL);
  return list_empty(list) ? NULL : list->head;
}

list_node_t* list_back_node(list_t* list)
{
  assert(list != NULL);
  return list_empty(list) ? NULL : list->tail;
}

list_node_t* list_push_front(list_t* list, void* data)
{
  assert(list != NULL);

  list_node_t* new_node = list_node_init(data);

  new_node->owner = list;
  new_node->next = list->head;
  new_node->prev = NULL;

  if (list->tail == NULL)
    list->tail = new_node;
  else
    list->head->prev = new_node;

  list->head = new_node;

  ++list->len;

  return new_node;
}

list_node_t* list_push_back(list_t* list, void* data)
{
  assert(list != NULL);

  list_node_t* new_node = list_node_init(data);

  new_node->owner = list;
  new_node->prev = list->tail;
  new_node->next = NULL;

  if (list->head == NULL)
    list->head = new_node;
  else
    list->tail->next = new_node;

  list->tail = new_node;

  ++list->len;

  return new_node;
}

void* list_pop_front(list_t* list)
{
  assert(!list_empty(list));

  list_node_t* node = list->head;
  
  if (list->head == list->tail)
    list->head = list->tail = NULL;
  else
  {
    list->head= list->head->next;
    list->head->prev = NULL;
  }

  --list->len;

  void* data = node->data;

  list_node_free(node);

  return data;
}

void* list_pop_back(list_t* list)
{
  assert(!list_empty(list));

  list_node_t* node = list->tail;
  
  if (list->head == list->tail)
    list->head = list->tail = NULL;
  else
  {
    list->tail = list->tail->prev;
    list->tail->next = NULL;
  }

  --list->len;

  void* data = node->data;

  list_node_free(node);

  return data;
}

list_node_t* list_insert_before(list_node_t* node, void* data)
{
  assert(node != NULL);

  if (node->owner->head == node)
    return list_push_front(node->owner, data);

  list_node_t* new_node = list_node_init(data);

  new_node->owner = node->owner;
  new_node->prev = node->prev;
  new_node->next = node;

  node->prev->next = new_node;
  node->prev = new_node;

  ++node->owner->len;
  
  return new_node;
}

list_node_t* list_insert_after(list_node_t* node, void* data)
{
  assert(node != NULL);

  if (node->owner->tail == node)
    return list_push_back(node->owner, data);

  list_node_t* new_node = list_node_init(data);

  new_node->owner = node->owner;
  new_node->prev = node;
  new_node->next = node->next;

  node->next->prev = new_node;
  node->next = new_node;

  ++node->owner->len;

  return new_node;
}

void* list_remove(list_node_t* node)
{
  assert(node != NULL);

  if (node->owner->head == node)
    return list_pop_front(node->owner);

  if (node->owner->tail == node)
    return list_pop_back(node->owner);

  --node->owner->len;
  node->prev->next = node->next;
  node->next->prev = node->prev;

  void* data = node->data;

  list_node_free(node);

  return data;
}

void* list_remove_before(list_node_t* node)
{
  assert(node != NULL);
  assert(list_node_prev(node) != NULL);
  return list_remove(node->prev);
}

void* list_remove_after(list_node_t* node)
{
  assert(node != NULL);
  assert(list_node_next(node) != NULL);
  return list_remove(node->next);
}

bool list_empty(list_t* list)
{
  assert(list != NULL);
  return list->len == 0;
}

size_t list_size(list_t* list)
{
  assert(list != NULL);
  return list->len;
}

void list_for_each(list_t* list, list_for_each_func_t func)
{
  LIST_FOR_LOOP(node, list)
    func(list_node_get(node));
}

void list_to_buffer(list_t* list, void* buffer)
{
  size_t i = 0;

  LIST_FOR_LOOP(node, list)
    ((void**)buffer)[i++] = list_node_get(node);
}
