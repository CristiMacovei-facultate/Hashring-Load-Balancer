#ifndef ARRAYLIST_H_GUARD
#define ARRAYLIST_H_GUARD 0x69

typedef struct {
	unsigned int data_size;
	unsigned int size;
	unsigned int capacity;
	void (*destructor)(void *);

	void **data;
} arraylist_t;

arraylist_t *al_init(unsigned int data_size, void(destructor)(void *));

void *al_get(arraylist_t *list, unsigned int index);

void al_erase(arraylist_t *list, unsigned int index);

void al_insert(arraylist_t *list, unsigned int index, void *data);

void al_free(arraylist_t *list);

#endif