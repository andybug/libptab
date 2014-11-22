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
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif


/* defines */

#define PTAB_VERSION_STRING "0.0.0"
#define PTAB_VERSION_NUMBER  0x000
#define PTAB_VERSION_MAJOR   0
#define PTAB_VERSION_MINOR   0
#define PTAB_VERSION_PATCH   0

#define PTAB_OK              0
#define PTAB_EOF           (-1)
#define PTAB_ENULL         (-2)
#define PTAB_ENOMEM        (-3)
#define PTAB_EINIT         (-4)
#define PTAB_ETYPEFLAGS    (-5)
#define PTAB_EALIGNFLAGS   (-6)
#define PTAB_EROWS         (-7)
#define PTAB_ENOCOLUMNS    (-8)
#define PTAB_EROWBEGAN     (-9)

#define PTAB_STRING       0x001
#define PTAB_INTEGER      0x002
#define PTAB_FLOAT        0x004
#define PTAB_ALIGN_LEFT   0x100
#define PTAB_ALIGN_RIGHT  0x200

#ifdef __linux__
#  define PTAB_EXPORT __attribute__ ((visibility("default")))
#endif


/* types */

typedef void *(*ptab_alloc_func)(size_t size, void *opaque);
typedef void (*ptab_free_func)(void *p, void *opaque);


/* structures */

typedef struct ptab_allocator_s {
	ptab_alloc_func alloc_func;
	ptab_free_func free_func;
	void *opaque;
} ptab_allocator;

typedef struct ptab_allocator_stats_s {
	size_t allocated;
	size_t used;
	unsigned int num_allocations;
} ptab_allocator_stats;

/* opaque library internals */
struct ptab_internal_s;

typedef struct ptab_s {
	struct ptab_internal_s *internal;
	struct ptab_allocator_s allocator;
	struct ptab_allocator_stats_s allocator_stats;
} ptab;


/* functions */

/**
 * Return the string version of the library (e.g. "1.0.3-rc1")
 */
extern PTAB_EXPORT const char *ptab_version_string(void);

/**
 * Get the version components of the library (e.g. 1, 0, 3)
 */
extern PTAB_EXPORT void ptab_version(int *major, int *minor, int *patch);

/* TODO add comment */
extern PTAB_EXPORT const char *ptab_strerror(int err);

/* TODO add comment */
extern PTAB_EXPORT int ptab_init(ptab *p, const ptab_allocator *a);

/* TODO add comment */
extern PTAB_EXPORT int ptab_free(ptab *p);

/* TODO add comment */
extern PTAB_EXPORT int ptab_column(ptab *p, const char *name, int flags);

/* TODO add comment */
extern PTAB_EXPORT int ptab_begin_row(ptab *p);

/* TODO add comment */
extern PTAB_EXPORT int ptab_row_data_s(ptab *p, const char *val);

/* TODO add comment */
extern PTAB_EXPORT int ptab_row_data_i(ptab *p, const char *format, int val);

/* TODO add comment */
extern PTAB_EXPORT int ptab_row_data_f(ptab *p, const char *format, float val);

/* TODO add comment */
extern PTAB_EXPORT int ptab_end_row(ptab *p);

/* TODO add comment */
extern PTAB_EXPORT int ptab_sort(ptab *p, int column, int order);

/* TODO add comment */
extern PTAB_EXPORT int ptab_write(ptab *p, FILE *stream, int flags);

#ifdef __cplusplus
}
#endif

#endif /* PTAB_H */
