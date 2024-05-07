/*
Copyright (c) 2024 Nicolae-Cristian MACOVEI nicolae.macovei1412@stud.acs.upb.ro
*/

#ifndef LINKED_LIST_H_GUARD
#define LINKED_LIST_H_GUARD 0x69

/**
 * @brief Structure to represent a singly linked list node
 */
typedef struct ll_node {
	struct ll_node *next;
	void *data;
} ll_node_t;

/**
 * @brief Structure to represent the singly linked list
 */
typedef struct {
	ll_node_t *head;
	ll_node_t *tail;
	unsigned int size;
	unsigned int data_size;
} ll_t;

/**
 * @brief Function to initialise a singly linked list
 *
 * @param data_size - size of each node's data
 * @return ll_t* - the list we just created
 */
ll_t *ll_init(unsigned int data_size);

/**
 * @brief Function to get the node at a specific index
 *
 * @param list
 * @param n - the index we want (starts from 0)
 * @return ll_node_t* - the node we found
 */
ll_node_t *ll_get_nth_node(ll_t *list, unsigned int n);

/**
 * @brief Function to insert a new node on a specific index
 *
 * @param list
 * @param n
 * @param data
 */
void ll_insert_nth(ll_t *list, unsigned int n, void *data);

/**
 * @brief Function to remove a node from a specific index
 *
 * @param list
 * @param n
 * @return ll_node_t* - the node we just removed, which should be freed by the
 * caller
 */
ll_node_t *ll_remove_nth_node(ll_t *list, unsigned int n);

/**
 * @brief Frees al the memory used by the linked list
 *
 * @param list
 */
void ll_free(ll_t *list);

#endif
