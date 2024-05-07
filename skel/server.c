/*
Copyright (c) 2024 Nicolae-Cristian MACOVEI nicolae.macovei1412@stud.acs.upb.ro
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
	res->server_log = malloc(1000);
	res->server_response = malloc(1000);

	char **content_ptr = lru_cache_get(s->cache, &doc_name);
	bool cache_hit;
	if (!content_ptr) {
		cache_hit = false;
	} else {
		cache_hit = true;
		sprintf(res->server_log, "Cache HIT for %s", doc_name);
	}

	void *content = hm_get(s->local_db, doc_name);
	hm_set(s->local_db, doc_name, 1 + strlen(doc_name), doc_content,
				 1 + strlen(doc_content));

	if (cache_hit || content) {
		sprintf(res->server_response, "Document %s has been overridden", doc_name);
	} else {
		sprintf(res->server_response, "Document %s has been created", doc_name);
	}

	void *evicted_key;
	// printf("Dup name: %p, dup content: %p\n", dup_doc_name, dup_doc_content);
	char *cache_dup_name = strdup(doc_name);
	char *cache_dup_cont = strdup(doc_content);
	lru_cache_put(s->cache, &cache_dup_name, &cache_dup_cont, &evicted_key);

	if (!cache_hit) {
		if (evicted_key) {
			sprintf(res->server_log,
							"Cache MISS for %s - cache entry for %s has been evicted",
							doc_name, (char *)evicted_key);
			free(evicted_key);
		} else {
			sprintf(res->server_log, "Cache MISS for %s", doc_name);
		}
	}

	return res;
}

static response *server_get_document(server *s, char *doc_name)
{
	// printf("[d] Am de gasit %s\n", doc_name);
	response *res = malloc(sizeof(response));
	res->server_log = malloc(1000);
	res->server_response = malloc(1000);

	char **content_ptr = lru_cache_get(s->cache, &doc_name);
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
			sprintf(res->server_log,
							"Cache MISS for %s - cache entry for %s has been evicted",
							doc_name, (char *)evicted_key);

			free(evicted_key);
		} else {
			sprintf(res->server_log, "Cache MISS for %s", doc_name);
		}
	} else {
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

	// fprintf(stderr, "I blew up %s\n", val);

	free(key);
	free(val);
}

server *init_server(unsigned int cache_size)
{
	server *srv = malloc(sizeof(server));
	srv->cache = init_lru_cache(cache_size);
	srv->local_db = hm_init(HASHMAP_SIZE, hash_string, compare_string,
													string_to_string_map_destructor);
	srv->task_queue = q_init(sizeof(request));

	return srv;
}

void solve_queue(server *s, bool free_only)
{
	while (s->task_queue->data->size > 0) {
		ll_node_t *req_node = q_pop(s->task_queue);
		request *queued_req = req_node->data;

		// queued_req should be an edit request
		if (!free_only) {
			response *q_res = server_edit_document(s, queued_req->doc_name,
																						 queued_req->doc_content);

			printf(GENERIC_MSG, s->id, q_res->server_response, s->id,
						 q_res->server_log);
			free(q_res->server_response);
			free(q_res->server_log);
			free(q_res);
		}

		free(queued_req->doc_content);
		free(queued_req->doc_name);
		free(queued_req);

		free(req_node);
	}
}

response *server_handle_request(server *s, request *req)
{
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

	solve_queue(s, false);

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
	if ((*s)->id < 100000) {
		solve_queue(*s, true);

		free_lru_cache(&((*s)->cache));
		q_free((*s)->task_queue);
		hm_free((*s)->local_db);
	}
	free(*s);
	*s = NULL;
}

void transfer_files(server *src, server *dest, bool force_move)
{
	if (src->id % 100000 == dest->id % 100000) {
		return;
	}
	// printf("Am de adaugat fisiere din serverul %d in serverul %d fm = %d\n",
	//  src->id, dest->id, force_move);

	solve_queue(src, false);

	unsigned int src_hash = hash_uint(&src->id);
	unsigned int dest_hash = hash_uint(&dest->id);

	// mut din local db in local db
	for (int i = 0; i < (int)src->local_db->hmax; ++i) {
		for (ll_node_t *node = src->local_db->buckets[i]->head; node;) {
			map_info_t *info = node->data;
			ll_node_t *next = node->next;

			char *name = (char *)info->key;
			char *content = (char *)info->val;

			unsigned int name_hash = hash_string(name);

			bool should_move = false;
			// printf("Consider %s\n", name);
			// printf("src hash: %u, name hash: %u, dest hash: %u\n", src_hash,
			// 			 name_hash, dest_hash);

			if (src_hash < name_hash && name_hash < dest_hash) {
				should_move = true;
			}
			if (src_hash < name_hash && dest_hash < src_hash) {
				should_move = true;
			}
			if (name_hash < dest_hash && dest_hash <= src_hash) {
				should_move = true;
			}

			// printf("Name: %s, gets moved: %d\n", name, force_move || should_move);
			if (force_move || should_move) {
				// printf("Mut din %d in %d: '%s' = '%s'\n", src->id, dest->id, name,
				// 			 content);
				hm_set(dest->local_db, name, 1 + strlen(name), content,
							 1 + strlen(content));
				// fprintf(stderr, "I added %s on server %d\n", content,
				// dest->local_db);

				map_info_t *removed = hm_remove(src->local_db, name);
				free(removed);

				// printf("Trying to remove %s\n", name);
				lru_cache_remove(src->cache, &name);

				free(content);
				free(name);
			}

			node = next;
		}
	}
}

server *pick_dest(unsigned name_hash, server **dests, server *src,
									unsigned *dest_hashes)
{
	unsigned best_hash;
	server *ans = NULL;
	for (int i = 0; i < 3; ++i) {
		// printf("Dest id: %d, src id %d\n", dests[i]->id, src->id);
		if (dests[i]->id % 100000 == src->id % 100000) {
			continue;
		}
		if (ans == NULL) {
			ans = dests[i];
			best_hash = dest_hashes[i];
			continue;
		}

		if (name_hash < dest_hashes[i] && dest_hashes[i] <= best_hash) {
			ans = dests[i];
			best_hash = dest_hashes[i];
			continue;
		}

		if (name_hash < dest_hashes[i] && best_hash < name_hash) {
			ans = dests[i];
			best_hash = dest_hashes[i];
			continue;
		}

		if (name_hash > best_hash && dest_hashes[i] < best_hash) {
			ans = dests[i];
			best_hash = dest_hashes[i];
			continue;
		}
	}

	if (!ans) {
		printf("Am dat-o in gard\n");
		exit(-1);
	}
	return ans;
}

void transfer_files_multiple(server *src, server **dests)
{
	solve_queue(src, false);

	unsigned int dest_hashes[3];
	for (int i = 0; i < 3; ++i) {
		dest_hashes[i] = hash_uint(&dests[i]->id);
	}

	for (int i = 0; i < (int)src->local_db->hmax; ++i) {
		for (ll_node_t *node = src->local_db->buckets[i]->head; node;) {
			map_info_t *info = node->data;
			ll_node_t *next = node->next;

			char *name = (char *)info->key;
			char *content = (char *)info->val;

			unsigned int name_hash = hash_string(name);

			// printf("For file = '%s' (hash = %u)\n", name, name_hash);

			server *dest = pick_dest(name_hash, dests, src, dest_hashes);
			// printf("Picked dest = %d\n", dest->id);

			hm_set(dest->local_db, name, 1 + strlen(name), content,
						 1 + strlen(content));

			// fprintf(stderr, "I added %s on server %d\n", content,
			// dest->local_db);

			map_info_t *removed = hm_remove(src->local_db, name);
			free(removed);

			// printf("Trying to remove %s\n", name);
			lru_cache_remove(src->cache, &name);

			free(content);
			free(name);

			node = next;
		}
	}
}

server *pick_src(unsigned name_hash, server **replicas,
								 unsigned *replica_hashes)
{
	server *ans = NULL;
	unsigned best_hash;

	for (int i = 0; i < 3; ++i) {
		if (!ans) {
			ans = replicas[i];
			best_hash = replica_hashes[i];
			continue;
		}

		if (name_hash < replica_hashes[i] && replica_hashes[i] < best_hash) {
			ans = replicas[i];
			best_hash = replica_hashes[i];
		}

		if (name_hash < replica_hashes[i] && best_hash < name_hash) {
			ans = replicas[i];
			best_hash = replica_hashes[i];
			continue;
		}

		if (name_hash > best_hash && replica_hashes[i] < best_hash) {
			ans = replicas[i];
			best_hash = replica_hashes[i];
			continue;
		}
	}

	return ans;
}

void transfer_files_multiple_srcs(server **replicas, server *src, server *dest)
{
	if (src->id % 100000 == dest->id % 100000) {
		return;
	}
	// printf("Am de adaugat fisiere din serverul %d in serverul %d fm = %d\n",
	//  src->id, dest->id, force_move);

	solve_queue(src, false);

	unsigned int src_hash = hash_uint(&src->id);
	unsigned int dest_hash = hash_uint(&dest->id);

	unsigned int replica_hashes[3];
	for (int i = 0; i < 3; ++i) {
		replica_hashes[i] = hash_uint(replicas[i]);
	}

	// mut din local db in local db
	for (int i = 0; i < (int)src->local_db->hmax; ++i) {
		for (ll_node_t *node = src->local_db->buckets[i]->head; node;) {
			map_info_t *info = node->data;
			ll_node_t *next = node->next;

			char *name = (char *)info->key;
			char *content = (char *)info->val;

			unsigned int name_hash = hash_string(name);

			bool should_move = false;
			// printf("Consider %s\n", name);
			// printf("src hash: %u, name hash: %u, dest hash: %u\n", src_hash,
			// 			 name_hash, dest_hash);

			if (src_hash < name_hash && name_hash < dest_hash) {
				should_move = true;
			}
			if (src_hash < name_hash && dest_hash < src_hash) {
				should_move = true;
			}
			if (name_hash < dest_hash && dest_hash <= src_hash) {
				should_move = true;
			}

			server *true_src = pick_src(name_hash, replicas, replica_hashes);
			if (true_src->id != src->id) {
				should_move = false;
			}

			// printf("Name: %s, gets moved: %d\n", name, force_move || should_move);
			if (should_move) {
				// printf("Mut din %d in %d: '%s' = '%s'\n", src->id, dest->id, name,
				// 			 content);
				hm_set(dest->local_db, name, 1 + strlen(name), content,
							 1 + strlen(content));
				// fprintf(stderr, "I added %s on server %d\n", content,
				// dest->local_db);

				map_info_t *removed = hm_remove(src->local_db, name);
				free(removed);

				// printf("Trying to remove %s\n", name);
				lru_cache_remove(src->cache, &name);

				free(content);
				free(name);
			}

			node = next;
		}
	}
}
