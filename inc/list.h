/**
 * \file list.h
 * 
 * \brief Linked list data structure interface.
 * 
 * \details A linked list is a data structure that consists of a sequence of
 * nodes, where each node contains data and a reference to the next node in the
 * sequence. It provides a flexible way to store and manipulate data
 * dynamically. Linked lists are useful when the number of elements is not
 * known in advance or when frequent insertions and deletions are expected.
 * They allow efficient insertion and removal at any position in the list, but
 * do not support accessing elements by indices.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_LIST_H
#define TAU_LIST_H

#include <stdbool.h>
#include <stddef.h>

/**
 * \brief Function type to serve as parameter in `list_for_each`.
 */
typedef void(*list_for_each_func_t)(void*);

/**
 * \brief Node of a linked list.
 */
typedef struct list_node_s list_node_t;

/**
 * \brief Linked list data structure.
 */
typedef struct list_s list_t;

/**
 * \brief Macro for iterating over each element in a list using a for loop.
 *
 * \param[in] VAR The name of the variable to use for iteration.
 * \param[in] LIST The list to iterate over.
 */
#define LIST_FOR_LOOP(VAR, LIST)\
  for (\
    list_node_t* (VAR) = (LIST) == NULL ? NULL : list_front_node((LIST));\
    (VAR) != NULL;\
    (VAR) = list_node_next((VAR))\
  )

/**
 * \brief Initializes a list node with the given data.
 *
 * \param[in] data The data to be stored in the node.
 * \returns Pointer to the initialized list node.
 */
list_node_t* list_node_init(void* data);

/**
 * \brief Frees the resources associated with a list node.
 *
 * \param[in] node Pointer to the list node to free.
 */
void list_node_free(list_node_t* node);

/**
 * \brief Creates a copy of a list node.
 *
 * \param[in] node Pointer to the list node to copy.
 * \returns Pointer to the copied list node.
 */
list_node_t* list_node_copy(list_node_t* node);

/**
 * \brief Returns the previous node in the list.
 *
 * \param[in] node Pointer to the list node.
 * \returns Pointer to the previous list node.
 */
list_node_t* list_node_prev(list_node_t* node);

/**
 * \brief Returns the next node in the list.
 *
 * \param[in] node Pointer to the list node.
 * \returns Pointer to the next list node.
 */
list_node_t* list_node_next(list_node_t* node);

/**
 * \brief Retrieves the data stored in a list node.
 *
 * \param[in] node Pointer to the list node.
 * \returns Pointer to the data stored in the node.
 */
void* list_node_get(list_node_t* node);

/**
 * \brief Sets the data stored in a list node.
 *
 * \param[out] node Pointer to the list node.
 * \param[in] data Pointer to the data to be stored in the node.
 */
void list_node_set(list_node_t* node, void* data);

/**
 * \brief Initializes a new empty list.
 *
 * \returns Pointer to the initialized list.
 */
list_t* list_init(void);

/**
 * \brief Frees the resources associated with a list.
 *
 * \param[in] list Pointer to the list to free.
 */
void list_free(list_t* list);

/**
 * \brief Creates a copy of a list.
 *
 * \param[in] list Pointer to the list to copy.
 * \returns Pointer to the copied list.
 */
list_t* list_copy(list_t* list);

/**
 * \brief Retrieves the data stored in the front of a list.
 *
 * \param[in] list Pointer to the list.
 * \returns Pointer to the data at the front of the list.
 */
void* list_front(list_t* list);

/**
 * \brief Retrieves the data stored in the back of a list.
 *
 * \param[in] list Pointer to the list.
 * \returns Pointer to the data at the back of the list.
 */
void* list_back(list_t* list);

/**
 * \brief Retrieves the front node of a list.
 *
 * \param[in] list Pointer to the list.
 * \returns Pointer to the front node of the list.
 */
list_node_t* list_front_node(list_t* list);

/**
 * \brief Retrieves the back node of a list.
 *
 * \param[in] list Pointer to the list.
 * \returns Pointer to the back node of the list.
 */
list_node_t* list_back_node(list_t* list);

/**
 * \brief Adds a new node with the given data to the front of a list.
 *
 * \param[in,out] list Pointer to the list.
 * \param[in] data Pointer to the data to be added.
 * \returns Pointer to the newly added list node.
 */
list_node_t* list_push_front(list_t* list, void* data);

/**
 * \brief Adds a new node with the given data to the back of a list.
 *
 * \param[in,out] list Pointer to the list.
 * \param[in] data Pointer to the data to be added.
 * \returns Pointer to the newly added list node.
 */
list_node_t* list_push_back(list_t* list, void* data);

/**
 * \brief Removes and returns the data stored in the front of a list.
 *
 * \param[in,out] list Pointer to the list.
 * \returns Pointer to the data removed from the front of the list.
 */
void* list_pop_front(list_t* list);

/**
 * \brief Removes and returns the data stored in the back of a list.
 *
 * \param[in,out] list Pointer to the list.
 * \returns Pointer to the data removed from the back of the list.
 */
void* list_pop_back(list_t* list);

/**
 * \brief Inserts a new node with the given data before the specified node.
 *
 * \param[in] node Pointer to the node in the list.
 * \param[in] data Pointer to the data to be inserted.
 * \returns Pointer to the newly inserted list node.
 */
list_node_t* list_insert_before(list_node_t* node, void* data);

/**
 * \brief Inserts a new node with the given data after the specified node.
 *
 * \param[in] node Pointer to the node in the list.
 * \param[in] data Pointer to the data to be inserted.
 * \returns Pointer to the newly inserted list node.
 */
list_node_t* list_insert_after(list_node_t* node, void* data);

/**
 * \brief Removes a node from the list and returns the associated data.
 *
 * \param[in,out] node Pointer to the node to be removed.
 * \returns Pointer to the data stored in the removed node.
 */
void* list_remove(list_node_t* node);

/**
 * \brief Removes the node before the specified node and returns the associated
 * data.
 *
 * \param[in] node Pointer to the node in the list.
 * \returns Pointer to the data stored in the removed node.
 */
void* list_remove_before(list_node_t* node);

/**
 * \brief Removes the node after the specified node and returns the associated
 * data.
 *
 * \param[in] node Pointer to the node in the list.
 * \returns Pointer to the data stored in the removed node.
 */
void* list_remove_after(list_node_t* node);

/**
 * \brief Checks if a list is empty.
 *
 * \param[in] list Pointer to the list.
 * \returns `true` if the list is empty, `false` otherwise.
 */
bool list_empty(list_t* list);

/**
 * \brief Returns the size of a list.
 *
 * \param[in] list Pointer to the list.
 * \returns The number of elements in the list.
 */
size_t list_size(list_t* list);

/**
 * \brief Applies a given function to each element in the list.
 *
 * \param[in] list Pointer to the list.
 * \param[in] func The function to be applied to each element.
 */
void list_for_each(list_t* list, list_for_each_func_t func);

#endif