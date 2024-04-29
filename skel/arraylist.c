#include <stdlib.h>
#include <string.h>

#include "arraylist.h"
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

void al_erase(arraylist_t *list, unsigned int index)
{
	if (index >= list->size) {
		index = list->size - 1;
	}

	for (int i = (int)list->size - 2; i >= (int)index; --i) {
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
	// printf("Bag server cu hash-ul %u pe pozitia %d\n", hash, index);
	void *new_data = malloc(list->data_size);
	memcpy(new_data, data, list->data_size);

	if (list->size == list->capacity) {
		al_resize(list, 2 * list->capacity);
	}

	for (int i = list->size; i > (int)index; ++i) {
		list->data[i] = list->data[i - 1];
	}
	list->data[index] = new_data;
	++(list->size);
}

void al_insert_ordered(arraylist_t *list, void *data,
											 int (*compare)(void *data1, void *data2))
{
	if (list->size == 0) {
		al_insert(list, 0, data);
		return;
	}

	int left = 0, right = list->size - 1, ans = list->size;
	while (left < right) {
		int mid = (left + right) / 2;
		void *mid_data = al_get(list, mid);

		if (compare(data, mid_data) < 0) {
			ans = mid;
			right = mid - 1;
		}
		else {
			left = mid + 1;
		}
	}

	al_insert(list, ans, data);
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