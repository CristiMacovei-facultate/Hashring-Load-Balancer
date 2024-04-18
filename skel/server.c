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

static response *server_edit_document(server *s, char **ptr_doc_name,
																			char **ptr_doc_content)
{
	char *doc_name = *ptr_doc_name;
	char *doc_content = *ptr_doc_content;

	response *res = malloc(sizeof(response));
	res->server_log = malloc(100);
	res->server_response = malloc(100);

	// printf("[d] Am de editat %s\n", doc_name);
	char **content_ptr = lru_cache_get(s->cache, ptr_doc_name);
	bool cache_hit;
	if (!content_ptr) {
		cache_hit = false;
		sprintf(res->server_log, "Cache MISS for %s", doc_name);
	}
	else {
		cache_hit = true;
		sprintf(res->server_log, "Cache HIT for %s", doc_name);
	}

	void *content = hm_get(s->local_db, doc_name);
	hm_set(s->local_db, doc_name, 1 + strlen(doc_name), doc_content,
				 1 + strlen(doc_content));

	if (cache_hit || content) {
		sprintf(res->server_response, "Document %s has been overridden\n",
						doc_name);
	}
	else {
		sprintf(res->server_response, "Document %s has been created\n", doc_name);
	}

	void *evicted_key;
	lru_cache_put(s->cache, ptr_doc_name, ptr_doc_content, &evicted_key);

	return res;
}

static response *server_get_document(server *s, char *doc_name)
{
	// printf("[d] Am de gasit %s\n", doc_name);
	response *res = malloc(sizeof(response));
	res->server_log = malloc(100);
	res->server_response = malloc(100);

	char **content_ptr = lru_cache_get(s->cache, &doc_name);
	// printf("Intoarce pointeru %p\n", content_ptr);
	if (!content_ptr) {
		// printf("Gherla cache\n");
		// res->server_log = strcat("Cache MISS for ", doc_name);
		sprintf(res->server_log, "Cache MISS for %s", doc_name);
	}
	else {
		// printf("A gasit cache-ul\n");
		// res->server_log = strcat("Cache HIT for ", doc_name);
		sprintf(res->server_log, "Cache HIT for %s", doc_name);
		res->server_id = s->id;
		// res->server_response = *content_ptr;
		sprintf(res->server_response, "%s", *content_ptr);
		return res;
	}

	char *local_response = hm_get(s->local_db, doc_name);
	if (local_response) {
		// res->server_response = local_response;
		sprintf(res->server_response, "%s", doc_name);
	}
	else {
		// res->server_response = strcat("Document", doc_name);
		sprintf(res->server_response, "(null)");
		sprintf(res->server_log, "Document %s doesn't exist", doc_name);
	}
	return res;
}

server *init_server(unsigned int cache_size)
{
	server *srv = malloc(sizeof(server));
	srv->cache = init_lru_cache(cache_size);
	srv->local_db = hm_init(100, hash_string, compare_string);
	srv->task_queue = q_init(sizeof(request));

	// printf("[d] Am ajuns aici\n");
	return srv;
}

response *server_handle_request(server *s, request *req)
{
	// printf("[d] Mi-a dat mi-a dat pachet\n");
	if (req->type == EDIT_DOCUMENT) {
		// on edit request, put task in queue and go next
		q_push(s->task_queue, req);

		printf("[Server %d]-Response: Request- EDIT %s - has been added to queue\n",
					 s->id, req->doc_name);
		printf("[Server %d]-Log: Task queue size is %d\n\n", s->id,
					 s->task_queue->data->size);

		return NULL;
	}

	// printf("[d] E de-ala grav\n");
	while (s->task_queue->data->size > 0) {
		request *queued_req = q_pop(s->task_queue)->data;
		// printf("Am scos din queue edit pe %s\n", queued_req->doc_name);

		// queued_req should be an edit request
		response *q_res = server_edit_document(s, &queued_req->doc_name,
																					 &queued_req->doc_content);

		printf(GENERIC_MSG, s->id, q_res->server_response, s->id,
					 q_res->server_log);
	}

	// printf("\n\n\nAm terminat restantele, fac ce trebuie acuma\n");
	response *res = server_get_document(s, req->doc_name);

	printf(GENERIC_MSG, s->id, res->server_response, s->id, res->server_log);

	return NULL;
}

void free_server(server **s)
{
	free(*s);
	*s = NULL;
}
