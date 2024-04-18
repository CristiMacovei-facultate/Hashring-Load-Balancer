#ifndef QUEUE_H_GUARD
#define QUEUE_H_GUARD 0x69

#include <stdlib.h>

#include "linked_list.h"

typedef struct {
	ll_t *data;
} queue_t;

queue_t *q_init(unsigned int data_size);

ll_node_t *q_pop(queue_t *q);

void q_push(queue_t *q, void *data);

void q_free(queue_t *q);

#endif