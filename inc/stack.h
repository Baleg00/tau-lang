/**
 * \file
 * 
 * Stack data structure. 
 * 
 * Stack is just a wrapper over a doubly-linked list.
*/

#ifndef TAU_STACK_H
#define TAU_STACK_H

#include <stdbool.h>

#include "list.h"

/** Represents a stack. */
typedef list_t stack_t;

/**
 * \brief Initializes a new stack.
 * 
 * \returns New stack.
*/
stack_t* stack_init(void);

/**
 * \brief Destroys a stack.
 * 
 * \param[in] stack Stack to be destroyed.
*/
void stack_free(stack_t* stack);

/**
 * \brief Pushes a data pointer onto the stack.
 * 
 * \param[in] stack Stack to be pushed onto.
 * \param[in] data Pointer to be pushed onto stack.
*/
void stack_push(stack_t* stack, void* data);

/**
 * \brief Pops and returns pointer from the stack.
 * 
 * \details The function fails if the provided stack is empty.
 * 
 * \param[in] stack Stack to be popped from.
 * \returns Pointer popped from the stack.
*/
void* stack_pop(stack_t* stack);

/**
 * \brief Returns top pointer from the stack.
 * 
 * \details The function fails if the provided stack is empty.
 * 
 * \param[in] stack Stack whose top item is to be returned.
 * \returns Top pointer.
*/
void* stack_top(stack_t* stack);

/**
 * \brief Checks if the stack has no items.
 * 
 * \param[in] stack Stack to be checked.
 * \returns True if stack is empty, false otherwise.
*/
bool stack_empty(stack_t* stack);

#endif