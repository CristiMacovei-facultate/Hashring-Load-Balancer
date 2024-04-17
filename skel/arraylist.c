#include <stdlib.h>
#include <string.h>

#include "arraylist.h"

arraylist_t *al_init(unsigned int data_size)
{
	arraylist_t *list = malloc(sizeof(arraylist_t));

	list->capacity = 1;
	list->size = 0;
	list->data_size = data_size;
	list->data = malloc(sizeof(void *) * list->capacity);

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