#ifndef HASHMAP_H_GUARD
#define HASHMAP_H_GUARD 0x69

#include "linked_list.h"

typedef struct {
	unsigned int hmax;
	unsigned int size;

	unsigned int (*hash_function_key)(void *);
	int (*key_compare_func)(void *, void *);

	ll_t **buckets;
} hashmap_t;

typedef struct {
	void *key;
	void *val;
} map_info_t;

hashmap_t *hm_init(unsigned int hmax, unsigned int (*hash_function_key)(void *),
									 int (*key_compare_func)(void *, void *));

void *hm_get(hashmap_t *map, void *key);

void hm_set(hashmap_t *map, void *key, unsigned int key_size, void *data,
						unsigned int data_size);

void *hm_remove(hashmap_t *map, void *key);

#endif