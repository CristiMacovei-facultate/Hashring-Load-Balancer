/*
 * Copyright (c) 2024, <>
 */

#include "server.h"
#include "constants.h"
#include "hashmap.h"
#include "linked_list.h"
#include "lru_cache.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "queue.h"
#include "utils.h"

static response *server_edit_document(server *s, char *doc_name,
																			char *doc_content)
{
	response *res = malloc(sizeof(response));
	res->server_log = malloc(100);
	res->server_response = malloc(100);

	char **content_ptr = lru_cache_get(s->cache, &doc_name);
	bool cache_hit;
	if (!content_ptr) {
		cache_hit = false;
	}
	else {
		cache_hit = true;
		sprintf(res->server_log, "Cache HIT for %s", doc_name);
	}

	void *content = hm_get(s->local_db, doc_name);
	hm_set(s->local_db, doc_name, 1 + strlen(doc_name), doc_content,
				 1 + strlen(doc_content));

	if (cache_hit || content) {
		sprintf(res->server_response, "Document %s has been overridden", doc_name);
	}
	else {
		sprintf(res->server_response, "Document %s has been created", doc_name);
	}

	void *evicted_key;
	// printf("Dup name: %p, dup content: %p\n", dup_doc_name, dup_doc_content);
	char *cache_dup_name = strdup(doc_name);
	char *cache_dup_cont = strdup(doc_content);
	lru_cache_put(s->cache, &cache_dup_name, &cache_dup_cont, &evicted_key);

	if (!cache_hit) {
		if (evicted_key) {
			char *actual_evicted = *(char **)evicted_key;
			sprintf(res->server_log,
							"Cache MISS for %s - cache entry for %s has been evicted",
							doc_name, actual_evicted);
		}
		else {
			sprintf(res->server_log, "Cache MISS for %s", doc_name);
		}
	}

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
	if (content_ptr) {
		// printf("A gasit cache-ul\n");
		sprintf(res->server_log, "Cache HIT for %s", doc_name);
		res->server_id = s->id;
		sprintf(res->server_response, "%s", *content_ptr);
		return res;
	}

	char *local_response = hm_get(s->local_db, doc_name);
	if (local_response) {
		// res->server_response = local_response;
		sprintf(res->server_response, "%s", local_response);

		void *evicted_key;
		char *cache_dup_name = strdup(doc_name);
		char *cache_dup_cont = strdup(local_response);
		lru_cache_put(s->cache, &cache_dup_name, &cache_dup_cont, &evicted_key);

		if (evicted_key) {
			char *actual_evicted = *(char **)evicted_key;
			sprintf(res->server_log,
							"Cache MISS for %s - cache entry for %s has been evicted",
							doc_name, actual_evicted);
		}
		else {
			sprintf(res->server_log, "Cache MISS for %s", doc_name);
		}
	}
	else {
		// res->server_response = strcat("Document", doc_name);
		sprintf(res->server_response, "(null)");
		sprintf(res->server_log, "Document %s doesn't exist", doc_name);
	}
	return res;
}

void string_to_string_map_destructor(map_info_t *info)
{
	char *key = info->key;
	char *val = info->val;
	free(key);
	free(val);
}

server *init_server(unsigned int cache_size)
{
	server *srv = malloc(sizeof(server));
	srv->cache = init_lru_cache(cache_size);
	srv->local_db = hm_init(100, hash_string, compare_string,
													string_to_string_map_destructor);
	srv->task_queue = q_init(sizeof(request));

	return srv;
}

response *server_handle_request(server *s, request *req)
{
	// printf("[d] Mi-a dat mi-a dat pachet\n");
	if (req->type == EDIT_DOCUMENT) {
		// on edit request, put task in queue and go next
		request *new_req = malloc(sizeof(request));
		new_req->type = req->type;
		new_req->doc_content = strdup(req->doc_content);
		new_req->doc_name = strdup(req->doc_name);

		q_push(s->task_queue, new_req);
		free(new_req);

		printf("[Server %d]-Response: Request- EDIT %s - has been added to queue\n",
					 s->id, req->doc_name);
		printf("[Server %d]-Log: Task queue size is %d\n\n", s->id,
					 s->task_queue->data->size);

		return NULL;
	}

	while (s->task_queue->data->size > 0) {
		ll_node_t *req_node = q_pop(s->task_queue);
		request *queued_req = req_node->data;

		// queued_req should be an edit request
		response *q_res =
				server_edit_document(s, queued_req->doc_name, queued_req->doc_content);

		printf(GENERIC_MSG, s->id, q_res->server_response, s->id,
					 q_res->server_log);
		free(q_res->server_response);
		free(q_res->server_log);
		free(q_res);

		free(queued_req->doc_content);
		free(queued_req->doc_name);
		free(queued_req);

		free(req_node);
	}

	// printf("\n\n\nAm terminat restantele, fac ce trebuie acuma\n");
	response *res = server_get_document(s, req->doc_name);

	printf(GENERIC_MSG, s->id, res->server_response, s->id, res->server_log);
	free(res->server_response);
	free(res->server_log);
	free(res);

	return NULL;
}

void free_server(server **s)
{
	free_lru_cache(&((*s)->cache));
	q_free((*s)->task_queue);
	hm_free((*s)->local_db);
	free(*s);
	*s = NULL;
}
