/* This implementation is based on the tutorial at
 * http://www.jamesmolloy.co.uk/tutorial_html/7.-The%20Heap.html */

#include <types/ordered_array.h>

signed char standard_lessthan_predicate(type_t a, type_t b)
{
	return (a<b)?1:0;
}

ordered_array_t create_ordered_array(unsigned int max_size, lessthan_predicate_t less_than)
{
	struct ordered_array ret;
	ret.array = (type_t*)kmalloc(max_size * sizeof(type_t));
	memset(ret.array, 0, max_size * sizeof(type_t));
	ret.size = 0;
	ret.max_size = max_size;
	ret.less_than = less_than;
	return ret;
}

ordered_array_t place_ordered_array(void* address, unsigned int max_size, lessthan_predicate_t less_than)
{
	struct ordered_array ret;
	ret.array = (type_t*)address;
	memset(ret.array, 0, max_size * sizeof(type_t));
	ret.size = 0;
	ret.max_size = max_size;
	ret.less_than = less_than;
	return ret;
}

void destroy_ordered_array(struct ordered_array* array)
{
	// kfree(array->array);
}

void insert_ordered_array(type_t item, struct ordered_array* array)
{
	ASSERT(array->less_than);
	
	unsigned int iterator = 0;
	while (iterator < array->size && array->less_than(array->array[iterator], item))
		iterator++;
	if (iterator == array->size)
		array->array[array->size++] = item;
	else
	{
		type_t tmp = array->array[iterator];
		array->array[iterator] = item;
		while (iterator < array->size)
		{
			iterator++;
			type_t tmp2 = array->array[iterator];
			array->array[iterator] = tmp;
			tmp = tmp2;
		}
		array->size++;
	}
}

type_t lookup_ordered_array(unsigned int i, struct ordered_array* array)
{
	ASSERT(i < array->size);

	return array->array[i];
}

void remove_ordered_array(unsigned int i, struct ordered_array* array)
{
	while (i < array->size)
	{
		array->array[i] = array->array[i+1];
		i++;
	}
	array->size--;
}
