/* ptab.h - interface for the pretty tables library
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

#ifndef PTAB_H
#define PTAB_H

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


/* defines */

#define PTAB_VERSION_STRING "0.0.0"
#define PTAB_VERSION_NUMBER  0x000
#define PTAB_VERSION_MAJOR   0
#define PTAB_VERSION_MINOR   0
#define PTAB_VERSION_PATCH   0

#define PTAB_OK            0
#define PTAB_EOF         (-1)
#define PTAB_ENULL       (-2)
#define PTAB_ENOMEM      (-3)
#define PTAB_EORDER      (-4)
#define PTAB_ETYPE       (-5)
#define PTAB_EALIGN      (-6)
#define PTAB_ENOROWS     (-7)
#define PTAB_ENOCOLUMNS  (-8)

#define PTAB_STRING       0x01
#define PTAB_INTEGER      0x02
#define PTAB_FLOAT        0x04
#define PTAB_ALIGN_RIGHT  0x08
#define PTAB_ALIGN_LEFT   0x10


/* types */

typedef void *(*ptab_alloc_func)(size_t size, void *opaque);
typedef void (*ptab_free_func)(void *p, void *opaque);


/* structures */

struct ptab_allocator {
	ptab_alloc_func alloc_func;
	ptab_free_func free_func;
	void *opaque;
};

struct ptab_allocator_stats {
	size_t total;
	unsigned int allocations;
	unsigned int frees;
};

/* opaque library internals */
struct ptab_internal;

struct ptab {
	struct ptab_internal *internal;
	struct ptab_allocator allocator;
	struct ptab_allocator_stats allocator_stats;
};


/* functions */

/**
 * Return the string version of the library (e.g. "1.0.3-rc1")
 */
extern const char *ptab_version_string(void);

/**
 * Get the version components of the library (e.g. 1, 0, 3)
 */
extern void ptab_version(int *major, int *minor, int *patch);

/* TODO add comment */
extern int ptab_init(struct ptab *p, const struct ptab_allocator *a);

/* TODO add comment */
extern int ptab_free(struct ptab *p);

/* TODO add comment */
extern int ptab_begin_columns(struct ptab *p);

/* TODO add comment */
extern int ptab_define_column(struct ptab *p,
			      const char *name,
			      const char *fmt,
			      int flags);

/* TODO add comment */
extern int ptab_end_columns(struct ptab *p);

/* TODO add comment */
extern int ptab_begin_row(struct ptab *p);

/* TODO add comment */
extern int ptab_add_row_data_s(struct ptab *p, const char *val);

#ifdef __cplusplus
}
#endif

#endif /* PTAB_H */
