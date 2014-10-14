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
#define PTABLES_ERR_NULL           (-2)
#define PTABLES_ERR_NOT_BUFFER     (-3)
#define PTABLES_ERR_NOT_ALLOCATOR  (-4)

#define PTABLES_USE_BUFFER     0x1
#define PTABLES_USE_ALLOCATOR  0x2


/* types */

typedef void *(*ptable_alloc_func)(size_t size, void *opaque);
typedef void (*ptable_free_func)(void *p, void *opaque);


/* structures */

struct ptable_allocator {
	ptable_alloc_func alloc_func;
	ptable_free_func free_func;
	void *opaque;
};

struct ptable_allocator_stats {
	size_t total;
	size_t high;
	size_t current;
	unsigned int allocations;
	unsigned int frees;

struct ptable_column {
	int flags;
	size_t width;
	size_t name_len;
	const char name[];
};

struct ptable_row {
	int flags;
	char **column_data;
	size_t *column_len;
	char data[];
};

struct ptable {
	char *buf;
	size_t bufsize;

	struct ptable_column *columns;
	struct ptable_row *rows;
	int num_columns;
	int num_rows;

	struct ptable_allocator allocator;
	struct ptable_allocator_stats allocator_stats;
};


/* functions */

/**
 * Return the string version of the library (e.g. "1.0.3")
 */
extern const char *ptables_version(void);

/* TODO add Doxygen comment */
extern int ptable_init(struct ptable *p);

/* TODO add Doxygen comment */
extern int ptable_set_allocator(struct ptable *p, struct ptable_allocator *a);

#ifdef __cplusplus
}
#endif

#endif /* PTABLES_H */
