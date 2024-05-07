/*
Copyright (c) 2024 Nicolae-Cristian MACOVEI nicolae.macovei1412@stud.acs.upb.ro
*/

#ifndef DOUBLY_LINKED_LIST_H_GUARD
#define DOUBLY_LINKED_LIST_H_GUARD 0x69

/**
 * @brief structure for the node of a doubly linked list
 */
typedef struct dll_node {
	struct dll_node *next;
	struct dll_node *prev;

	void *data;
} dll_node_t;

/**
 * @brief structure for the doubly linked list itself
 * size - the number of nodes in the list
 * data_size - size of each node's data (the same for all nodes)
 * destructor - function to free the data of each node
 */
typedef struct {
	dll_node_t *head;
	dll_node_t *tail;

	unsigned int size;
	unsigned int data_size;

	void (*destructor)(void *);
} dll_t;

/**
 * @brief Function to initialise a doubly linked list
 *
 * @param data_size - size of each node's data
 * @param destructor - function to free data stored in the linked list
 * @return dll_t*
 */
dll_t *dll_init(unsigned int data_size, void (*destructor)(void *));

/**
 * @brief Function to get the node at a given index
 *
 * @param list
 * @param n - the index (starts at zero)
 * @return dll_node_t* - the answer
 */
dll_node_t *dll_get_nth_node(dll_t *list, unsigned int n);

/**
 * @brief Function to insert a node in the linked list on a given index
 *
 * @param list
 * @param n - the index we want to insert at
 * @param data - a pointer to the new node's data (size must be equal to the
 * list's data_size)
 * @return dll_node_t* - the node we just inserted
 */
dll_node_t *dll_insert_nth_node(dll_t *list, unsigned int n, void *data);

/**
 * @brief Function to insert a node in the linked list on a given index
 * It doesn't free the removed node's data, that's supposed to be done by the
 * caller
 * @param list
 * @param n - the index we want to remove from
 * @return dll_node_t* - the node we just removed
 */
dll_node_t *dll_remove_nth_node(dll_t *list, unsigned int n);

/**
 * @brief Inserts a node directly, rather than copying the data and creating a
 * new node
 *
 * @param list
 * @param n
 * @param node
 */
void dll_insert_nth(dll_t *list, unsigned int n, dll_node_t *node);

/**
 * @brief Removes a node directly. This is significantly faster than finding the
 * index and calling remove_nth
 *
 * @param list
 * @param node
 */
void dll_remove_node(dll_t *list, dll_node_t *node);

/**
 * @brief Frees all of the data in the list
 *
 * @param list
 */
void dll_free(dll_t *list);

#endif
