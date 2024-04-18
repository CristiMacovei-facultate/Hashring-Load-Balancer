#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

ll_t *ll_init(unsigned int data_size)
{
	ll_t *ll = malloc(sizeof(ll_t));
	ll->head = NULL;
	ll->tail = NULL;
	ll->data_size = data_size;
	ll->size = 0;
	return ll;
}

ll_node_t *ll_get_nth_node(ll_t *list, unsigned int n)
{
	if (!list || list->size == 0) {
		return NULL;
	}

	ll_node_t *node = list->head;
	for (int i = 0; i < (int)n && node; ++i) {
		node = node->next;
	}
	return node;
}

void ll_insert_nth(ll_t *list, unsigned int n, void *data)
{
	if (!list) {
		return;
	}

	if (n > list->size) {
		n = list->size;
	}

	ll_node_t *new_node = malloc(sizeof(ll_node_t));
	new_node->next = NULL;
	new_node->data = malloc(list->data_size);
	// printf("newnode.data = %p\n", new_node->data);
	memcpy(new_node->data, data, list->data_size);

	// if we insert on first pos
	if (n == 0) {
		new_node->next = list->head;
		list->head = new_node;

		if (list->size == 0) {
			list->tail = new_node;
		}
		++(list->size);

		return;
	}

	// if we insert on last pos
	if (n == list->size) {
		list->tail->next = new_node;
		list->tail = new_node;
		++(list->size);
		return;
	}

	// any other pos
	ll_node_t *node = list->head;
	for (int i = 0; i < (int)n - 1; ++i) {
		node = node->next;
	}

	new_node->next = node->next;
	node->next = new_node;
	++(list->size);
}

ll_node_t *ll_remove_nth_node(ll_t *list, unsigned int n)
{
	if (!list) {
		return NULL;
	}

	if (n >= list->size) {
		n = list->size - 1;
	}

	if (n == 0) {
		ll_node_t *ans = list->head;
		list->head = list->head->next;

		if (list->size == 0) {
			list->tail = NULL;
		}
		--(list->size);
		return ans;
	}

	ll_node_t *node = list->head;
	for (int i = 0; i < (int)n - 1; ++i) {
		node = node->next;
	}

	ll_node_t *ans = node->next;
	node->next = node->next ? node->next->next : NULL;

	if (n == list->size - 1) {
		list->tail = node;
	}

	return ans;
}

void ll_free(ll_t *list)
{
	for (ll_node_t *node = list->head; node;) {
		ll_node_t *aux = node->next;

		free(node->data);
		free(node);

		node = aux;
	}
	free(list);
}