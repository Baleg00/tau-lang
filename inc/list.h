/**
 * \file
 * 
 * Doubly-linked list data structure.
*/

#ifndef TAU_LIST_H
#define TAU_LIST_H

#include <stddef.h>
#include <stdbool.h>

/** Represents an node in a linked list. */
typedef struct list_node_s list_node_t;

/** Represents a linked list. */
typedef struct list_s list_t;

/** Function pointer type to be used by `list_for_each` function. */
typedef void(*list_for_each_func_t)(void*);

struct list_node_s
{
  void* data; // Pointer to the stored data.
  list_t* owner; // Pointer to linked list which this node belongs to.
  list_node_t* prev; // Pointer to previous node or null pointer.
  list_node_t* next; // Pointer to next node or null pointer.
};

struct list_s
{
  list_node_t* head; // Pointer to first node in list or null pointer.
  list_node_t* tail; // Pointer to last node in list or null pointer.
  size_t len; // Number of nodes in list.
};

/** Utility macro iterating over a list. */
#define LIST_FOR_LOOP(VAR, LIST)\
  for (list_node_t* (VAR) = list_front_node((LIST)); (VAR) != NULL; (VAR) = list_node_next((VAR)))

/**
 * \brief Initializes a new node.
 * 
 * \details `data` may be a null pointer. You can get and set this pointer
 * using the `list_node_get` and `list_node_set` functions.
 * 
 * \param[in] data Pointer to be stored by the node.
 * \returns New node.
*/
list_node_t* list_node_init(void* data);

/**
 * \brief Destroys a node.
 * 
 * \details The pointer stored by the node is left untouched.
 * 
 * \param[in] node Node to be destroyed.
*/
void list_node_free(list_node_t* node);

/**
 * \brief Creates a copy of a node.
 * 
 * \param[in] node Node to be copied.
 * \returns New node.
*/
list_node_t* list_node_copy(list_node_t* node);

/**
 * \brief Returns the previous node or null pointer if there is no previous node.
 * 
 * \param[in] node Node whose predecessor is to be returned.
 * \returns Previous node or null pointer.
*/
list_node_t* list_node_prev(list_node_t* node);

/**
 * \brief Returns the next node or null pointer if there is no next node.
 * 
 * \param[in] node Node whose successor is to be returned.
 * \returns Next node or null pointer.
*/
list_node_t* list_node_next(list_node_t* node);

/**
 * \brief Returns pointer stored by node.
 * 
 * \param[in] node Node whose stored pointer is to be returned.
 * \returns Stored pointer.
*/
void* list_node_get(list_node_t* node);

/**
 * \brief Overwrites pointer stored by node.
 * 
 * \param[in] node Node whose stored pointer is to be overwritten.
 * \param[in] data Pointer to be stored by node.
*/
void list_node_set(list_node_t* node, void* data);

/**
 * \brief Initializes a new linked list.
 * 
 * \returns New list.
*/
list_t* list_init(void);

/**
 * \brief Destroys a linked list.
 * 
 * \details The list itself and all of its nodes will be destroyed.
 * 
 * \param[in] list List to be destroyed.
*/
void list_free(list_t* list);

/**
 * \brief Creates a copy of a linked list.
 * 
 * \details The retuned list and its nodes are completely new instances.
 * 
 * \param[in] list List to be copied.
 * \returns New list.
*/
list_t* list_copy(list_t* list);

/**
 * \brief Returns the pointer stored by the first node in the list.
 * 
 * \details The function fails if the provided list is empty.
 * 
 * \param[in] list List to be used.
 * \returns Pointer stored by the first node.
*/
void* list_front(list_t* list);

/**
 * \brief Returns the pointer stored by the last node in the list.
 * 
 * \details The function fails if the provided list is empty.
 * 
 * \param[in] list List to be used.
 * \returns Pointer stored by the last node.
*/
void* list_back(list_t* list);

/**
 * \brief Returns the first node in the list or null pointer if the list is empty.
 * 
 * \param[in] list List to be used.
 * \returns First node.
*/
list_node_t* list_front_node(list_t* list);

/**
 * \brief Returns the last node in the list or null pointer if the list is empty.
 * 
 * \param[in] list List to be used.
 * \returns Last node.
*/
list_node_t* list_back_node(list_t* list);

/**
 * \brief Appends a new node to the beginning of the list. The node will store
 * the provided data pointer.
 * 
 * \param[in] list List to be appended to.
 * \param[in] data Pointer to be stored by appended node.
 * \returns Appended node.
*/
list_node_t* list_push_front(list_t* list, void* data);

/**
 * \brief Appends a new node to the end of the list. The node will store the
 * provided data pointer.
 * 
 * \param[in] list List to be appended to.
 * \param[in] data Pointer to be stored by appended node.
 * \returns Appended node.
*/
list_node_t* list_push_back(list_t* list, void* data);

/**
 * \brief Removes the first node from the list and returns the stored pointer.
 * 
 * \details The function fails if the provided list is empty.
 * 
 * \param[in] list List whose first node is to be removed.
 * \returns Pointer stored by removed node.
*/
void* list_pop_front(list_t* list);

/**
 * \brief Removes the last node from the list and returns the stored pointer.
 * 
 * \details The function fails if the provided list is empty.
 * 
 * \param[in] list List whose last node is to be removed.
 * \returns Pointer stored by removed node.
*/
void* list_pop_back(list_t* list);

/**
 * \brief Inserts a new node before another one. The node will store the
 * provided data pointer.
 * 
 * \param[in] node Node before which a new one will be inserted.
 * \param[in] data Pointer to be stored by inserted node.
 * \returns Inserted node.
*/
list_node_t* list_insert_before(list_node_t* node, void* data);

/**
 * \brief Inserts a new node after another one. The node will store the
 * provided data pointer.
 * 
 * \param[in] node Node after which a new one will be inserted.
 * \param[in] data Pointer to be stored by inserted node.
 * \returns Inserted node.
*/
list_node_t* list_insert_after(list_node_t* node, void* data);

/**
 * \brief Removes node from list and returns the stored pointer.
 * 
 * \param[in] node Node to be removed.
 * \returns Pointer stored by removed node.
*/
void* list_remove(list_node_t* node);

/**
 * \brief Removes node before another one and returns the stored pointer.
 * 
 * \details The function fails if there is no previous node.
 * 
 * \param[in] node Node whose predecessor is to be removed.
 * \returns Pointer stored by removed node.
*/
void* list_remove_before(list_node_t* node);

/**
 * \brief Removes node before another one and returns the stored pointer.
 * 
 * \details The function fails if there is no next node.
 * 
 * \param[in] node Node whose successor is to be removed.
 * \returns Pointer stored by removed node.
*/
void* list_remove_after(list_node_t* node);

/**
 * \brief Checks if the list has no nodes.
 * 
 * \param[in] list List to be checked.
 * \returns True if list is empty, false otherwise.
*/
bool list_empty(list_t* list);

/**
 * \brief Returns the number of nodes in a list.
 * 
 * \param[in] list List to be used.
 * \returns Number of nodes in list.
*/
size_t list_size(list_t* list);

/**
 * \brief Applies the provided function for each data pointer stored in a list.
 * 
 * \details The function is called for each node's stored data pointer.
 * 
 * \param[in] list List whose nodes are to be iterated over.
 * \param[in] func Function to be called for each stored pointer.
*/
void list_for_each(list_t* list, list_for_each_func_t func);

#endif