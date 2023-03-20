#ifndef TAU_LIST_H
#define TAU_LIST_H

#include <stddef.h>
#include <stdbool.h>

typedef struct list_elem_s list_elem_t;
typedef struct list_s list_t;

struct list_elem_s
{
  void* data;
  list_t* owner;
  list_elem_t *prev, *next;
};

struct list_s
{
  list_elem_t *head, *tail;
  size_t len;
};

#define LIST_FOR_EACH(VAR, LIST)\
  for (list_elem_t* (VAR) = list_front_elem((LIST)); (VAR) != NULL; (VAR) = list_elem_next((VAR)))

list_elem_t* list_elem_init(void* data);
void list_elem_free(list_elem_t* elem);
list_elem_t* list_elem_copy(list_elem_t* elem);

list_elem_t* list_elem_prev(list_elem_t* elem);
list_elem_t* list_elem_next(list_elem_t* elem);
void* list_elem_get(list_elem_t* elem);
void list_elem_set(list_elem_t* elem, void* data);

list_t* list_init(void);
void list_free(list_t* list);
list_t* list_copy(list_t* list);

void* list_front(list_t* list);
void* list_back(list_t* list);

list_elem_t* list_front_elem(list_t* list);
list_elem_t* list_back_elem(list_t* list);

list_elem_t* list_push_front(list_t* list, void* data);
list_elem_t* list_push_back(list_t* list, void* data);

void* list_pop_front(list_t* list);
void* list_pop_back(list_t* list);

list_elem_t* list_insert_before(list_elem_t* elem, void* data);
list_elem_t* list_insert_after(list_elem_t* elem, void* data);

void* list_remove(list_elem_t* elem);
void* list_remove_before(list_elem_t* elem);
void* list_remove_after(list_elem_t* elem);

bool list_empty(list_t* list);

size_t list_size(list_t* list);

void list_for_each(list_t* list, void(*func)(void*));

#endif