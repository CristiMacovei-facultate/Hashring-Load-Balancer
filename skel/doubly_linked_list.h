#ifndef DOUBLY_LINKED_LIST_H_GUARD
#define DOUBLY_LINKED_LIST_H_GUARD 0x69

typedef struct dll_node {
	struct dll_node *next;
	struct dll_node *prev;

	void *data;
} dll_node_t;

typedef struct {
	dll_node_t *head;
	dll_node_t *tail;

	unsigned int size;
	unsigned int data_size;
} dll_t;

dll_t *dll_init(unsigned int data_size);

dll_node_t *dll_get_nth_node(dll_t *list, unsigned int n);

dll_node_t *dll_insert_nth_node(dll_t *list, unsigned int n, void *data);

dll_node_t *dll_remove_nth_node(dll_t *list, unsigned int n);

void dll_insert_nth(dll_t *list, unsigned int n, dll_node_t *node);

void dll_remove_node(dll_t *list, dll_node_t *node);

#endif