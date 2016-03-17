#include <stddef.h> /* size_t */

/* taken from dlmalloc.c */
typedef void* mspace;
size_t destroy_mspace(mspace msp);
mspace create_mspace_with_base(void* base, size_t capacity, int locked);
int mspace_track_large_chunks(mspace msp, int enable);
void* mspace_malloc(mspace msp, size_t bytes);
void mspace_free(mspace msp, void* mem);
void* mspace_realloc(mspace msp, void* mem, size_t newsize);
void* mspace_calloc(mspace msp, size_t n_elements, size_t elem_size);
void* mspace_memalign(mspace msp, size_t alignment, size_t bytes);
#if 0 /* unused */
mspace create_mspace(size_t capacity, int locked);
void** mspace_independent_calloc(mspace msp, size_t n_elements,
void** mspace_independent_comalloc(mspace msp, size_t n_elements,
size_t mspace_footprint(mspace msp);
size_t mspace_max_footprint(mspace msp);
size_t mspace_usable_size(const void* mem);
void mspace_malloc_stats(mspace msp);
int mspace_trim(mspace msp, size_t pad);
int mspace_mallopt(int, int);
/* needs mallinfo declaration
 * struct mallinfo mspace_mallinfo(mspace msp); */
#endif
