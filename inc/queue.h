#ifndef TAU_QUEUE_H
#define TAU_QUEUE_H

#include <stdbool.h>

#include "forward_decls.h"

queue_t* queue_init(void);
void queue_free(queue_t* que);

void queue_offer(queue_t* que, void* data);
void* queue_poll(queue_t* que);
void* queue_peek(queue_t* que);

bool queue_empty(queue_t* que);

#endif