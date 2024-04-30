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

void *al_get_ordered(arraylist_t *list, void *target,
										 int (*compare)(void *element, void *target));

int al_find_by(arraylist_t *list, void *bs_key, void *target,
							 int (*bs_compare)(void *element, void *target),
							 int (*compare)(void *element, void *target));

void al_erase(arraylist_t *list, unsigned int index);

void al_insert(arraylist_t *list, unsigned int index, void *data);

int al_insert_ordered(arraylist_t *list, void *data,
											int (*compare)(void *data1, void *data2));

void al_free(arraylist_t *list);

void print_servers(arraylist_t *list);

#endif