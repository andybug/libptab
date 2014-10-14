libptab API
===========

```c
int ptab_init(struct ptab *p)
int ptab_destroy(struct ptab *p)

int ptab_set_allocator(struct ptab_allocator *a)

int ptab_begin_columns(struct ptab *p)
int ptab_define_column(struct ptab *p, const char *name, size_t len, int flags)
int ptab_end_columns(struct ptab *p)

int ptab_begin_row(struct ptab *p)
int ptab_add_row_data(struct ptab *p, const char *val, size_t len)
int ptab_end_row(struct ptab *p)

/*
 * undecided
 * size_t ptab_write_file(struct ptab *p, FILE *stream)
 * size_t ptab_write_buffer(struct ptab *p, char *buf, size_t len)
 */
```
