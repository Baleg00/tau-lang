/**
 * \file
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

#include "utils/common.h"

/**
 * \brief Macro for iterating over each element in a list using a for loop.
 *
 * \param[in] VAR The name of the variable to use for iteration.
 * \param[in] LIST The list to iterate over.
 */
#define TAU_LIST_FOR_LOOP(VAR, LIST)\
  for (\
    tau_list_node_t* (VAR) = (LIST) == NULL ? NULL : tau_list_front_node((LIST));\
    (VAR) != NULL;\
    (VAR) = tau_list_node_next((VAR))\
  )

TAU_EXTERN_C_BEGIN

/// Utility function pointer type for `tau_list_for_each` function.
typedef void(*tau_list_for_each_func_t)(void*);

/// Represents a node in a linked list.
typedef struct tau_list_node_t tau_list_node_t;

/// Represents a linked list.
typedef struct tau_list_t tau_list_t;

/**
 * \brief Returns the previous node in the list.
 *
 * \param[in] node Pointer to the list node.
 * \returns Pointer to the previous list node.
 */
tau_list_node_t* tau_list_node_prev(tau_list_node_t* node);

/**
 * \brief Returns the next node in the list.
 *
 * \param[in] node Pointer to the list node.
 * \returns Pointer to the next list node.
 */
tau_list_node_t* tau_list_node_next(tau_list_node_t* node);

/**
 * \brief Retrieves the data stored in a list node.
 *
 * \param[in] node Pointer to the list node.
 * \returns Pointer to the data stored in the node.
 */
void* tau_list_node_get(const tau_list_node_t* node);

/**
 * \brief Sets the data stored in a list node.
 *
 * \param[out] node Pointer to the list node.
 * \param[in] data Pointer to the data to be stored in the node.
 */
void tau_list_node_set(tau_list_node_t* restrict node, void* restrict data);

/**
 * \brief Initializes a new empty list.
 *
 * \returns Pointer to the initialized list.
 */
tau_list_t* tau_list_init(void);

/**
 * \brief Initializes a new list from a memory buffer.
 *
 * \param[in] buffer Pointer to the memory buffer.
 * \param[in] length The number of elements in the buffer.
 * \returns Pointer to the initialized list.
 */
tau_list_t* tau_list_init_from_buffer(const void* buffer, size_t length);

/**
 * \brief Frees the resources associated with a list.
 *
 * \param[in] list Pointer to the list to free.
 */
void tau_list_free(tau_list_t* list);

/**
 * \brief Creates a copy of a list.
 *
 * \param[in] list Pointer to the list to copy.
 * \returns Pointer to the copied list.
 */
tau_list_t* tau_list_copy(const tau_list_t* list);

/**
 * \brief Retrieves the data stored in the front of a list.
 *
 * \param[in] list Pointer to the list.
 * \returns Pointer to the data at the front of the list.
 */
void* tau_list_front(const tau_list_t* list);

/**
 * \brief Retrieves the data stored in the back of a list.
 *
 * \param[in] list Pointer to the list.
 * \returns Pointer to the data at the back of the list.
 */
void* tau_list_back(const tau_list_t* list);

/**
 * \brief Retrieves the front node of a list.
 *
 * \param[in] list Pointer to the list.
 * \returns Pointer to the front node of the list.
 */
tau_list_node_t* tau_list_front_node(const tau_list_t* list);

/**
 * \brief Retrieves the back node of a list.
 *
 * \param[in] list Pointer to the list.
 * \returns Pointer to the back node of the list.
 */
tau_list_node_t* tau_list_back_node(const tau_list_t* list);

/**
 * \brief Adds a new node with the given data to the front of a list.
 *
 * \param[in,out] list Pointer to the list.
 * \param[in] data Pointer to the data to be added.
 * \returns Pointer to the newly added list node.
 */
tau_list_node_t* tau_list_push_front(tau_list_t* restrict list, void* restrict data);

/**
 * \brief Adds a new node with the given data to the back of a list.
 *
 * \param[in,out] list Pointer to the list.
 * \param[in] data Pointer to the data to be added.
 * \returns Pointer to the newly added list node.
 */
tau_list_node_t* tau_list_push_back(tau_list_t* restrict list, void* restrict data);

/**
 * \brief Removes and returns the data stored in the front of a list.
 *
 * \param[in,out] list Pointer to the list.
 * \returns Pointer to the data removed from the front of the list.
 */
void* tau_list_pop_front(tau_list_t* list);

/**
 * \brief Removes and returns the data stored in the back of a list.
 *
 * \param[in,out] list Pointer to the list.
 * \returns Pointer to the data removed from the back of the list.
 */
void* tau_list_pop_back(tau_list_t* list);

/**
 * \brief Inserts a new node with the given data before the specified node.
 *
 * \param[in] node Pointer to the node in the list.
 * \param[in] data Pointer to the data to be inserted.
 * \returns Pointer to the newly inserted list node.
 */
tau_list_node_t* tau_list_insert_before(tau_list_node_t* restrict node, void* restrict data);

/**
 * \brief Inserts a new node with the given data after the specified node.
 *
 * \param[in] node Pointer to the node in the list.
 * \param[in] data Pointer to the data to be inserted.
 * \returns Pointer to the newly inserted list node.
 */
tau_list_node_t* tau_list_insert_after(tau_list_node_t* restrict node, void* restrict data);

/**
 * \brief Removes a node from the list and returns the associated data.
 *
 * \param[in,out] node Pointer to the node to be removed.
 * \returns Pointer to the data stored in the removed node.
 */
void* tau_list_remove(tau_list_node_t* node);

/**
 * \brief Removes the node before the specified node and returns the associated
 * data.
 *
 * \param[in] node Pointer to the node in the list.
 * \returns Pointer to the data stored in the removed node.
 */
void* tau_list_remove_before(tau_list_node_t* node);

/**
 * \brief Removes the node after the specified node and returns the associated
 * data.
 *
 * \param[in] node Pointer to the node in the list.
 * \returns Pointer to the data stored in the removed node.
 */
void* tau_list_remove_after(tau_list_node_t* node);

/**
 * \brief Removes all elements from a list.
 *
 * \param[in,out] list Pointer to the list to be cleared.
 */
void tau_list_clear(tau_list_t* list);

/**
 * \brief Checks if a list is empty.
 *
 * \param[in] list Pointer to the list.
 * \returns `true` if the list is empty, `false` otherwise.
 */
bool tau_list_empty(const tau_list_t* list);

/**
 * \brief Returns the size of a list.
 *
 * \param[in] list Pointer to the list.
 * \returns The number of elements in the list.
 */
size_t tau_list_size(const tau_list_t* list);

/**
 * \brief Applies a given function to each element in the list.
 *
 * \param[in] list Pointer to the list.
 * \param[in] func The function to be applied to each element.
 */
void tau_list_for_each(tau_list_t* list, tau_list_for_each_func_t func);

/**
 * \brief Writes every element of a list into a memory buffer.
 * 
 * \note The memory buffer must be large enough to hold all elements, otherwise
 * a buffer overflow will occur.
 * 
 * \param list Pointer to the list.
 * \param buffer Pointer to the memory buffer.
 */
void tau_list_to_buffer(const tau_list_t* restrict list, void* restrict buffer);

TAU_EXTERN_C_END

#endif
