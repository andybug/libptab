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

#define PTAB_VERSION  "0.1.0"

#define PTAB_OK           (0)
#define PTAB_ENULL       (-1)
#define PTAB_EMEM        (-2)
#define PTAB_EORDER      (-3)
#define PTAB_ERANGE      (-4)
#define PTAB_ETYPE       (-5)
#define PTAB_EALIGN      (-6)
#define PTAB_EFORMAT     (-7)
#define PTAB_ECOLUMNS    (-8)


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

/*
 * ptab_version
 *
 * Returns a statically-allocated string containing the library
 * version in major.minor.patch format (e.g. "1.0.3"). The string
 * is null terminated.
 */
extern PTAB_EXPORT const char *ptab_version(void);

/*
 * ptab_strerror
 *
 * Returns a statically-allocated string that describes the provided
 * error code. Use this function to convert any of the errors produced
 * by ptab library functions into a human-readable form.
 */
extern PTAB_EXPORT const char *ptab_strerror(int err);

/*
 * ptab_init
 *
 * Allocate and initialize a new ptab_t object using the provided memory
 * allocation functions, or use the standard malloc and free functions if
 * NULL is passed for the ptab_allocator_t. NULL is returned if memory
 * could not be acquired from the allocator.
 */
extern PTAB_EXPORT ptab_t *ptab_init(const ptab_allocator_t *a);

/*
 * ptab_free
 *
 * Release the resources held by a ptab_t object, including any
 * ptab_string_t objects associated with this table. Only call this
 * function once.
 */
extern PTAB_EXPORT int ptab_free(ptab_t *p);

/*
 * ptab_free_string
 *
 * Release the resources held by a ptab_string_t object. After calling
 * this function, it is no longer safe to access the str compnent of
 * the structure. Note: it is not necessary to call this function if
 * you will be calling ptab_free afterwards, as that function will clean
 * up all ptab_string_t objects associated with the table.
 */
extern PTAB_EXPORT int ptab_free_string(ptab_t *p, ptab_string_t *s);

/*
 * ptab_column
 *
 * Create a column in the table. The name parameter will be used as the
 * heading for the column and the type will be used to determine the default
 * alignment - strings are left aligned and numbers are right aligned.
 * Future calls to on of the ptab_row_data_* functions must match the type
 * provided here.
 */
extern PTAB_EXPORT int ptab_column(ptab_t *p, const char *name, enum ptab_type t);

/*
 * ptab_column_align
 *
 * Change the alignment of a previously-defined column. Columns are
 * identified by a zero-based index, where the first column is 0, the
 * second 1, and so on.
 */
extern PTAB_EXPORT int ptab_column_align(ptab_t *p, unsigned int col, enum ptab_align a);

/*
 * ptab_begin_row
 *
 * Start adding data to a row. For each column that has been defined,
 * call one of the ptab_row_data_* functions that corresponds to the
 * type of that column. Data for each column must be provided before
 * calling ptab_end_row. See the example code for a demonstration.
 * Note: once the function has been called, new columns can no longer
 * be defined.
 */
extern PTAB_EXPORT int ptab_begin_row(ptab_t *p);

/*
 * ptab_row_data_s
 *
 * Add string data to the row. The column must have been defined with
 * a PTAB_STRING type.
 */
extern PTAB_EXPORT int ptab_row_data_s(ptab_t *p, const char *val);

/*
 * ptab_row_data_i
 *
 * Add integer data to the row. The column must have been defined with
 * a PTAB_INTEGER type. The format string makes use of printf-style
 * format specifiers, so a %d must be provided somewhere in the format
 * string. This allows you to show units such as "%d km" while still
 * maintaining the integer data.
 */
extern PTAB_EXPORT int ptab_row_data_i(ptab_t *p, const char *format, int val);

/*
 * ptab_row_data_f
 *
 * Add float data to the row. The column must have been defined with
 * a PTAB_FLOAT type. The format string makes use of printf-style
 * format specifiers, so a %f must be provided somewhere in the format
 * string. This allows you to show units such as "%f m/s" while still
 * maintaining the float data.
 */
extern PTAB_EXPORT int ptab_row_data_f(ptab_t *p, const char *format, float val);

/*
 * ptab_end_row
 *
 * End a row of data in the table. For each column in the table, a call
 * to one of the ptab_row_data_* functions must have been made. This
 * function acts as a sanity check to make sure that all of the data
 * for the row has been added.
 */
extern PTAB_EXPORT int ptab_end_row(ptab_t *p);

/* Future */
/* extern PTAB_EXPORT int ptab_sort(ptab_t *p, int column, int order); */

/*
 * ptab_dumpf
 *
 * Once the columns and rows have been defined, the table can be generated.
 * This function writes the table to a C standard FILE stream, using the
 * specified table format. Most likely, you will want to pass stdout as
 * the stream.
 */
extern PTAB_EXPORT int ptab_dumpf(ptab_t *p, FILE *stream, enum ptab_format f);

/*
 * ptab_dumps
 *
 * Once the columns and rows have been defined, the table can be generated.
 * This function writes the table to a provided ptab_string_t object (no
 * initialization of the ptab_string_t object is required) using the
 * specified table format. Call ptab_free_string to cleanup the string when
 * done, or call ptab_free to cleanup the table and all of the ptab_string_t
 * objects associated with it.
 */
extern PTAB_EXPORT int ptab_dumps(ptab_t *p, ptab_string_t *s, enum ptab_format f);


#ifdef __cplusplus
}
#endif

#endif /* PTAB_H */
