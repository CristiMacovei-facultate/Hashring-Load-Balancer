/*
Copyright (c) 2024 Nicolae-Cristian MACOVEI nicolae.macovei1412@stud.acs.upb.ro
*/

#ifndef HASHMAP_H_GUARD
#define HASHMAP_H_GUARD 0x69

#include "linked_list.h"

typedef struct {
	void *key;
	void *val;
} map_info_t;
typedef struct {
	unsigned int hmax;
	unsigned int size;

	unsigned int (*hash_function_key)(void *);
	int (*key_compare_func)(void *, void *);
	void (*key_val_destructor)(map_info_t *);

	ll_t **buckets;
} hashmap_t;

hashmap_t *hm_init(unsigned int hmax, unsigned int (*hash_function_key)(void *),
									 int (*key_compare_func)(void *, void *),
									 void (*key_val_destructor)(map_info_t *));

void *hm_get(hashmap_t *map, void *key);

void hm_set(hashmap_t *map, void *key, unsigned int key_size, void *data,
						unsigned int data_size);

void *hm_remove(hashmap_t *map, void *key);

void hm_free(hashmap_t *map);

#endif
