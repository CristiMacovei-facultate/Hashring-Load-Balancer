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

void loader_add_server(load_balancer *lb, int server_id, int cache_size)
{
	server *srv = init_server(cache_size);
	srv->id = server_id;

	al_insert(lb->servers, lb->servers->size, srv);
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
	server *srv = al_get(main->servers, 0);
	response *res = server_handle_request(srv, req);

	return res;
}

void free_load_balancer(load_balancer **main)
{
	al_free((*main)->servers);

	free(*main);
	*main = NULL;
}
