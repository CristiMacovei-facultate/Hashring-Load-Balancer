/*
 * Copyright (c) 2024, <>
 */

#include "server.h"
#include "hashmap.h"
#include "lru_cache.h"
#include <stdio.h>

#include "queue.h"
#include "utils.h"

static response *server_edit_document(server *s, char *doc_name,
																			char *doc_content)
{
	/* TODO */
	return NULL;
}

static response *server_get_document(server *s, char *doc_name)
{
	/* TODO */
	return NULL;
}

server *init_server(unsigned int cache_size)
{
	server *srv = malloc(sizeof(server));
	srv->cache = init_lru_cache(cache_size);
	srv->local_db = hm_init(100, hash_string, compare_string);
	srv->task_queue = q_init(sizeof(request));

	printf("[d] Am ajuns aici\n");
	return srv;
}

response *server_handle_request(server *s, request *req)
{
	/* TODO */
	return NULL;
}

void free_server(server **s)
{ /* TODO */
}
