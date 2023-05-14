/**
 * \file
 * 
 * Queue data structure.
 * 
 * Queue is just a wrapper over a doubly-linked list.
*/

#ifndef TAU_QUEUE_H
#define TAU_QUEUE_H

#include <stdbool.h>

#include "typedefs.h"

/**
 * \brief Initializes a new queue.
 * 
 * \returns New queue.
*/
queue_t* queue_init(void);

/**
 * \brief Destroys a queue.
 * 
 * \param[in] que Queue to be destroyed.
*/
void queue_free(queue_t* que);

/**
 * \brief Pushes a data pointer into the queue.
 * 
 * \param[in] que Queue to be pushed into.
 * \param[in] data Pointer to be pushed into the queue.
*/
void queue_offer(queue_t* que, void* data);

/**
 * \brief Polls a pointer from the queue.
 * 
 * \details The function fails if the provided queue is empty.
 * 
 * \param[in] que Queue to be polled from.
 * \returns Pointer polled from the queue.
*/
void* queue_poll(queue_t* que);

/**
 * \brief Peeks the last pointer in the queue.
 * 
 * \details The function fails if the provided queue is empty.
 * 
 * \param[in] que Queue to be peeked.
 * \returns Last pointer in the queue.
*/
void* queue_peek(queue_t* que);

/**
 * \brief Checks if the queue has no items.
 * 
 * \param[in] que Queue to be checked.
 * \returns True if queue is empty, false otherwise.
*/
bool queue_empty(queue_t* que);

#endif