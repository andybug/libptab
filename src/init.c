
#include "internal.h"

ptab_t *ptab_init(const ptab_allocator_t *a)
{
	ptab_t *p;

	/*
	 * if an allocator is provided, make sure
	 * the alloc and free funcs are valid
	 */
	if (a && (!a->alloc_func || !a->free_func))
		return NULL;

	p = mem_init(a);

	return p;
}
