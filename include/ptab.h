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

#define PTAB_VERSION       "0.0.0"

#define PTAB_OK                (0)
#define PTAB_EOF              (-1)
#define PTAB_ENULL            (-2)
#define PTAB_ENOMEM           (-3)
#define PTAB_EINIT            (-4)
#define PTAB_ETYPEFLAGS       (-5)
#define PTAB_EALIGNFLAGS      (-6)
#define PTAB_EROWS            (-7)
#define PTAB_ENOCOLUMNS       (-8)
#define PTAB_EROWBEGAN        (-9)
#define PTAB_ENUMCOLUMNS     (-10)
#define PTAB_ETYPE           (-11)
#define PTAB_ENOROWBEGAN     (-12)
#define PTAB_EFORMATFLAGS    (-13)

#ifdef __linux__
#  define PTAB_EXPORT __attribute__ ((visibility("default")))
#endif


/* enums */

enum ptab_type {
	PTAB_STRING  = 1,
	PTAB_INTEGER = 2,
	PTAB_FLOAT   = 3
};

enum ptab_align {
	PTAB_LEFT   = 1,
	PTAB_RIGHT  = 2,
	PTAB_CENTER = 3
};

enum ptab_format {
	PTAB_ASCII   = 1,
	PTAB_UNICODE = 2
};


/* types */

typedef void *(*ptab_alloc_func)(size_t size, void *opaque);
typedef void (*ptab_free_func)(void *p, void *opaque);

/* opaque library internals */
typedef struct ptab_internal ptab_t;


/* structures */

typedef struct ptab_allocator {
	ptab_alloc_func alloc_func;
	ptab_free_func free_func;
	void *opaque;
} ptab_allocator_t;

typedef struct ptab_string {
	const char *str;
	size_t len;
} ptab_string_t;


/* functions */

/**
 * Return the string version of the library (e.g. "1.0.3-rc1")
 */
extern PTAB_EXPORT const char *ptab_version(void);

/* TODO add comment */
extern PTAB_EXPORT const char *ptab_strerror(int err);

/* TODO add comment */
extern PTAB_EXPORT ptab_t *ptab_init(const ptab_allocator_t *a);

/* TODO add comment */
extern PTAB_EXPORT int ptab_free(ptab_t *p);

/* TODO add comment */
extern PTAB_EXPORT int ptab_free_string(ptab_string_t *s);

/* TODO add comment */
extern PTAB_EXPORT int ptab_column(ptab_t *p, const char *name, enum ptab_type t);

/* TODO add comment */
extern PTAB_EXPORT int ptab_column_align(ptab_t *p, unsigned int col, enum ptab_align a);

/* TODO add comment */
extern PTAB_EXPORT int ptab_begin_row(ptab_t *p);

/* TODO add comment */
extern PTAB_EXPORT int ptab_row_data_s(ptab_t *p, const char *val);

/* TODO add comment */
extern PTAB_EXPORT int ptab_row_data_i(ptab_t *p, const char *format, int val);

/* TODO add comment */
extern PTAB_EXPORT int ptab_row_data_f(ptab_t *p, const char *format, float val);

/* TODO add comment */
extern PTAB_EXPORT int ptab_end_row(ptab_t *p);

/* Future */
/* extern PTAB_EXPORT int ptab_sort(ptab_t *p, int column, int order); */

/* TODO add comment */
extern PTAB_EXPORT int ptab_dumpf(ptab_t *p, FILE *stream, enum ptab_format f);

/* TODO add comment */
extern PTAB_EXPORT int ptab_dumps(ptab_t *p, ptab_string_t *s, enum ptab_format f);


#ifdef __cplusplus
}
#endif

#endif /* PTAB_H */
