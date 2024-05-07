/*
Copyright (c) 2024 Nicolae-Cristian MACOVEI nicolae.macovei1412@stud.acs.upb.ro
*/

#ifndef QUEUE_H_GUARD
#define QUEUE_H_GUARD 0x69

#include <stdlib.h>

#include "linked_list.h"

typedef struct {
	ll_t *data;
} queue_t;

/**
 * @brief Function to initialise a queue
 *
 * @param data_size - size of each element's data
 * @return queue_t* - the queue we created
 */
queue_t *q_init(unsigned int data_size);

/**
 * @brief Function to remove the first element from the queue
 *
 * @param q
 * @return ll_node_t* - the element we got (should be freed by the caller)
 */
ll_node_t *q_pop(queue_t *q);

/**
 * @brief Function to add a new element to the end of the queue
 *
 * @param q
 * @param data
 */
void q_push(queue_t *q, void *data);

/**
 * @brief Frees all memory used by the queue
 *
 * @param q
 */
void q_free(queue_t *q);

#endif
