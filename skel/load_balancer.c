/*
 * Copyright (c) 2024, <>
 */

#include "load_balancer.h"
#include "arraylist.h"
#include "server.h"
#include "utils.h"
#include <stdio.h>
#include <sys/types.h>

void lb_free_server(void *s)
{
	server *serv_ptr = s;
	free_server(&serv_ptr);
}

load_balancer *init_load_balancer(bool enable_vnodes)
{
	load_balancer *lb = malloc(sizeof(load_balancer));
	lb->hash_function_docs = hash_string;
	lb->hash_function_servers = hash_uint;
	lb->servers = al_init(sizeof(server), lb_free_server);
	return lb;
}

int cmp_servers(void *a, void *b)
{
	server *first = a;
	server *second = b;

	int hash_first = hash_uint(&first->id);
	int hash_second = hash_uint(&second->id);

	if (hash_first != hash_second) {
		return hash_first - hash_second;
	}

	return first->id - second->id;
}

int cmp_server_to_hash(void *s, void *h)
{
	server *srv = s;
	unsigned int hash_server = hash_uint(&srv->id);
	unsigned int hash = *(unsigned int *)h;

	int result = 0;
	if (hash_server > hash) {
		result = 1;
	}
	else if (hash_server < hash) {
		result = -1;
	}

	// printf("Compar %u (h) cu %u (s), iese %d\n", hash, hash_server, result);

	return result;
}

void loader_add_server(load_balancer *lb, int server_id, int cache_size)
{
	server *srv = init_server(cache_size);
	srv->id = server_id;

	// al_insert(lb->servers, lb->servers->size, srv);
	al_insert_ordered(lb->servers, srv, cmp_servers);
	free(srv);
}

void loader_remove_server(load_balancer *main, int server_id)
{ /* TODO */
}

response *loader_forward_request(load_balancer *main, request *req)
{
	if (main->servers->size == 0) {
		printf("[d] N-am servere cplm\n");
		return NULL;
	}

	// hardcoded for now
	unsigned int hash_document = hash_string(req->doc_name);
	server *srv2 =
			al_get_ordered(main->servers, &hash_document, cmp_server_to_hash);
	// printf("Imi vine document cu hash-ul %u, pun pe server %d\n",
	// hash_document,
	//		 srv2->id);
	// server *srv = al_get(main->servers, 0);
	response *res = server_handle_request(srv2, req);

	return res;
}

void free_load_balancer(load_balancer **main)
{
	al_free((*main)->servers);

	free(*main);
	*main = NULL;
}
