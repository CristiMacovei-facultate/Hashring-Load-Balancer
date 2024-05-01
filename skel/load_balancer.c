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

void loader_add_server(load_balancer *lb, int server_id, int cache_size)
{
	server *srv = init_server(cache_size);
	srv->id = server_id;

	// al_insert(lb->servers, lb->servers->size, srv);
	int dest_index = al_insert_ordered(lb->servers, srv, cmp_servers);
	free(srv);

	// print_servers(lb->servers);

	int src_index = (dest_index + 1) % lb->servers->size;
	server *src_server = al_get(lb->servers, src_index);
	server *dest_server = al_get(lb->servers, dest_index);

	int index_before_dest =
			(lb->servers->size + dest_index - 1) % lb->servers->size;
	server *before_dest_server = al_get(lb->servers, index_before_dest);
	unsigned int bd_hash = hash_uint(&before_dest_server->id);

	// printf("Vom muta toate fisierele care trebuie din src = %d in dest = %d\n",
	// 			 src_index, dest_index);

	transfer_files(src_server, dest_server, false, bd_hash);

	// print_servers(lb->servers);
}

void loader_remove_server(load_balancer *main, int server_id)
{
	unsigned int id_hash = hash_uint(&server_id);

	int index = al_find_by(main->servers, &id_hash, &server_id,
												 cmp_server_to_hash, cmp_server_to_id);

	// printf("Index de %d este %d\n", server_id, index);

	int src = index;
	int dest = (index + 1) % main->servers->size;
	// printf("Vom muta toate fisierele care trebuie din src = %d in dest = %d\n",
	// 			 src, dest);

	server *src_server = al_get(main->servers, src);
	server *dest_server = al_get(main->servers, dest);

	transfer_files(src_server, dest_server, true, 0);

	al_erase(main->servers, src);
	// print_servers(main->servers);

	free_server(&src_server);
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
