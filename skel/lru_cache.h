/*
Copyright (c) 2024 Nicolae-Cristian MACOVEI nicolae.macovei1412@stud.acs.upb.ro
*/

#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <stdbool.h>

#include "doubly_linked_list.h"
#include "hashmap.h"

typedef struct {
	char **key;
	char **val;
} lru_dll_data;

typedef struct lru_cache {
	// data will be lru_dll_data*
	dll_t *dll;

	// ptr<string> --> ptr<dll_node_t>
	hashmap_t *map;

	unsigned int cap;
} lru_cache;

lru_cache *init_lru_cache(unsigned int cache_capacity);

bool lru_cache_is_full(lru_cache *cache);

void free_lru_cache(lru_cache **cache);

/**
 * lru_cache_put() - Adds a new pair in our cache.
 *
 * @param cache: Cache where the key-value pair will be stored.
 * @param key: Key of the pair.
 * @param value: Value of the pair.
 * @param evicted_key: The function will RETURN via this parameter the
 *      key removed from cache if the cache was full.
 *
 * @return - true if the key was added to the cache,
 *      false if the key already existed.
 */
bool lru_cache_put(lru_cache *cache, void *key, void *value,
									 void **evicted_key);

/**
 * lru_cache_get() - Retrieves the value associated with a key.
 *
 * @param cache: Cache where the key-value pair is stored.
 * @param key: Key of the pair.
 *
 * @return - The value associated with the key,
 *      or NULL if the key is not found.
 */
void *lru_cache_get(lru_cache *cache, void *key);

/**
 * lru_cache_remove() - Removes a key-value pair from the cache.
 *
 * @param cache: Cache where the key-value pair is stored.
 * @param key: Key of the pair.
 */
void lru_cache_remove(lru_cache *cache, void *key);

#endif /* LRU_CACHE_H */
