/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/collections/list.h"

struct tau_list_node_t
{
  void* data;        ///< Pointer to the stored data.
  tau_list_t* owner;     ///< Pointer to the list which the node belongs to.
  tau_list_node_t* prev; ///< Pointer to the previous node or NULL.
  tau_list_node_t* next; ///< Pointer to the next node or NULL.
};

struct tau_list_t
{
  tau_list_node_t* head; ///< Pointer to the first node in the list or NULL.
  tau_list_node_t* tail; ///< Pointer to the last node in the list or NULL.
  size_t len;        ///< Number of nodes in the list.
};

/**
 * \brief Initializes a list node with the given data.
 *
 * \param[in] data The data to be stored in the node.
 * \returns Pointer to the initialized list node.
 */
static tau_list_node_t* tau_list_node_init(void* data)
{
  tau_list_node_t* node = (tau_list_node_t*)malloc(sizeof(tau_list_node_t));
  TAU_ASSERT(node != NULL);

  node->data = data;
  node->owner = NULL;
  node->prev = NULL;
  node->next = NULL;

  return node;
}

/**
 * \brief Frees the resources associated with a list node.
 *
 * \param[in] node Pointer to the list node to free.
 */
static void tau_list_node_free(tau_list_node_t* node)
{
  free(node);
}

tau_list_node_t* tau_list_node_prev(tau_list_node_t* node)
{
  return node->prev;
}

tau_list_node_t* tau_list_node_next(tau_list_node_t* node)
{
  return node->next;
}

void* tau_list_node_get(const tau_list_node_t* node)
{
  return node->data;
}

void tau_list_node_set(tau_list_node_t* restrict node, void* restrict data)
{
  node->data = data;
}

tau_list_t* tau_list_init(void)
{
  tau_list_t* list = (tau_list_t*)malloc(sizeof(tau_list_t));
  TAU_ASSERT(list != NULL);

  list->len = 0;
  list->head = NULL;
  list->tail = NULL;

  return list;
}

tau_list_t* tau_list_init_from_buffer(const void* buffer, size_t length)
{
  tau_list_t* list = tau_list_init();

  for (size_t i = 0; i < length; i++)
    tau_list_push_back(list, ((void* const*)buffer)[i]);

  return list;
}

void tau_list_free(tau_list_t* list)
{
  for (tau_list_node_t *node = list->head, *next; node != NULL; node = next)
  {
    next = node->next;
    tau_list_node_free(node);
  }

  free(list);
}

tau_list_t* tau_list_copy(const tau_list_t* list)
{
  tau_list_t* new_list = tau_list_init();

  TAU_LIST_FOR_LOOP(node, list)
    tau_list_push_back(new_list, node->data);

  return new_list;
}

void* tau_list_front(const tau_list_t* list)
{
  return list->head->data;
}

void* tau_list_back(const tau_list_t* list)
{
  return list->tail->data;
}

tau_list_node_t* tau_list_front_node(const tau_list_t* list)
{
  return list->head;
}

tau_list_node_t* tau_list_back_node(const tau_list_t* list)
{
  return list->tail;
}

tau_list_node_t* tau_list_push_front(tau_list_t* restrict list, void* restrict data)
{
  tau_list_node_t* new_node = tau_list_node_init(data);

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

tau_list_node_t* tau_list_push_back(tau_list_t* restrict list, void* restrict data)
{
  tau_list_node_t* new_node = tau_list_node_init(data);

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

void* tau_list_pop_front(tau_list_t* list)
{
  tau_list_node_t* node = list->head;

  if (list->head == list->tail)
    list->head = list->tail = NULL;
  else
  {
    list->head= list->head->next;
    list->head->prev = NULL;
  }

  --list->len;

  void* data = node->data;

  tau_list_node_free(node);

  return data;
}

void* tau_list_pop_back(tau_list_t* list)
{
  tau_list_node_t* node = list->tail;

  if (list->head == list->tail)
    list->head = list->tail = NULL;
  else
  {
    list->tail = list->tail->prev;
    list->tail->next = NULL;
  }

  --list->len;

  void* data = node->data;

  tau_list_node_free(node);

  return data;
}

tau_list_node_t* tau_list_insert_before(tau_list_node_t* restrict node, void* restrict data)
{
  if (node->owner->head == node)
    return tau_list_push_front(node->owner, data);

  tau_list_node_t* new_node = tau_list_node_init(data);

  new_node->owner = node->owner;
  new_node->prev = node->prev;
  new_node->next = node;

  node->prev->next = new_node;
  node->prev = new_node;

  ++node->owner->len;

  return new_node;
}

tau_list_node_t* tau_list_insert_after(tau_list_node_t* restrict node, void* restrict data)
{
  if (node->owner->tail == node)
    return tau_list_push_back(node->owner, data);

  tau_list_node_t* new_node = tau_list_node_init(data);

  new_node->owner = node->owner;
  new_node->prev = node;
  new_node->next = node->next;

  node->next->prev = new_node;
  node->next = new_node;

  ++node->owner->len;

  return new_node;
}

void* tau_list_remove(tau_list_node_t* node)
{
  if (node->owner->head == node)
    return tau_list_pop_front(node->owner);

  if (node->owner->tail == node)
    return tau_list_pop_back(node->owner);

  --node->owner->len;
  node->prev->next = node->next;
  node->next->prev = node->prev;

  void* data = node->data;

  tau_list_node_free(node);

  return data;
}

void* tau_list_remove_before(tau_list_node_t* node)
{
  return tau_list_remove(node->prev);
}

void* tau_list_remove_after(tau_list_node_t* node)
{
  return tau_list_remove(node->next);
}

void tau_list_clear(tau_list_t* list)
{
  for (tau_list_node_t* node = list->head, *next; node != NULL; node = next)
  {
    next = node->next;
    tau_list_node_free(node);
  }

  list->head = NULL;
  list->tail = NULL;
  list->len = 0;
}

bool tau_list_empty(const tau_list_t* list)
{
  return list->len == 0;
}

size_t tau_list_size(const tau_list_t* list)
{
  return list->len;
}

void tau_list_for_each(tau_list_t* list, tau_list_for_each_func_t func)
{
  TAU_LIST_FOR_LOOP(node, list)
    func(node->data);
}

void tau_list_to_buffer(const tau_list_t* restrict list, void* restrict buffer)
{
  size_t i = 0;

  TAU_LIST_FOR_LOOP(node, list)
    ((void**)buffer)[i++] = node->data;
}
