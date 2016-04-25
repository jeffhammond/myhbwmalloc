# HBWMALLOC

## Interface

```
int hbw_check_available(void);
void* hbw_malloc(size_t size);
void* hbw_calloc(size_t nmemb, size_t size);
void* hbw_realloc (void *ptr, size_t size);
void hbw_free(void *ptr);
int hbw_posix_memalign(void **memptr, size_t alignment, size_t size);
```

## Building

I will implement a serious build system later.
```
make -C src && make -C test
```

## Environment Variables

`HBWMALLOC_VERBOSE` - print out extra information, such as when commands succeed.

`HBWMALLOC_SOFTFAIL` - by default, if HBW allocation fails, the library will abort.  Setting this option allows the library to fall back to the default heap when the special heap is not available.

`HBWMALLOC_BYTES` - unimplemented.

`HBWMALLOC_LOCKLESS` - library calls will not be thread-safe, in which case the application must use its own mutual exclusion if calls are made from more than one thread.

Note that `HBWMALLOC_LOCKLESS` is set automatically if MPI is used in single-threaded mode (`MPI_THREAD_SINGLE`).  If you use `MPI_Init`, you are implicitly requesting `MPI_THREAD_SINGLE`.  It is _erroneous_ to use threads in an MPI program in which `MPI_THREAD_SINGLE` is requested.

# Contributors

* Jeff Hammond, Intel - Initial implementation.
* Adrian Jackson, EPCC - Fortran interface.
