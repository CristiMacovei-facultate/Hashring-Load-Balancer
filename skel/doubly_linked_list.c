#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doubly_linked_list.h"

dll_t *dll_init(unsigned int data_size)
{
	dll_t *dll = malloc(sizeof(dll_t));
	dll->data_size = data_size;
	dll->size = 0;
	dll->head = NULL;
	dll->tail = NULL;
	return dll;
}

dll_node_t *dll_get_nth_node(dll_t *list, unsigned int n)
{
	if (!list || list->size == 0) {
		return NULL;
	}

	dll_node_t *node = list->head;
	for (int i = 0; i < (int)n && node; ++i) {
		node = node->next;
	}
	return node;
}

dll_node_t *dll_remove_nth_node(dll_t *list, unsigned int n);

void dll_insert_nth(dll_t *list, unsigned int n, dll_node_t *new_node)
{
	if (n > list->size) {
		n = list->size;
	}

	if (n == 0) {
		new_node->next = list->head;
		new_node->prev = NULL;

		if (list->head) {
			list->head->prev = new_node;
		}
		list->head = new_node;

		if (list->size == 0) {
			list->tail = new_node;
		}

		++(list->size);
		return;
	}

	dll_node_t *node = list->head;
	for (int i = 0; i < (int)n - 1; ++i) {
		node = node->next;
	}
	new_node->next = node->next;
	new_node->prev = node;

	if (node->next) {
		node->next->prev = new_node;
	}
	else {
		list->tail = new_node;
	}
	node->next = new_node;

	++(list->size);
}

void dll_remove_node(dll_t *list, dll_node_t *node)
{
	dll_node_t *prev = node->prev;
	dll_node_t *next = node->next;

	if (prev) {
		prev->next = next;
	}
	else {
		list->head = next;
	}

	if (next) {
		next->prev = prev;
	}
	else {
		list->tail = prev;
	}
}

dll_node_t *dll_insert_nth_node(dll_t *list, unsigned int n, void *data)
{
	dll_node_t *new_node = malloc(sizeof(dll_node_t));
	new_node->data = malloc(list->data_size);
	memcpy(new_node->data, data, list->data_size);
	dll_insert_nth(list, n, new_node);
	return new_node;
}

void dll_free(dll_t *list)
{
	for (dll_node_t *node = list->head; node;) {
		dll_node_t *aux = node->next;

		free(node->data);
		free(node);

		node = aux;
	}
	free(list);
}