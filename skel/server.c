/*
 * Copyright (c) 2024, <>
 */

#include "server.h"
#include "constants.h"
#include "hashmap.h"
#include "lru_cache.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "queue.h"
#include "utils.h"

static response *server_edit_document(server *s, char *doc_name,
																			char *doc_content)
{
	printf("[d] Am de editat %s\n", doc_name);
	bool cache_hit;
	char **content_ptr = lru_cache_get(s->cache, &doc_name);
	if (!content_ptr) {
		printf("Gherla cache\n");
		cache_hit = false;
	}
	else {
		printf("A gasit cache-ul\n");
		cache_hit = true;
		return NULL;
	}

	char *content = hm_get(s->local_db, doc_name);
	if (!content) {
		printf("Gherla si local db, trebuie creat nenorocitu\n");

		hm_set(s->local_db, doc_name, strlen(doc_name), doc_content,
					 strlen(doc_content));

		printf("L-am creat pe %s in local\n", doc_name);
		void *evicted_key;
		lru_cache_put(s->cache, &doc_name, &doc_content, &evicted_key);
		printf("L-am pus pe %s ca %s in cache\n", doc_name, doc_content);

		char *check = *(char **)lru_cache_get(s->cache, &doc_name);
		printf("Fac proba: %s\n", check);
	}
	else {
		printf("L-a gasit db\n");
		hm_set(s->local_db, doc_name, strlen(doc_name), doc_content,
					 strlen(doc_content));

		printf("L-am editat pe %s in local\n", doc_name);

		void *evicted_key;
		lru_cache_put(s->cache, doc_name, doc_content, &evicted_key);
		printf("L-am pus pe %s ca %s in cache\n", doc_name, doc_content);
	}

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
	printf("[d] Mi-a dat mi-a dat pachet\n");
	if (req->type == EDIT_DOCUMENT) {
		// on edit request, put task in queue and go next
		q_push(s->task_queue, req);

		printf("[Server %d]-Response: Request- EDIT %s - has been added to queue\n",
					 s->id, req->doc_name);
		printf("[Server %d]-Log: Task queue size is %d\n\n", s->id,
					 s->task_queue->data->size);

		return NULL;
	}

	printf("[d] E de-ala grav\n");
	while (s->task_queue->data->size > 0) {
		request *queued_req = q_pop(s->task_queue)->data;
		printf("Am scos din queue edit pe %s\n", queued_req->doc_name);

		// queued_req should be an edit request
		response *res =
				server_edit_document(s, queued_req->doc_name, queued_req->doc_content);
	}

	return NULL;
}

void free_server(server **s)
{ /* TODO */
}
