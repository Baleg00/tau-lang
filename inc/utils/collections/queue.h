/**
 * \file
 * 
 * \brief Stack data structure interface.
 * 
 * \details A queue is a linear data structure that follows the
 * First-In-First-Out (FIFO) principle. It is similar to a queue of people
 * waiting in line, where the first person to join the queue is the first one
 * to be served. Elements can only be inserted at the back and removed from the
 * front of the queue. This ensures that the order of insertion is preserved.
 * Common queue operations include offer (adding an item to the back), poll
 * (removing the front item), and peek (viewing the front item without removing
 * it).
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_QUEUE_H
#define TAU_QUEUE_H

#include "utils/common.h"

TAU_EXTERN_C_BEGIN
/**
 * \brief Queue data structure.
 */
typedef struct queue_t queue_t;

/**
 * \brief Initializes a new empty queue.
 *
 * \returns Pointer to the initialized queue.
 */
queue_t* queue_init(void);

/**
 * \brief Frees the resources associated with a queue.
 *
 * \param[in] que Pointer to the queue to free.
 */
void queue_free(queue_t* que);

/**
 * \brief Adds the given data to the back of a queue.
 *
 * \param[in,out] que Pointer to the queue.
 * \param[in] data Pointer to the data to be added.
 */
void queue_offer(queue_t* que, void* data);

/**
 * \brief Removes and returns the data stored at the front of a queue.
 *
 * \param[in,out] que Pointer to the queue.
 * \returns Pointer to the data removed from the front of the queue.
 */
void* queue_poll(queue_t* que);

/**
 * \brief Retrieves the data stored at the front of a queue.
 *
 * \param[in] que Pointer to the queue.
 * \returns Pointer to the data at the front of the queue.
 */
void* queue_peek(const queue_t* que);

/**
 * \brief Checks if a queue is empty.
 *
 * \param[in] que Pointer to the queue.
 * \returns `true` if the queue is empty, `false` otherwise.
 */
bool queue_empty(const queue_t* que);

TAU_EXTERN_C_END

#endif
