#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "linked_list.h"
#include "queue.h"
#include "server.h"

queue_t *q_init(unsigned int data_size)
{
	queue_t *q = malloc(sizeof(queue_t));
	q->data = ll_init(data_size);
	return q;
}

ll_node_t *q_pop(queue_t *q)
{
	if (q->data->size == 0) {
		return NULL;
	}

	return ll_remove_nth_node(q->data, 0);
}

void q_push(queue_t *q, void *data)
{
	if (q->data->size == TASK_QUEUE_SIZE) {
		fprintf(stderr, "NU stiu ce vreti de la mine aici");
		exit(-1);
	}

	request *debug = data;
	printf("Inserez %s\n", debug->doc_name);
	ll_insert_nth(q->data, q->data->size, data);
}