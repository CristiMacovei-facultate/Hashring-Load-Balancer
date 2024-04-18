/*
 * Copyright (c) 2024, <>
 */

#include <stdio.h>
#include <string.h>

#include "doubly_linked_list.h"
#include "hashmap.h"
#include "lru_cache.h"
#include "utils.h"

void string_ptr_to_long_map_destructor(map_info_t *info)
{
	char **key = info->key;
	// comentat pentru ca ii da dll-ul free
	// e skill issue la mine, cheia din mapa si cheia din dll pointeaza de fapt la
	// acelasi string
	// free(*key);
	free(key);

	size_t *val = info->val;
	free(val);
}

void dll_data_destructor(void *data)
{
	lru_dll_data *dll_data = (lru_dll_data *)data;

	char **key = dll_data->key;
	free(*key);
	free(key);

	char **val = dll_data->val;
	free(*val);
	free(val);
}

lru_cache *init_lru_cache(unsigned int cache_capacity)
{
	lru_cache *cache = malloc(sizeof(lru_cache));

	cache->cap = cache_capacity;

	cache->dll = dll_init(sizeof(lru_dll_data), dll_data_destructor);
	cache->map =
			hm_init(cache_capacity, hash_string_pointer, compare_string_pointers,
							string_ptr_to_long_map_destructor);

	return cache;
}

bool lru_cache_is_full(lru_cache *cache)
{
	return cache->dll->size == cache->cap;
}

void free_lru_cache(lru_cache **cache)
{
	hm_free((*cache)->map);
	dll_free((*cache)->dll);
	free(*cache);
	*cache = NULL;
}

bool lru_cache_put(lru_cache *cache, void *key, void *value, void **evicted_key)
{
	*evicted_key = NULL;

	dll_node_t **hm_response = hm_get(cache->map, key);
	if (!hm_response) {
		if (lru_cache_is_full(cache)) {
			dll_node_t *removed = cache->dll->tail;
			*evicted_key = ((lru_dll_data *)removed->data)->key;

			lru_cache_remove(cache, *evicted_key);
		}

		lru_dll_data *node_data = malloc(sizeof(lru_dll_data));
		node_data->key = malloc(sizeof(char *));
		memcpy(node_data->key, key, sizeof(char *));
		node_data->val = malloc(sizeof(char *));
		memcpy(node_data->val, value, sizeof(char *));

		dll_node_t *node = dll_insert_nth_node(cache->dll, 0, node_data);
		free(node_data);

		size_t pointer = (size_t)node;
		hm_set(cache->map, key, sizeof(char *), &pointer, sizeof(size_t));

		return false;
	}

	dll_node_t *node = *hm_response;

	// key duplicate will not be stored since we already have the key
	free(*(char **)key);

	// delete old value before changing to the new one
	lru_dll_data *node_data = node->data;
	char **old_val = node_data->val;
	free(*old_val);
	free(old_val);

	node_data->val = malloc(sizeof(char *));
	memcpy(node_data->val, value, sizeof(char *));

	dll_remove_node(cache->dll, node);
	dll_insert_nth(cache->dll, 0, node);
	return true;
}

void *lru_cache_get(lru_cache *cache, void *key)
{
	dll_node_t **hm_response = hm_get(cache->map, key);
	if (!hm_response) {
		return NULL;
	}

	dll_node_t *node = *(dll_node_t **)hm_response;
	lru_dll_data *info = node->data;

	return info->val;
}

void lru_cache_remove(lru_cache *cache, void *key)
{
	map_info_t *info = hm_remove(cache->map, key);

	char *key_string = info->key;
	dll_node_t *node = *(dll_node_t **)info->val;

	if (node) {
		dll_remove_node(cache->dll, node);
	}

	cache->map->key_val_destructor(info);
}
