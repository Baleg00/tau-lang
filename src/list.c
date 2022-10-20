#include "list.h"

#include <stdlib.h>
#include <assert.h>

list_elem_t* list_elem_init(void* data)
{
  list_elem_t* elem = (list_elem_t*)malloc(sizeof(list_elem_t));
  assert(elem != NULL);
  elem->data = data;
  elem->owner = NULL;
  elem->prev = NULL;
  elem->next = NULL;
  return elem;
}

void list_elem_free(list_elem_t* elem)
{
  free(elem);
}

list_elem_t* list_elem_copy(list_elem_t* elem)
{
  return list_elem_init(elem->data);
}

list_elem_t* list_elem_prev(list_elem_t* elem)
{
  return elem->prev;
}

list_elem_t* list_elem_next(list_elem_t* elem)
{
  return elem->next;
}

void* list_elem_data(list_elem_t* elem)
{
  return elem->data;
}

list_t* list_init(void)
{
  list_t* list = (list_t*)malloc(sizeof(list_t));
  assert(list != NULL);
  list->len = 0;
  list->root = NULL;
  list->tail = NULL;
  return list;
}

void list_free(list_t* list)
{
  for (list_elem_t *elem = list_front_elem(list), *next; elem != NULL; elem = next)
  {
    next = list_elem_next(elem);
    list_elem_free(elem);
  }

  free(list);
}

list_t* list_copy(list_t* list)
{
  list_t* new_list = list_init();

  for (list_elem_t* elem = list_front_elem(list); elem != NULL; elem = list_elem_next(elem))
    list_push_back(new_list, list_elem_data(elem));

  return new_list;
}

void* list_front(list_t* list)
{
  assert(!list_empty(list));
  return list_elem_data(list_front_elem(list));
}

void* list_back(list_t* list)
{
  assert(!list_empty(list));
  return list_elem_data(list_back_elem(list));
}

list_elem_t* list_front_elem(list_t* list)
{
  return list_empty(list) ? NULL : list->root;
}

list_elem_t* list_back_elem(list_t* list)
{
  return list_empty(list) ? NULL : list->tail;
}

list_elem_t* list_push_front(list_t* list, void* data)
{
  list_elem_t* new_elem = list_elem_init(data);

  new_elem->owner = list;
  new_elem->next = list->root;
  new_elem->prev = NULL;

  if (list->tail == NULL)
    list->tail = new_elem;
  else
    list->root->prev = new_elem;

  list->root = new_elem;

  ++list->len;

  return new_elem;
}

list_elem_t* list_push_back(list_t* list, void* data)
{
  list_elem_t* new_elem = list_elem_init(data);

  new_elem->owner = list;
  new_elem->prev = list->tail;
  new_elem->next = NULL;

  if (list->root == NULL)
    list->root = new_elem;
  else
    list->tail->next = new_elem;

  list->tail = new_elem;

  ++list->len;

  return new_elem;
}

void* list_pop_front(list_t* list)
{
  assert(!list_empty(list));

  list_elem_t* elem = list->root;
  
  if (list->root == list->tail)
    list->root = list->tail = NULL;
  else
  {
    list->root= list->root->next;
    list->root->prev = NULL;
  }

  --list->len;

  void* data = elem->data;

  list_elem_free(elem);

  return data;
}

void* list_pop_back(list_t* list)
{
  assert(!list_empty(list));

  list_elem_t* elem = list->tail;
  
  if (list->root == list->tail)
    list->root = list->tail = NULL;
  else
  {
    list->tail = list->tail->prev;
    list->tail->next = NULL;
  }

  --list->len;

  void* data = elem->data;

  list_elem_free(elem);

  return data;
}

list_elem_t* list_insert_before(list_elem_t* elem, void* data)
{
  list_elem_t* new_elem = list_elem_init(data);

  if (elem->owner->root == elem)
    return list_push_front(elem->owner, new_elem);

  new_elem->owner = elem->owner;
  new_elem->prev = elem->prev;
  new_elem->next = elem;

  elem->prev = new_elem;

  ++elem->owner->len;
  
  return new_elem;
}

list_elem_t* list_insert_after(list_elem_t* elem, void* data)
{
  list_elem_t* new_elem = list_elem_init(data);

  if (elem->owner->tail == elem)
    return list_push_back(elem->owner, new_elem);

  new_elem->owner = elem->owner;
  new_elem->prev = elem;
  new_elem->next = elem->next;

  elem->next = new_elem;

  ++elem->owner->len;

  return new_elem;
}

void* list_remove(list_elem_t* elem)
{
  if (elem->owner->root == elem)
    return list_pop_front(elem->owner);

  if (elem->owner->tail == elem)
    return list_pop_back(elem->owner);

  elem->owner = NULL;
  elem->prev->next = elem->next;

  void* data = elem->data;

  list_elem_free(elem);

  return data;
}

void* list_remove_before(list_elem_t* elem)
{
  assert(list_elem_prev(elem) != NULL);
  return list_remove(elem->prev);
}

void* list_remove_after(list_elem_t* elem)
{
  assert(list_elem_next(elem) != NULL);
  return list_remove(elem->next);
}

bool list_empty(list_t* list)
{
  return list->len == 0;
}

size_t list_size(list_t* list)
{
  return list->len;
}
