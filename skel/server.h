/*
Copyright (c) 2024 Nicolae-Cristian MACOVEI nicolae.macovei1412@stud.acs.upb.ro
*/

#ifndef SERVER_H
#define SERVER_H

#include "constants.h"
#include "hashmap.h"
#include "lru_cache.h"
#include "queue.h"
#include "utils.h"

#define TASK_QUEUE_SIZE 1000
#define MAX_LOG_LENGTH 1000
#define MAX_RESPONSE_LENGTH 4096

typedef struct server {
	unsigned int id;

	hashmap_t *local_db;
	lru_cache *cache;
	queue_t *task_queue;
} server;

typedef struct request {
	request_type type;
	char *doc_name;
	char *doc_content;
} request;

typedef struct response {
	char *server_log;
	char *server_response;
	int server_id;
} response;

server *init_server(unsigned int cache_size);

/**
 * @brief Should deallocate completely the memory used by server,
 *     taking care of deallocating the elements in the queue, if any,
 *     without executing the tasks
 */
void free_server(server **s);

/**
 * server_handle_request() - Receives a request from the load balancer
 *      and processes it according to the request type
 *
 * @param s: Server which processes the request.
 * @param req: Request to be processed.
 *
 * @return response*: Response of the requested operation, which will
 *      then be printed in main.
 *
 * @brief Based on the type of request, should call the appropriate
 *     solver, and should execute the tasks from queue if needed (in
 *     this case, after executing each task, PRINT_RESPONSE should
 *     be called).
 */
response *server_handle_request(server *s, request *req);

/**
 * @brief Function to transfer the files from a server to another server.
 * This gets called when either the source server is removed, or the destination
 * just got added to our topology
 *
 * @param src - the source server
 * @param dest - the destination server
 * @param force_move - boolean value which tells whether to move all the files
 * regardless of hashes (source was removed so all files go to the next server)
 * or to check if we should move to dest or leave files where they were (when
 * dest just got added)
 * @details This function sadly doesn't work when vnodes are involved (see the
 * next two functions)
 */
void transfer_files(server *src, server *dest, bool force_move);

/**
 * @brief Function to transfer all the files from a server to multiple
 * destinations, automatically choosing which destination to send to based on
 * hashes
 *
 * @param src - the source server
 * @param dests - array of server*, containing all posibile destinations for
 * every file
 *
 * @details This function was only used for removing a server when there's
 * vnodes involved. Simply calling the transfer_files function doesn't work
 * because there are three possible destinations when we have replicas and we
 * don't know which one to send to, which leads to files ending up on the wrong
 * server.
 */
void transfer_files_multiple_dests(server *src, server **dests);

/**
 * @brief Function to transfer only the files that need to be transferred from a
 * source server to a destination, but with additional checks (see details).
 *
 * @param replicas - all the possible sources for the files
 * @param src - the source we want to send from
 * @param dest - the destination server
 *
 * @details This function is used when adding a new server to the topology with
 * vnodes. Again, inserting each vnode and calling the transfer_files function
 * with the source being the node before it doesn't work because the node before
 * contains all of its files, even the ones that "belong" to its replicas (which
 * it obviously it shouldn't send, but I couldn't find a way to figure that out
 * from hashes alone). So this function takes an array of possible "owners" of
 * each file (all replicas of src) and for each file, before sending it over as
 * src, it checks whether src is the actual "owner" of the file.
 */
void transfer_files_multiple_srcs(server **replicas, server *src, server *dest);

#endif /* SERVER_H */
