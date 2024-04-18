#ifndef LINKED_LIST_H_GUARD
#define LINKED_LIST_H_GUARD 0x69

typedef struct ll_node {
	struct ll_node *next;
	void *data;
} ll_node_t;

typedef struct {
	ll_node_t *head;
	ll_node_t *tail;
	unsigned int size;
	unsigned int data_size;
} ll_t;

ll_t *ll_init(unsigned int data_size);

ll_node_t *ll_get_nth_node(ll_t *list, unsigned int n);

void ll_insert_nth(ll_t *list, unsigned int n, void *data);

ll_node_t *ll_remove_nth_node(ll_t *list, unsigned int n);

void ll_free(ll_t *list);

#endif