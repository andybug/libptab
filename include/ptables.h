/* ptables.h - interface for the pretty tables library
 * version 0.0.0, Oct 9th, 2014
 *
 * Copyright (C) 2014 Andrew Fields
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef PTABLES_H
#define PTABLES_H

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


/* defines */

#define PTABLES_VERSION_STRING "0.0.0"
#define PTABLES_VERSION_NUMBER 0x000
#define PTABLES_VERSION_MAJOR 0
#define PTABLES_VERSION_MINOR 0
#define PTABLES_VERSION_PATCH 0

#define PTABLES_OK                   0
#define PTABLES_ERR_ONE_ALLOCATOR  (-1)

#define PTABLES_USE_BUFFER     0x1
#define PTABLES_USE_ALLOCATOR  0x2


/* types */

struct ptable;

typedef void *(*ptable_alloc_func)(struct ptable *, size_t, void *);
typedef void (*ptable_free_func)(struct ptable *, void *, void *);


/* structures */

struct ptable_buffer {
	char *buf;
	size_t size;
	size_t used;
	size_t avail;
};

struct ptable {
	int columns;
	int rows;

	struct ptable_buffer buffer;

	size_t alloc_total;
	ptable_alloc_func alloc_func;
	ptable_free_func free_func;
	void *opaque;
};


/* functions */

/* TODO add Doxygen comment */
extern const char *ptables_version(void);

/* TODO add Doxygen comment */
extern int ptable_init(struct ptable *p, int flags);

/* TODO add Doxygen comment */
extern int ptable_buffer_set(struct ptable *p, char *buf, size_t size);

/* TODO add Doxygen comment */
extern int ptable_allocator_set(
	struct ptable *p,
	ptable_alloc_func alloc_func,
	ptable_free_func free_func,
	void *opaque);


#ifdef __cplusplus
}
#endif

#endif /* PTABLES_H */
