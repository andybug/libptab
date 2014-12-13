libptab API
===========

```c
const char *ptab_version_string(void)
void ptab_version(int *major, int *minor, int *patch)

const char *ptab_strerr(int err)

int ptab_init(ptab *p, const ptab_allocator *a)
int ptab_free(ptab *p)

int ptab_column(ptab *p, const char *name, int flags)

int ptab_begin_row(ptab *p)
int ptab_row_data_s(ptab *p, const char *s)
int ptab_row_data_i(ptab *p, const char *format, int i)
int ptab_row_data_f(ptab *p, const char *format, double f)
int ptab_end_row(ptab *p)

int ptab_sort(ptab *p, int column, int order)

int ptab_dumpf(ptab *p, FILE *stream, int flags)
int ptab_dumps(ptab *p, ptab_stream_t *stream, int flags)

int ptab_stream_open(ptab *p, ptab_stream_t *stream, int flags)
int ptab_stream_read(ptab *p, ptab_stream_t *stream)
int ptab_stream_close(ptab *p, ptab_stream_t *stream)
```
