ptables API
===========

```c
int ptable_init(struct ptable *p)
int ptable_destroy(struct ptable *p)

int ptable_set_allocator(struct ptable_allocator *a)

int ptable_begin_columns(struct ptable *p)
int ptable_add_column_data(struct ptable *p, const char *name, size_t len, int flags)
int ptable_end_columns(struct ptable *p)

int ptable_begin_row(struct ptable *p)
int ptable_add_row_data(struct ptable *p, const char *val, size_t len, int flags)
int ptable_end_row(struct ptable *p)

/*
 * undecided
 * size_t ptable_write_file(struct ptable *p, FILE *stream)
 * size_t ptable_write_buffer(struct ptable *p, char *buf, size_t len)
 */
```
