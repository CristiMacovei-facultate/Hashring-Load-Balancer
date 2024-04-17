#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "linked_list.h"

hashmap_t *hm_init(unsigned int hmax, unsigned int size,
									 unsigned int (*hash_function_key)(void *),
									 int (*key_compare_func)(void *, void *))
{
	hashmap_t *hm = malloc(sizeof(hashmap_t));
	hm->hmax = hmax;
	hm->hash_function_key = hash_function_key;
	hm->key_compare_func = key_compare_func;
	hm->size = 0;

	hm->buckets = malloc(hmax * sizeof(ll_t *));
	for (int i = 0; i < hmax; ++i) {
		hm->buckets[i] = ll_init(sizeof(map_info_t));
	}

	return hm;
}

void *hm_get(hashmap_t *map, void *key)
{
	unsigned int hash = map->hash_function_key(key) % map->hmax;
	ll_t *list = map->buckets[hash];

	for (ll_node_t *node = list->head; node; node = node->next) {
		map_info_t *info = node->data;
		if (map->key_compare_func(info->key, key)) {
			return info->val;
		}
	}

	return NULL;
}

void hm_set(hashmap_t *map, void *key, unsigned int key_size, void *data,
						unsigned int data_size)
{
	unsigned int hash = map->hash_function_key(key) % map->hmax;
	ll_t *list = map->buckets[hash];

	void *copy_data = malloc(data_size);
	memcpy(copy_data, data, data_size);

	for (ll_node_t *node = list->head; node; node = node->next) {
		map_info_t *info = node->data;
		if (map->key_compare_func(info->key, key)) {
			info->val = copy_data;
			return;
		}
	}

	void *copy_key = malloc(key_size);
	memcpy(copy_key, key, key_size);

	map_info_t *new_info = malloc(sizeof(map_info_t));
	ll_insert_nth(list, list->size, new_info);
}

void hm_remove(hashmap_t *map, void *key)
{
	unsigned int hash = map->hash_function_key(key) % map->hmax;
	ll_t *list = map->buckets[hash];

	int index = 0;
	for (ll_node_t *node = list->head; node; node = node->next) {
		map_info_t *info = node->data;
		if (map->key_compare_func(info->key, key)) {
			ll_remove_nth_node(map->buckets[hash], index);
			return;
		}

		++index;
	}
}