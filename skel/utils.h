/*
Copyright (c) 2024 Nicolae-Cristian MACOVEI nicolae.macovei1412@stud.acs.upb.ro
*/

#ifndef UTILS_H
#define UTILS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"

#define DIE(assertion, call_description)                                       \
	do {                                                                         \
		if (assertion) {                                                           \
			fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                       \
			perror(call_description);                                                \
			exit(errno);                                                             \
		}                                                                          \
	} while (0)

#define PRINT_RESPONSE(response_ptr)                                           \
	({                                                                           \
		if (response_ptr) {                                                        \
			printf(GENERIC_MSG, response_ptr->server_id,                             \
						 response_ptr->server_response, response_ptr->server_id,           \
						 response_ptr->server_log);                                        \
			free(response_ptr->server_response);                                     \
			free(response_ptr->server_log);                                          \
			free(response_ptr);                                                      \
		}                                                                          \
	})

/**
 * @brief Should be used as hash function for server IDs,
 *      to find server's position on the hash ring
 */
unsigned int hash_uint(void *key);

/**
 * @brief Should be used as hash function for document names,
 *      to find the proper server on the hash ring
 */
unsigned int hash_string(void *key);

/**
 * @brief Function to compare two strings
 *
 * @param s1 - first string
 * @param s2 - second string
 * @return int - zero if they match, other value if not
 */
int compare_string(void *s1, void *s2);

/**
 * @brief same as hash_string, but takes a pointer to a string instead (char**)
 *
 * @param string_ptr
 * @return unsigned int
 */
unsigned int hash_string_pointer(void *string_ptr);

/**
 * @brief Similar to compare_string, but takes string pointers instead
 *
 * @param s1 - first string pointer
 * @param s2 - second string pointer
 * @return int - same as compare_string
 */
int compare_string_pointers(void *s1, void *s2);

char *get_request_type_str(request_type req_type);
request_type get_request_type(char *request_type_str);

#endif /* UTILS_H */
