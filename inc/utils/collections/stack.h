/**
 * \file
 * 
 * \brief Stack data structure interface.
 * 
 * \details A stack is a linear data structure that follows the
 * Last-In-First-Out (LIFO) principle. It is similar to a stack of items, where
 * the last item added is the first one to be removed. Elements can only be
 * inserted or removed from the top of the stack. Common stack operations
 * include push (adding an item to the top), pop (removing the top item), and
 * peek (viewing the top item without removing it).
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_STACK_H
#define TAU_STACK_H

#include "utils/common.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Stack data structure.
 */
typedef struct stack_t stack_t;

/**
 * \brief Initializes a new empty stack.
 *
 * \returns Pointer to the initialized stack.
 */
stack_t* stack_init(void);

/**
 * \brief Frees the resources associated with a stack.
 *
 * \param[in] stack Pointer to the stack to free.
 */
void stack_free(stack_t* stack);

/**
 * \brief Adds the given data to the top of a stack.
 *
 * \param[in,out] stack Pointer to the stack.
 * \param[in] data Pointer to the data to be added.
 */
void stack_push(stack_t* restrict stack, void* restrict data);

/**
 * \brief Removes and returns the data stored at the top of a stack.
 *
 * \param[in,out] stack Pointer to the stack.
 * \returns Pointer to the data removed from the top of the stack.
 */
void* stack_pop(stack_t* stack);

/**
 * \brief Removes all data from a stack.
 *
 * \param[in,out] stack Pointer to the stack to be cleared.
 */
void stack_clear(stack_t* stack);

/**
 * \brief Retrieves the data stored at the top of a stack.
 *
 * \param[in] stack Pointer to the stack.
 * \returns Pointer to the data at the top of the stack.
 */
void* stack_top(const stack_t* stack);

/**
 * \brief Checks if a stack is empty.
 *
 * \param[in] stack Pointer to the stack.
 * \returns `true` if the stack is empty, `false` otherwise.
 */
bool stack_empty(const stack_t* stack);

TAU_EXTERN_C_END

#endif
