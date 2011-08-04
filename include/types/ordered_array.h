/* This implementation is based on the tutorial at
 * http://www.jamesmolloy.co.uk/tutorial_html/7.-The%20Heap.html */

#ifndef __ORDERED_ARRAY_H
#define __ORDERED_ARRAY_H

#include <system.h>

/* This array is insertion sorted, it always remains in a sorted state (between calls).
 * It can store anything that can be cast to void*, so a unsigned int or any pointer */
typedef void* type_t;

/* A predicate should return nonzero if the first argument is less than the second, else
 * it should return zero */
typedef signed char (*lessthan_predicate_t)(type_t, type_t);
typedef struct ordered_array
{
	type_t* array;
	unsigned int size;
	unsigned int max_size;
	lessthan_predicate_t less_than;
} ordered_array_t;

/* A standard less than predicate */
signed char standard_lessthan_predicate(type_t a, type_t b);

/* Create an ordered array */
ordered_array_t create_ordered_array(unsigned int max_size, lessthan_predicate_t less_than);
ordered_array_t place_ordered_array(void* address, unsigned int max_size, lessthan_predicate_t less_than);

/* Destroy an ordered array */
void destroy_ordered_array(struct ordered_array* array);

/* Add an item into the array */
void insert_ordered_array(type_t item, struct ordered_array* array);

/* Lookup the item at index i */
type_t lookup_ordered_array(unsigned int i, struct ordered_array* array);

/* Deletes the item at location i from the array */
void remove_ordered_array(unsigned int i, struct ordered_array* array);

#endif
