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
	free(*key);
	free(key);
}

void string_ptr_destructor(void *data)
{
	char *string_ptr = data;
	// free(*string_ptr);
	// free(string_ptr);
}

lru_cache *init_lru_cache(unsigned int cache_capacity)
{
	lru_cache *cache = malloc(sizeof(lru_cache));

	cache->cap = cache_capacity;

	cache->dll = dll_init(sizeof(char *), string_ptr_destructor);
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
	dll_node_t **hm_response = hm_get(cache->map, key);
	if (!hm_response) {
		char **data = malloc(sizeof(char *));
		memcpy(data, value, sizeof(char *));
		dll_node_t *node = dll_insert_nth_node(cache->dll, 0, data);
		free(data);

		size_t pointer = (size_t)node;
		hm_set(cache->map, key, sizeof(char *), &pointer, sizeof(size_t));

		if (lru_cache_is_full(cache)) {
			dll_node_t *removed = cache->dll->tail;
			*evicted_key = ((lru_dll_data *)removed->data)->key;
			lru_cache_remove(cache, *evicted_key);
		}

		return false;
	}

	dll_node_t *node = *hm_response;
	char **info = node->data;
	info = malloc(cache->dll->data_size);
	memcpy(info, value, sizeof(char *));

	// dll_move_node_first(cache->dll, node);
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
	char **info = node->data;

	return info;
}

void lru_cache_remove(lru_cache *cache, void *key)
{
	dll_node_t *node = hm_remove(cache->map, key);

	if (node) {
		dll_remove_node(cache->dll, node);
	}
}
