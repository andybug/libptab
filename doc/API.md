libptab API
===========

```c
const char *ptab_version_string(void)
void ptab_version(int *major, int *minor, int *patch)

int ptab_init(struct ptab *p, const struct ptab_allocator *a)
int ptab_free(struct ptab *p)

int ptab_begin_columns(struct ptab *p)
int ptab_define_column(struct ptab *p, const char *name, const char *fmt, int flags)
int ptab_end_columns(struct ptab *p)

int ptab_begin_row(struct ptab *p)
int ptab_add_row_data_s(struct ptab *p, const char *val)
int ptab_add_row_data_i(struct ptab *p, int val)
int ptab_add_row_data_f(struct ptab *p, double val)
int ptab_end_row(struct ptab *p)

ssize_t ptab_read(struct ptab *p, char *buf, size_t count)
```
