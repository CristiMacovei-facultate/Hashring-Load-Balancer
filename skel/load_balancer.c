/*
 * Copyright (c) 2024, <>
 */

#include "load_balancer.h"
#include "arraylist.h"
#include "server.h"
#include "utils.h"

load_balancer *init_load_balancer(bool enable_vnodes)
{
	load_balancer *lb = malloc(sizeof(load_balancer));
	lb->hash_function_docs = hash_string;
	lb->hash_function_servers = hash_uint;
	lb->servers = al_init(sizeof(server));
	return lb;
}

void loader_add_server(load_balancer *lb, int server_id, int cache_size)
{
	server *srv = malloc(sizeof(server));

	al_insert(lb->servers, lb->servers->size, srv);
}

void loader_remove_server(load_balancer *main, int server_id)
{ /* TODO */
}

response *loader_forward_request(load_balancer *main, request *req)
{
	/* TODO */
	return NULL;
}

void free_load_balancer(load_balancer **main)
{
	/* TODO: get rid of test_server after testing the server implementation */
	// free_server(&(*main)->test_server);
	free(*main);

	*main = NULL;
}
