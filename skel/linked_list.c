/*
Copyright (c) 2024 Nicolae-Cristian MACOVEI nicolae.macovei1412@stud.acs.upb.ro
*/

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
	// if (debug_info) {
	// 	if ((size_t)(((map_info_t *)data)->key) == 0x4bdc480)
	// 		fprintf(stderr, "incerci macar?\n");
	// }
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
		// if (debug_info == 1 && (size_t)(((map_info_t *)data)->key) == 0x4bdc480)
		// { 	map_info_t *info = data; 	if (strcmp(*(char **)(info->key),
		// "other_outside.txt") == 0) { 		fprintf(stderr, "Uite-l pe nemernic\n");
		// 	}
		// 	fprintf(stderr, "uite aici\n");
		// }
		if (list->tail) {
			// if (debug_info == 1 && (size_t)(((map_info_t *)data)->key) ==
			// 0x4bdc480) { 	fprintf(stderr, "Adresa tail: %p\n", list->tail);
			// }
			list->tail->next = new_node;
		}
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
		// if ((size_t)list->tail == 0x4d90be0) {
		// 	for (ll_node_t *node = list->head; node; node = node->next) {
		// 		fprintf(stderr, "node = %p\n", node);
		// 	}
		// 	fprintf(stderr, "Aici se intampla amuzanta\n");
		// }
		--(list->size);
		ll_node_t *ans = list->head;
		list->head = list->head->next;

		if (list->size == 0) {
			list->tail = NULL;
		}
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
		// if ((size_t)list->tail == 0x4d90be0) {
		// 	fprintf(stderr, "MUIE tail pe %p, ans pe %p\n", list->tail, ans);
		// }
	}
	--(list->size);
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
