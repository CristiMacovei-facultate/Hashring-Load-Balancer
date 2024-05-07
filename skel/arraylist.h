/*
Copyright (c) 2024 Nicolae-Cristian MACOVEI nicolae.macovei1412@stud.acs.upb.ro
*/

#ifndef ARRAYLIST_H_GUARD
#define ARRAYLIST_H_GUARD 0x69

/**
 * @brief Ordered arraylist structure implementation
 */
typedef struct {
	unsigned int data_size;
	unsigned int size;
	unsigned int capacity;
	void (*destructor)(void *);

	void **data;
} arraylist_t;

/**
 * @brief Function to initialise the arraylist
 *
 * @param data_size - size of each element in the arraylist
 * @param destructor - destructor for each element
 * @return arraylist_t* - the list we just created
 */
arraylist_t *al_init(unsigned int data_size, void(destructor)(void *));

/**
 * @brief Function to get an element at some specific index
 *
 * @param list - self-explanatory
 * @param index - also self-explanatory
 * @return void*
 */
void *al_get(arraylist_t *list, unsigned int index);

/**
 * @brief Function to get the first element "greater" than some target
 *
 * @param list
 * @param target - the target we're comparing to
 * @param compare - comparator function
 * @return void* - the element found, or element at index zero if there's none
 */
void *al_get_ordered(arraylist_t *list, void *target,
										 int (*compare)(void *element, void *target));

/**
 * @brief Function to find one specific element in the list.<br>
 * This is used when there's multiple elements in the sorted list with the same
 * key, but different (should be unique) "target" fields (in our case multiple
 * servers with the same hash, but different ids)
 * @param list
 * @param bs_key - sorting key of the element
 * @param target - unique identifier of the element
 * @param bs_compare - comparator for the sorting key
 * @param compare - comparator for unique identifier
 * @return int - index of the element found
 */
int al_find_by(arraylist_t *list, void *bs_key, void *target,
							 int (*bs_compare)(void *element, void *target),
							 int (*compare)(void *element, void *target));

/**
 * @brief Function to delete the element at a given index from the arraylist
 *
 * @param list
 * @param index
 */
void al_erase(arraylist_t *list, unsigned int index);

/**
 * @brief Function to insert an element at a given index in the list
 *
 * @param list
 * @param index
 * @param data - pointer to the element (has to be freed by the caller)
 */
void al_insert(arraylist_t *list, unsigned int index, void *data);

/**
 * @brief Function to insert an element before the first element that's greater
 * than it
 *
 * @param list
 * @param data - pointer to the element data (has to be freed by the caller)
 * @param compare - comparator function
 * @return int - the index where the element was inserted
 */
int al_insert_ordered(arraylist_t *list, void *data,
											int (*compare)(void *data1, void *data2));

/**
 * @brief Function to free the list and all elements inside it. This uses the
 * function that's been passed as destructor
 *
 * @param list
 */
void al_free(arraylist_t *list);

/**
 * @brief Function used solely for debug purposes. Please don't judge
 *
 * @param list
 */
void print_servers(arraylist_t *list);

#endif
