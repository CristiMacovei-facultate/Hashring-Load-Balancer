/*
 * Copyright (c) 2024, <>
 */

#include <stdio.h>
#include <string.h>

#include "doubly_linked_list.h"
#include "hashmap.h"
#include "lru_cache.h"
#include "utils.h"

lru_cache *init_lru_cache(unsigned int cache_capacity)
{
	lru_cache *cache = malloc(sizeof(lru_cache));

	cache->cap = cache_capacity;

	cache->dll = dll_init(sizeof(lru_dll_data));
	cache->map =
			hm_init(cache_capacity, hash_string_pointer, compare_string_pointers);

	return cache;
}

bool lru_cache_is_full(lru_cache *cache)
{
	return cache->dll->size == cache->cap;
}

void free_lru_cache(lru_cache **cache)
{ /* TODO */
}

bool lru_cache_put(lru_cache *cache, void *key, void *value, void **evicted_key)
{
	dll_node_t *node = hm_get(cache->map, key);
	printf("node = %p\n", node);
	if (!node) {
		lru_dll_data *data = malloc(sizeof(lru_dll_data));
		data->key = key;
		data->val = value;
		node = dll_insert_nth_node(cache->dll, 0, data);
		printf("Vad asta 1, node = %p\n", node);

		size_t pointer = (size_t)node;
		printf("Node ptr = 0x%lx\n", pointer);

		hm_set(cache->map, key, sizeof(char *), &pointer, sizeof(size_t));

		size_t *check = hm_get(cache->map, key);
		printf("Fac proba la mapa: 0x%lx\n", *check);

		if (lru_cache_is_full(cache)) {
			dll_node_t *removed = cache->dll->tail;
			*evicted_key = ((lru_dll_data *)removed->data)->key;
			lru_cache_remove(cache, *evicted_key);
		}

		return false;
	}

	lru_dll_data *info = node->data;
	info->val = malloc(cache->dll->data_size);
	memcpy(info->val, value, cache->dll->data_size);

	// dll_move_node_first(cache->dll, node);
	dll_remove_node(cache->dll, node);
	dll_insert_nth(cache->dll, 0, node);
	return true;
}

void *lru_cache_get(lru_cache *cache, void *key)
{
	printf("Caut cheia %s\n", *(char **)key);
	dll_node_t **hm_response = hm_get(cache->map, key);
	printf("Aici crapi 1 %p\n", hm_response);
	if (!hm_response) {
		return NULL;
	}

	dll_node_t *node = *(dll_node_t **)hm_response;
	printf("Aici crapi 2 %p\n", node);
	lru_dll_data *info = node->data;
	return info->val;
}

void lru_cache_remove(lru_cache *cache, void *key)
{
	dll_node_t *node = hm_remove(cache->map, key);

	if (node) {
		dll_remove_node(cache->dll, node);
	}
}
