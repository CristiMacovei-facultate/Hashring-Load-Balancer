/*
Copyright (c) 2024 Nicolae-Cristian MACOVEI nicolae.macovei1412@stud.acs.upb.ro
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

	return result;
}

int cmp_server_to_hash(void *s, void *h)
{
	server *srv = s;
	unsigned int hash_server = hash_uint(&srv->id);
	unsigned int hash = *(unsigned int *)h;

	int result = compare_uints(hash_server, hash);

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
	} else {
		srv = malloc(sizeof(server));
		srv->cache = vnode_ref->cache;
		srv->local_db = vnode_ref->local_db;
		srv->task_queue = vnode_ref->task_queue;
	}
	srv->id = server_id;

	int dest_index = al_insert_ordered(lb->servers, srv, cmp_servers);
	free(srv);

	int src_index = (dest_index + 1) % lb->servers->size;

	server *src_server = al_get(lb->servers, src_index);
	server *dest_server = al_get(lb->servers, dest_index);

	transfer_files(src_server, dest_server, false);

	return dest_server;
}

server *internal_loader_add_server_with_vnodes(load_balancer *lb, int server_id,
																							 int cache_size,
																							 server *vnode_ref)
{
	server *srv = NULL;
	if (!vnode_ref) {
		srv = init_server(cache_size);
	} else {
		srv = malloc(sizeof(server));
		srv->cache = vnode_ref->cache;
		srv->local_db = vnode_ref->local_db;
		srv->task_queue = vnode_ref->task_queue;
	}
	srv->id = server_id;

	int dest_index = al_insert_ordered(lb->servers, srv, cmp_servers);
	free(srv);

	server *dest_server = al_get(lb->servers, dest_index);

	int src_index = (dest_index + 1) % lb->servers->size;
	server *src_server = al_get(lb->servers, src_index);

	server *src_replicas[3];
	unsigned src_id = src_server->id % 100000;
	for (int i = 0; i < 3; ++i) {
		unsigned id = src_id + i * 100000;
		unsigned hash = hash_uint(&id);

		int index = al_find_by(lb->servers, &hash, &id, cmp_server_to_hash,
													 cmp_server_to_id);

		src_replicas[i] = al_get(lb->servers, index);
	}

	transfer_files_multiple_srcs(src_replicas, src_server, dest_server);

	return dest_server;
}

void loader_add_server(load_balancer *lb, int server_id, int cache_size)
{
	if (lb->has_vnodes) {
		server *s =
				internal_loader_add_server_with_vnodes(lb, server_id, cache_size, NULL);
		internal_loader_add_server_with_vnodes(lb, server_id + 100000, cache_size,
																					 s);
		internal_loader_add_server_with_vnodes(lb, server_id + 200000, cache_size,
																					 s);
	} else {
		internal_loader_add_server(lb, server_id, cache_size, NULL);
	}
}

void internal_loader_remove_server(load_balancer *main, int server_id)
{
	unsigned int id_hash = hash_uint(&server_id);

	int index = al_find_by(main->servers, &id_hash, &server_id,
												 cmp_server_to_hash, cmp_server_to_id);

	int src = index;
	int dest = (index + 1) % main->servers->size;

	server *src_server = al_get(main->servers, src);
	server *dest_server = al_get(main->servers, dest);

	transfer_files(src_server, dest_server, true);

	al_erase(main->servers, src);

	free_server(&src_server);
}

void internal_loader_remove_server_with_vnodes(load_balancer *main,
																							 int server_id)
{
	server *dests[3];

	unsigned int id_hash = hash_uint(&server_id);
	int index = al_find_by(main->servers, &id_hash, &server_id,
												 cmp_server_to_hash, cmp_server_to_id);
	int src = index;
	server *src_server = al_get(main->servers, src);

	server_id %= 100000;
	for (int i = 0; i < 3; ++i) {
		int new_id = server_id + 100000 * i;
		id_hash = hash_uint(&new_id);

		index = al_find_by(main->servers, &id_hash, &new_id, cmp_server_to_hash,
											 cmp_server_to_id);
		int dest = (index + 1) % main->servers->size;
		server *dest_server = al_get(main->servers, dest);

		dests[i] = dest_server;
	}

	transfer_files_multiple_dests(src_server, dests);

	// free all replicas
	for (int i = 0; i < 3; ++i) {
		unsigned id = server_id + 100000 * i;
		id_hash = hash_uint(&id);

		index = al_find_by(main->servers, &id_hash, &id, cmp_server_to_hash,
											 cmp_server_to_id);

		server *replica = al_get(main->servers, index);
		al_erase(main->servers, index);

		if (i == 0) {
			free_server(&replica);
		} else {
			free(replica);
		}
	}
}

void loader_remove_server(load_balancer *main, int server_id)
{
	if (main->has_vnodes) {
		internal_loader_remove_server_with_vnodes(main, server_id);
	} else {
		internal_loader_remove_server(main, server_id);
	}
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
