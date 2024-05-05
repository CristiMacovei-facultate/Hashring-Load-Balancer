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
	lb->has_vnodes = enable_vnodes;
	lb->servers = al_init(sizeof(server), lb_free_server);
	return lb;
}

int compare_uints(unsigned int a, unsigned int b)
{
	if (a > b) {
		return 1;
	}
	if (a < b) {
		return -1;
	}
	return 0;
}

int cmp_servers(void *a, void *b)
{
	server *first = a;
	server *second = b;

	unsigned int hash_first = hash_uint(&first->id);
	unsigned hash_second = hash_uint(&second->id);

	int result = compare_uints(hash_first, hash_second);

	// printf("Compar %u (id = %d) cu %u (id = %d) si iese %d\n", hash_first,
	// 			 first->id, hash_second, second->id, result);

	return result;
}

int cmp_server_to_hash(void *s, void *h)
{
	server *srv = s;
	unsigned int hash_server = hash_uint(&srv->id);
	unsigned int hash = *(unsigned int *)h;

	int result = compare_uints(hash_server, hash);

	// printf("Compar %u (h) cu %u (s), iese %d\n", hash, hash_server, result);

	return result;
}

int cmp_server_to_id(void *s, void *i)
{
	server *srv = s;
	int id = *(int *)i;

	return srv->id - id;
}

server *internal_loader_add_server(load_balancer *lb, int server_id,
																	 int cache_size, server *vnode_ref)
{
	server *srv = NULL;
	if (!vnode_ref) {
		srv = init_server(cache_size);
	}
	else {
		srv = malloc(sizeof(server));
		srv->cache = vnode_ref->cache;
		srv->local_db = vnode_ref->local_db;
		srv->task_queue = vnode_ref->task_queue;
	}
	srv->id = server_id;

	// al_insert(lb->servers, lb->servers->size, srv);
	int dest_index = al_insert_ordered(lb->servers, srv, cmp_servers);
	free(srv);

	// print_servers(lb->servers);

	int src_index = (dest_index + 1) % lb->servers->size;

	server *src_server = al_get(lb->servers, src_index);
	server *dest_server = al_get(lb->servers, dest_index);

	// printf("Vom muta toate fisierele care trebuie din src = %d in dest = %d\n",
	// 			 src_index, dest_index);

	transfer_files(src_server, dest_server, false);

	// print_servers(lb->servers);

	return dest_server;
}

void loader_add_server(load_balancer *lb, int server_id, int cache_size)
{
	server *s = internal_loader_add_server(lb, server_id, cache_size, NULL);

	if (lb->has_vnodes) {
		internal_loader_add_server(lb, server_id + 100000, cache_size, s);
		internal_loader_add_server(lb, server_id + 200000, cache_size, s);
	}
	// print_servers(lb->servers);
}

void internal_loader_remove_server(load_balancer *main, int server_id)
{
	unsigned int id_hash = hash_uint(&server_id);

	int index = al_find_by(main->servers, &id_hash, &server_id,
												 cmp_server_to_hash, cmp_server_to_id);

	// printf("Index de %d este %d\n", server_id, index);

	int src = index;

	// printf("Vom muta toate fisierele care trebuie din src = %d in dest = %d\n",
	// 			 src, dest);

	server *src_server = al_get(main->servers, src);

	if (src_server->id < 100000) {
		// printf("Bubui %d\n", src_server->id);
		int dest = (index + 1) % main->servers->size;
		server *dest_server = al_get(main->servers, dest);

		while (dest != src && dest_server->id % 100000 == src_server->id % 100000) {
			dest = (1 + dest) % main->servers->size;
			dest_server = al_get(main->servers, dest);
		}
		// printf("Am gasit sa fac transfer cu %d\n", dest_server->id);
		// print_servers(main->servers);

		transfer_files(src_server, dest_server, true);
	}

	al_erase(main->servers, src);
	// print_servers(main->servers);

	free_server(&src_server);
}

void loader_remove_server(load_balancer *main, int server_id)
{
	// print_servers(main->servers);

	internal_loader_remove_server(main, server_id);

	if (main->has_vnodes) {
		internal_loader_remove_server(main, server_id + 100000);
		internal_loader_remove_server(main, server_id + 200000);
	}
	// print_servers(main->servers);
}

response *loader_forward_request(load_balancer *main, request *req)
{
	if (main->servers->size == 0) {
		printf("[d] N-am servere cplm\n");
		return NULL;
	}

	unsigned int hash_document = hash_string(req->doc_name);
	server *srv =
			al_get_ordered(main->servers, &hash_document, cmp_server_to_hash);
	response *res = server_handle_request(srv, req);

	return res;
}

void free_load_balancer(load_balancer **main)
{
	al_free((*main)->servers);

	free(*main);
	*main = NULL;
}
