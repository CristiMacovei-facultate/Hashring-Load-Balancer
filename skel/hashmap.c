#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "linked_list.h"

hashmap_t *hm_init(unsigned int hmax, unsigned int (*hash_function_key)(void *),
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
		printf("Caut %s, am gasit %s\n", *(char **)key, *(char **)info->key);
		if (map->key_compare_func(info->key, key) == 0) {
			return info->val;
		}
		else {
			printf("Cum drq ba)))\n");
		}
	}

	printf("Aici plescaie\n");

	return NULL;
}

void hm_set(hashmap_t *map, void *key, unsigned int key_size, void *data,
						unsigned int data_size)
{
	if (key_size == 8 && data_size == 8) {
		printf("Scriu 0x%lx pe cheia %s\n", *(size_t *)data, *(char **)key);
	}
	unsigned int hash = map->hash_function_key(key) % map->hmax;
	ll_t *list = map->buckets[hash];

	void *copy_data = malloc(data_size);
	memcpy(copy_data, data, data_size);
	if (key_size == 8 && data_size == 8) {
		printf("Scriu 0x%lx pe cheia %s\n", *(size_t *)copy_data, *(char **)key);
	}

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
	new_info->key = copy_key;
	new_info->val = copy_data;
	ll_insert_nth(list, list->size, new_info);

	if (key_size == 8 && data_size == 8) {
		ll_node_t *check_tail = list->tail;
		printf("Proba dupa ll insert: 0x%lx, %s\n",
					 *(size_t *)(((map_info_t *)check_tail->data)->val),
					 *(char **)(((map_info_t *)check_tail->data)->key));
	}
}

void *hm_remove(hashmap_t *map, void *key)
{
	unsigned int hash = map->hash_function_key(key) % map->hmax;
	ll_t *list = map->buckets[hash];

	int index = 0;
	for (ll_node_t *node = list->head; node; node = node->next) {
		map_info_t *info = node->data;
		if (map->key_compare_func(info->key, key)) {
			ll_node_t *removed = ll_remove_nth_node(map->buckets[hash], index);
			return removed->data;
		}

		++index;
	}

	return NULL;
}