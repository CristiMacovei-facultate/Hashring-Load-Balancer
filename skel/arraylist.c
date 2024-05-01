#include <stdlib.h>
#include <string.h>

#include "arraylist.h"
#include "hashmap.h"
#include "linked_list.h"
#include "server.h"
#include "utils.h"

arraylist_t *al_init(unsigned int data_size, void (*destructor)(void *))
{
	arraylist_t *list = malloc(sizeof(arraylist_t));

	list->capacity = 1;
	list->size = 0;
	list->data_size = data_size;
	list->data = malloc(sizeof(void *) * list->capacity);
	list->destructor = destructor;

	return list;
}

void al_resize(arraylist_t *list, unsigned int new_cap)
{
	list->capacity = new_cap;
	list->data = realloc(list->data, sizeof(void *) * new_cap);
}

void *al_get(arraylist_t *list, unsigned int index)
{
	if (index >= list->size) {
		index = list->size - 1;
	}

	return list->data[index];
}

void *al_get_ordered(arraylist_t *list, void *target,
										 int (*compare)(void *element, void *target))
{
	int left = 0, right = list->size - 1, ans = 0;
	while (left <= right) {
		int mid = (left + right) / 2;
		void *mid_data = al_get(list, mid);

		if (compare(mid_data, target) >= 0) {
			ans = mid;
			right = mid - 1;
		}
		else {
			left = 1 + mid;
		}
	}

	return al_get(list, ans);
}

int al_find_by(arraylist_t *list, void *bs_key, void *target,
							 int (*bs_compare)(void *element, void *target),
							 int (*compare)(void *element, void *target))
{
	// printf("intru aici\n");
	int left = 0, right = list->size - 1, ans = 0;
	while (left <= right) {
		// printf("left= %d, right = %d\n", left, right);
		int mid = (left + right) / 2;
		void *mid_data = al_get(list, mid);

		int bsc_result = bs_compare(mid_data, bs_key);
		if (bs_compare < 0) {
			right = -1 + mid;
		}
		else if (bs_compare > 0) {
			left = 1 + mid;
		}
		else {
			ans = mid;
			left = 1 + mid;
		}
	}

	// printf("gata asta\n");

	for (int i = ans; i < list->size; ++i) {
		void *element = al_get(list, i);
		if (compare(element, target) == 0) {
			return i;
		}
	}
	return -1;
}

void al_erase(arraylist_t *list, unsigned int index)
{
	if (index >= list->size) {
		index = list->size - 1;
	}

	for (int i = index; i < list->size - 1; ++i) {
		list->data[i] = list->data[i + 1];
	}

	--(list->size);
	if (list->size < list->capacity / 2) {
		al_resize(list, list->capacity / 2);
	}
}

void al_insert(arraylist_t *list, unsigned int index, void *data)
{
	unsigned int hash = hash_uint(&((server *)data)->id);
	int id = ((server *)data)->id;
	// printf("Bag server cu hash-ul %u (id = %d) pe pozitia %d\n", hash, id,
	// index);
	void *new_data = malloc(list->data_size);
	memcpy(new_data, data, list->data_size);

	if (list->size == list->capacity) {
		al_resize(list, 2 * list->capacity);
	}

	// printf("Trec de resize, cap = %d\n", list->capacity);

	for (int i = list->size; i > (int)index; --i) {
		// printf("Vreau sa scriu list.data[%d]\n", i);
		list->data[i] = list->data[i - 1];
	}
	list->data[index] = new_data;
	++(list->size);
}

int al_insert_ordered(arraylist_t *list, void *data,
											int (*compare)(void *data1, void *data2))
{
	if (list->size == 0) {
		al_insert(list, 0, data);
		return 0;
	}

	int left = 0, right = list->size - 1, ans = list->size;
	while (left <= right) {
		// printf("Bs: l = %d, r = %d\n", left, right);
		int mid = (left + right) / 2;
		void *mid_data = al_get(list, mid);

		if (compare(mid_data, data) > 0) {
			ans = mid;
			right = mid - 1;
		}
		else {
			left = mid + 1;
		}
	}

	al_insert(list, ans, data);
	return ans;
}

void al_free(arraylist_t *list)
{
	for (int i = 0; i < list->size; ++i) {
		server *s = al_get(list, i);
		list->destructor(s);
	}
	free(list->data);
	free(list);
}

void print_servers(arraylist_t *list)
{
	printf("\n\nDau debug la lista\n");
	for (int i = 0; i < list->size; ++i) {
		server *srv = ((server *)al_get(list, i));
		printf("list[%d] are id = %d (hash = %u)\n", i, srv->id,
					 hash_uint(&srv->id));

		printf("Fisiere:\n");
		for (int j = 0; j < srv->local_db->hmax; ++j) {
			ll_t *list = srv->local_db->buckets[j];
			for (ll_node_t *node = list->head; node; node = node->next) {
				map_info_t *info = node->data;
				char *name = info->key;
				char *cont = info->val;

				printf("'%s': '%s'\n", name, cont);
			}
		}
	}
	printf("\n\n");
}