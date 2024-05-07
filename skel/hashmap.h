/*
Copyright (c) 2024 Nicolae-Cristian MACOVEI nicolae.macovei1412@stud.acs.upb.ro
*/

#ifndef HASHMAP_H_GUARD
#define HASHMAP_H_GUARD 0x69

#include "linked_list.h"

/**
 * @brief This structure represents the data that's held in each node of the
 * linked lists
 */
typedef struct {
	void *key;
	void *val;
} map_info_t;

/**
 * @brief Structure for a generic hashmap
 * hmax - the number of buckets
 * size - number of elements stored in the hashmap
 * hash_function_key - function to generate a hash for each key
 * key_compare_func - function to compare two keys (should return zero if
 * they're equal)
 * key_val_destructor - function to free both the key and the value
 */
typedef struct {
	unsigned int hmax;
	unsigned int size;

	unsigned int (*hash_function_key)(void *);
	int (*key_compare_func)(void *, void *);
	void (*key_val_destructor)(map_info_t *);

	ll_t **buckets;
} hashmap_t;

/**
 * @brief Initialises the hashmap
 *
 * @param hmax - the number of buckets
 * @param hash_function_key - function to generate a hash for each key
 * @param key_compare_func - function to compare two keys (should return zero if
 * @param key_val_destructor - function to free both the key and the value
 * @return hashmap_t* - the hashmap we just created
 */
hashmap_t *hm_init(unsigned int hmax, unsigned int (*hash_function_key)(void *),
									 int (*key_compare_func)(void *, void *),
									 void (*key_val_destructor)(map_info_t *));

/**
 * @brief Function to get the value corresponding to a key from the map
 *
 * @param map
 * @param key
 * @return void* - the value if it's found or NULL otherwise
 */
void *hm_get(hashmap_t *map, void *key);

/**
 * @brief Function to set a value on the hashmap. Old value is destroyed if
 * found
 *
 * @param map
 * @param key - the key the new value corresponds to
 * @param key_size - size of the key (the hashmap stores a copy)
 * @param data - the new value
 * @param data_size - size of the new value
 */
void hm_set(hashmap_t *map, void *key, unsigned int key_size, void *data,
						unsigned int data_size);

/**
 * @brief Function to completely remove a key from the map
 *
 * @param map
 * @param key
 * @return void* -the key-value pair as a pointer of map_info_t, which should be
 * freed by the caller
 */
void *hm_remove(hashmap_t *map, void *key);

/**
 * @brief Function to free al the data in a hashmap
 *
 * @param map
 */
void hm_free(hashmap_t *map);

#endif
