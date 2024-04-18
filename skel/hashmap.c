#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "linked_list.h"

hashmap_t *hm_init(unsigned int hmax, unsigned int (*hash_function_key)(void *),
									 int (*key_compare_func)(void *, void *),
									 void (*key_val_destructor)(map_info_t *))
{
	hashmap_t *hm = malloc(sizeof(hashmap_t));
	hm->hmax = hmax;
	hm->hash_function_key = hash_function_key;
	hm->key_compare_func = key_compare_func;
	hm->size = 0;
	hm->key_val_destructor = key_val_destructor;

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
		if (map->key_compare_func(info->key, key) == 0) {
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
		if (map->key_compare_func(info->key, key) == 0) {
			info->val = copy_data;
			return;
		}
	}

	void *copy_key = malloc(key_size);
	memcpy(copy_key, key, key_size);

	map_info_t *new_info = malloc(sizeof(map_info_t));
	new_info->key = copy_key;
	new_info->val = copy_data;
	ll_insert_nth(list, list->size, new_info);
	free(new_info);
}

/*
function to remove a key-value pair from the hashmap
returns the pair as a pointer of map_info_t, which should be freed by the caller
*/
void *hm_remove(hashmap_t *map, void *key)
{
	unsigned int hash = map->hash_function_key(key) % map->hmax;
	ll_t *list = map->buckets[hash];

	int index = 0;
	for (ll_node_t *node = list->head; node; node = node->next) {
		map_info_t *info = node->data;
		if (map->key_compare_func(info->key, key) == 0) {
			--(map->size);
			ll_node_t *removed = ll_remove_nth_node(map->buckets[hash], index);
			free(removed);
			return info;
		}

		++index;
	}

	return NULL;
}

void hm_free(hashmap_t *map)
{
	for (int i = 0; i < map->hmax; ++i) {
		for (ll_node_t *node = map->buckets[i]->head; node; node = node->next) {
			map->key_val_destructor(node->data);
		}
		ll_free(map->buckets[i]);
	}
	free(map->buckets);
	free(map);
}